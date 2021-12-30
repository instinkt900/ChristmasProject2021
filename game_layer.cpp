#include "game_pch.h"
#include "game_layer.h"
#include "components.h"
#include "layer_stack.h"
#include "state_pre_game.h"
#include "state_game.h"
#include "state_post_game.h"
#include "sprite_system.h"
#include "utils.h"

GameLayer::GameLayer(SDL_Renderer* renderer)
:m_renderer(renderer)
,m_random(0) {
    m_registry.on_destroy<SpriteComponent>().connect<&GameLayer::DestroySprite>(this);

    SDL_Surface* bg = IMG_Load("background.jpg");
    m_backgroundTexture = SDL_CreateTextureFromSurface(m_renderer, bg);
    SDL_FreeSurface(bg);

    SDL_Surface* explosion = IMG_Load("exp2_0.png");
    m_explosionTexture = SDL_CreateTextureFromSurface(m_renderer, explosion);
    SDL_FreeSurface(explosion);

    m_tileMap = std::make_unique<TileMap>(m_renderer, 8, 8);

    m_stateMachine.AddState<StatePreGame>(*this);
    m_stateMachine.AddState<StateGame>(*this);
    m_stateMachine.AddState<StatePostGame>(*this);

    m_scoreFont = FC_CreateFont();
    FC_LoadFont(m_scoreFont, m_renderer, m_worldParameters.m_gameFontPath.c_str(), 20, FC_MakeColor(255, 255, 255, 255), TTF_STYLE_NORMAL);

    m_weaponSFX = Mix_LoadWAV("Pew__006.wav");
    Mix_VolumeChunk(m_weaponSFX, 16);

    m_explosionSFX = Mix_LoadWAV("Explosion2__006.wav");
    m_playerDiedSFX = Mix_LoadWAV("Explosion2__007.wav");
    Mix_VolumeChunk(m_playerDiedSFX, 16);

    m_countSFX = Mix_LoadWAV("Pickup__010.wav");
    m_startSFX = Mix_LoadWAV("Pickup__003.wav");

    m_music = Mix_LoadMUS("OutThere.ogg");

    if (std::filesystem::exists(m_worldParameters.m_dataFile)) {
        std::fstream scoreData(m_worldParameters.m_dataFile, std::ios_base::in | std::ios_base::binary);
        scoreData.read(reinterpret_cast<char*>(&m_worldState.m_highScore), sizeof(m_worldState.m_highScore));
        scoreData.close();
    }
}

GameLayer::~GameLayer() {
    SDL_DestroyTexture(m_backgroundTexture);
    FC_FreeFont(m_scoreFont);
    Mix_FreeMusic(m_music);
    Mix_FreeChunk(m_weaponSFX);
    Mix_FreeChunk(m_explosionSFX);
    Mix_FreeChunk(m_playerDiedSFX);
    Mix_FreeChunk(m_countSFX);
    Mix_FreeChunk(m_startSFX);
}

bool GameLayer::OnEvent(SDL_Event const& event) {
    if (event.type == SDL_WINDOWEVENT) {
        switch (event.window.event) {
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            auto& cameraComponent = m_registry.get<CameraComponent>(m_cameraEntity);
            cameraComponent.width = event.window.data1;
            cameraComponent.height = event.window.data2;
            break;
        }
    }

    return m_stateMachine.OnEvent(event);
}

void GameLayer::Update(uint32_t ticks) {
    m_stateMachine.Update(ticks, m_registry);

    SpriteSystem::Update(ticks, *this);

    // clean up dead entities
    std::vector<entt::entity> deadEntities;
    m_registry.view<HealthComponent>().each([&deadEntities](auto entity, auto const& healthComponent) {
        if (!healthComponent.alive) {
            deadEntities.push_back(entity);
        }
        });

    for (auto entity : deadEntities) {
        m_registry.destroy(entity);
    }

    if (m_stateMachine.IsInState<StateGame>() && !m_registry.valid(m_playerEntity))
    {
        // game over
        Mix_PlayChannel(-1, m_playerDiedSFX, 0);
        m_stateMachine.StateTransition<StatePostGame>();
    }
}

void GameLayer::Draw(SDL_Renderer* renderer) {
    SDL_RenderCopy(renderer, m_backgroundTexture, nullptr, nullptr);

    auto const& cameraPositionComponent = m_registry.get<PositionComponent>(m_cameraEntity);
    auto const& cameraCameraComponent = m_registry.get<CameraComponent>(m_cameraEntity);
    ViewParameters viewParameters;
    viewParameters.m_width = cameraCameraComponent.width;
    viewParameters.m_height = cameraCameraComponent.height;
    viewParameters.m_offsetX = static_cast<int>(cameraPositionComponent.x) - viewParameters.m_width / 2;
    viewParameters.m_offsetY = static_cast<int>(cameraPositionComponent.y) - viewParameters.m_height / 2;

    m_tileMap->Draw(m_renderer, viewParameters);

    SpriteSystem::Draw(m_renderer, m_registry, viewParameters);

    m_stateMachine.Draw(renderer);

    FC_Draw(m_scoreFont, renderer, 3, 0, "Score: %d", m_worldState.m_score);
    FC_Draw(m_scoreFont, renderer, 3, GetHeight() - 22.0f, "High Score: %d", m_worldState.m_highScore);
}

void GameLayer::OnAddedToStack(LayerStack* layerStack) {
    Layer::OnAddedToStack(layerStack);
    m_stateMachine.StateTransition<StatePreGame>();
    Mix_PlayMusic(m_music, -1);
}

void GameLayer::OnRemovedFromStack() {
    Layer::OnRemovedFromStack();
    Mix_PauseMusic();
}

