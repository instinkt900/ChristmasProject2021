#include "game_pch.h"
#include "game_layer.h"
#include "layer_stack.h"
#include "utils.h"
#include "tile_map.h"
#include "ecs/components/components.h"
#include "ecs/systems/sprite_system.h"
#include "ecs/systems/animation_system.h"
#include "states/state_pre_game.h"
#include "states/state_game.h"
#include "states/state_post_game.h"

GameLayer::GameLayer(SDL_Renderer& renderer)
    : m_renderer(renderer)
    , m_random(0) {
    m_backgroundTexture = CreateTextureRef(&m_renderer, "background.jpg");
    m_explosionTexture = CreateTextureRef(&m_renderer, "exp2_0.png");

    m_tileMap = std::make_unique<TileMap>(m_renderer, 8, 8);

    m_stateMachine.AddState<StatePreGame>(*this);
    m_stateMachine.AddState<StateGame>(*this);
    m_stateMachine.AddState<StatePostGame>(*this);

    m_scoreFont = CreateCachedFontRef(&m_renderer, m_worldParameters.m_gameFontPath.c_str(), 20, FC_MakeColor(255, 255, 255, 255), TTF_STYLE_NORMAL);

    m_weaponSFX = CreateAudioRef("Pew__006.wav");
    Mix_VolumeChunk(m_weaponSFX.get(), 16);

    m_explosionSFX = CreateAudioRef("Explosion2__006.wav");
    m_playerDiedSFX = CreateAudioRef("Explosion2__007.wav");
    Mix_VolumeChunk(m_playerDiedSFX.get(), 16);

    m_countSFX = CreateAudioRef("Pickup__010.wav");
    m_startSFX = CreateAudioRef("Pickup__003.wav");

    m_music = CreateMusicRef("OutThere.ogg");

    if (std::filesystem::exists(m_worldParameters.m_dataFile)) {
        std::fstream scoreData(m_worldParameters.m_dataFile, std::ios_base::in | std::ios_base::binary);
        scoreData.read(reinterpret_cast<char*>(&m_worldState.m_highScore), sizeof(m_worldState.m_highScore));
        scoreData.close();
    }
}

GameLayer::~GameLayer() {
}

bool GameLayer::OnEvent(SDL_Event const& event) {
    return m_stateMachine.OnEvent(event);
}

void GameLayer::Update(uint32_t ticks) {
    m_stateMachine.Update(ticks, m_registry);

    AnimationSystem::Update(ticks, *this);

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

    if (m_stateMachine.IsInState<StateGame>() && !m_registry.valid(m_playerEntity)) {
        // game over
        Mix_PlayChannel(-1, m_playerDiedSFX.get(), 0);
        m_stateMachine.StateTransition<StatePostGame>();
    }
}

void GameLayer::Draw(SDL_Renderer& renderer) {
    SDL_RenderCopy(&m_renderer, m_backgroundTexture.get(), nullptr, nullptr);

    auto const& cameraPositionComponent = m_registry.get<PositionComponent>(m_cameraEntity);
    auto const& cameraCameraComponent = m_registry.get<CameraComponent>(m_cameraEntity);
    ViewParameters viewParameters;
    viewParameters.m_width = GetWidth();
    viewParameters.m_height = GetHeight();
    viewParameters.m_offsetX = static_cast<int>(cameraPositionComponent.x) - viewParameters.m_width / 2;
    viewParameters.m_offsetY = static_cast<int>(cameraPositionComponent.y) - viewParameters.m_height / 2;

    m_tileMap->Draw(viewParameters);

    SpriteSystem::Draw(m_renderer, m_registry, viewParameters);

    m_stateMachine.Draw(m_renderer);

    FC_Draw(m_scoreFont.get(), &m_renderer, 3, 0, "Score: %d", m_worldState.m_score);
    FC_Draw(m_scoreFont.get(), &m_renderer, 3, GetHeight() - 22.0f, "High Score: %d", m_worldState.m_highScore);
}

void GameLayer::OnAddedToStack(LayerStack* layerStack) {
    Layer::OnAddedToStack(layerStack);
    m_stateMachine.StateTransition<StatePreGame>();
    Mix_PlayMusic(m_music.get(), -1);
}

void GameLayer::OnRemovedFromStack() {
    Layer::OnRemovedFromStack();
    Mix_PauseMusic();
}

void GameLayer::SpawnExplosion(int x, int y, bool playSound) {
    auto entity = m_registry.create();
    auto& positionComponent = m_registry.emplace<PositionComponent>(entity);
    positionComponent.x = static_cast<float>(x);
    positionComponent.y = static_cast<float>(y);
    auto& spriteComponent = m_registry.emplace<SpriteComponent>(entity);
    spriteComponent.texture = m_explosionTexture;
    spriteComponent.width = 64;
    spriteComponent.height = 64;
    auto& animComponent = m_registry.emplace<AnimationComponent>(entity);
    int const frameCount = 16;
    animComponent.ticks_per_frame = 1000 / frameCount;
    animComponent.playing = true;
    animComponent.anim_type = AnimType::Stop;
    int srcTop = 0;
    int srcLeft = 0;
    animComponent.frames.resize(frameCount);
    for (int i = 0; i < frameCount; ++i) {
        auto& rect = animComponent.frames[i];
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
    lifetimeComponent.lifetime = animComponent.ticks_per_frame * frameCount;

    if (playSound) {
        Mix_PlayChannel(-1, m_explosionSFX.get(), 0);
    }
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

    cameraDetails.active = true;

    // player
    m_playerEntity = m_registry.create();
    m_registry.emplace<HealthComponent>(m_playerEntity);
    m_registry.emplace<VelocityComponent>(m_playerEntity);
    auto& playerPositionComponent = m_registry.emplace<PositionComponent>(m_playerEntity);
    auto& playerCollisionComponent = m_registry.emplace<CollisionComponent>(m_playerEntity);
    auto& playerSpriteComponent = m_registry.emplace<SpriteComponent>(m_playerEntity);
    auto& playerWeaponComponent = m_registry.emplace<WeaponComponent>(m_playerEntity);

    playerSpriteComponent.texture = CreateTextureRef(&m_renderer, m_worldParameters.m_playerSpritePath.c_str());
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
        SpawnExplosion(static_cast<int>(positionComponent.x), static_cast<int>(positionComponent.y), false);
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