void GameLayer::SpawnExplosion(int x, int y) {
    auto entity = m_registry.create();
    auto& positionComponent = m_registry.emplace<PositionComponent>(entity);
    positionComponent.x = static_cast<float>(x);
    positionComponent.y = static_cast<float>(y);
    auto& spriteComponent = m_registry.emplace<SpriteComponent>(entity);
    spriteComponent.texture = m_explosionTexture;
    spriteComponent.managed_texture = true;
    spriteComponent.width = 64;
    spriteComponent.height = 64;
    spriteComponent.frames = 16;
    spriteComponent.ticks_per_frame = 1000 / spriteComponent.frames;
    spriteComponent.playing = true;
    spriteComponent.anim_type = AnimType::Stop;
    int srcTop = 0;
    int srcLeft = 0;
    spriteComponent.source_rects.resize(spriteComponent.frames);
    for (int i = 0; i < spriteComponent.frames; ++i) {
        auto& rect = spriteComponent.source_rects[i];
        rect.x = srcLeft;
        rect.y = srcTop;
        rect.w = 64;
        rect.h = 64;

        srcLeft += 64;
        if (((i + 1) % 4) == 0) {
            srcLeft = 0;
            srcTop += 64;
        }
    }
    auto& lifetimeComponent = m_registry.emplace<LifetimeComponent>(entity);
    lifetimeComponent.lifetime = spriteComponent.ticks_per_frame * spriteComponent.frames;

    Mix_PlayChannel(-1, m_explosionSFX, 0);
}

void GameLayer::SetupLevel() {
    m_registry.clear();
    m_random.Reset(m_worldParameters.m_seed);

    // camera
    m_cameraEntity = m_registry.create();
    auto& cameraPosition = m_registry.emplace<PositionComponent>(m_cameraEntity);
    auto& cameraDetails = m_registry.emplace<CameraComponent>(m_cameraEntity);
    m_registry.emplace<VelocityComponent>(m_cameraEntity);

    cameraPosition.x = 0;
    cameraPosition.y = 0;
    
    cameraDetails.width = 640;
    cameraDetails.height = 480;
    cameraDetails.active = true;

    // player
    m_playerEntity = m_registry.create();
    m_registry.emplace<HealthComponent>(m_playerEntity);
    m_registry.emplace<VelocityComponent>(m_playerEntity);
    auto& playerPositionComponent = m_registry.emplace<PositionComponent>(m_playerEntity);
    auto& playerCollisionComponent = m_registry.emplace<CollisionComponent>(m_playerEntity);
    auto& playerSpriteComponent = m_registry.emplace<SpriteComponent>(m_playerEntity);
    auto& playerWeaponComponent = m_registry.emplace<WeaponComponent>(m_playerEntity);

    SDL_Surface* playerImage = IMG_Load(m_worldParameters.m_playerSpritePath.c_str());
    playerSpriteComponent.texture = SDL_CreateTextureFromSurface(m_renderer, playerImage);
    SDL_FreeSurface(playerImage);
    playerSpriteComponent.width = m_worldParameters.m_playerSpriteWidth;
    playerSpriteComponent.height = m_worldParameters.m_playerSpriteHeight;

    playerPositionComponent.x = 0;
    playerPositionComponent.y = 0;

    playerCollisionComponent.width = m_worldParameters.m_playerCollisionWidth;
    playerCollisionComponent.height = m_worldParameters.m_playerCollisionHeight;
    playerCollisionComponent.flags = COLLISION_FLAG_PLAYER;
    playerCollisionComponent.flag_mask = COLLISION_FLAG_MAP | COLLISION_FLAG_ENEMY;
    playerCollisionComponent.on_collision = [this](entt::entity otherEntity) {
        auto& healthComponent = m_registry.get<HealthComponent>(m_playerEntity);
        healthComponent.alive = false;
        auto const& positionComponent = m_registry.get<PositionComponent>(m_playerEntity);
        SpawnExplosion(static_cast<int>(positionComponent.x), static_cast<int>(positionComponent.y));
    };

    playerWeaponComponent.fire_delay = m_worldParameters.m_playerFireDelay;
    playerWeaponComponent.velocity = m_worldParameters.m_playerBulletSpeed;

    m_worldState.m_levelSpeed = m_worldParameters.m_levelSpeedInit;
    m_worldState.m_enemySpawnDelayMin = static_cast<float>(m_worldParameters.m_enemySpawnDelayMinInit);
    m_worldState.m_enemySpawnDelayMax = static_cast<float>(m_worldParameters.m_enemySpawnDelayMaxInit);
    m_worldState.m_enemySpawnTimer = static_cast<uint32_t>(m_random.Range(m_worldState.m_enemySpawnDelayMin, m_worldState.m_enemySpawnDelayMax));
    m_worldState.m_enemyCurrentSpeed = m_worldParameters.m_enemySpeedInit;
    m_worldState.m_score = 0;
}

void GameLayer::SaveScore() {
    std::fstream scoreData(m_worldParameters.m_dataFile, std::fstream::out | std::fstream::binary | std::fstream::trunc);
    if (scoreData.is_open()) {
        scoreData.write(reinterpret_cast<char const*>(&m_worldState.m_highScore), sizeof(m_worldState.m_highScore));
        scoreData.close();
    }
}

void GameLayer::DestroySprite(entt::registry& registry, entt::entity entity) {
    auto& spriteComponent = m_registry.get<SpriteComponent>(entity);
    if (!spriteComponent.managed_texture) {
        SDL_DestroyTexture(spriteComponent.texture);
    }
}