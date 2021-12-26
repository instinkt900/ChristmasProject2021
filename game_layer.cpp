#include "game_layer.h"

#include <SDL_image.h>
#include <SDL_ttf.h>

#include "components.h"
#include "layer_stack.h"

#include "state_pre_game.h"
#include "state_game.h"
#include "state_post_game.h"

GameLayer::GameLayer(SDL_Renderer* renderer)
:m_renderer(renderer)
,m_random(0) {
    m_registry.on_destroy<SpriteComponent>().connect<&GameLayer::DestroySprite>(this);

    SDL_Surface* bg = IMG_Load("background.jpg");
    m_backgroundTexture = SDL_CreateTextureFromSurface(m_renderer, bg);
    SDL_FreeSurface(bg);

    m_tileMap = std::make_unique<TileMap>(m_renderer, 8, 8);

    m_stateMachine.AddState<StatePreGame>(*this);
    m_stateMachine.AddState<StateGame>(*this);
    m_stateMachine.AddState<StatePostGame>(*this);
}

GameLayer::~GameLayer() {
    SDL_DestroyTexture(m_backgroundTexture);
}

bool GameLayer::OnEvent(SDL_Event& event) {
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

    // update move speed
    float const seconds = ticks / 1000.0f;
    m_worldState.m_levelSpeed += m_worldParameters.m_levelSpeedIncrease * seconds;
    auto& playerVelocityComponent = m_registry.get<VelocityComponent>(m_playerEntity);
    auto& cameraVelocityComponent = m_registry.get<VelocityComponent>(m_cameraEntity);
    playerVelocityComponent.x = cameraVelocityComponent.x = m_worldState.m_levelSpeed;

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
}

void GameLayer::Draw(SDL_Renderer* renderer) {
    SDL_RenderCopy(renderer, m_backgroundTexture, nullptr, nullptr);

    auto const& cameraPositionComponent = m_registry.get<PositionComponent>(m_cameraEntity);
    auto const& cameraCameraComponent = m_registry.get<CameraComponent>(m_cameraEntity);
    int const viewWidth = cameraCameraComponent.width;
    int const viewHeight = cameraCameraComponent.height;
    int const viewOffsetX = static_cast<int>(cameraPositionComponent.x) - viewWidth / 2;
    int const viewOffsetY = static_cast<int>(cameraPositionComponent.y) - viewHeight / 2;

    m_tileMap->Draw(m_renderer, viewOffsetX, viewOffsetY, viewWidth, viewHeight);

    m_registry.view<PositionComponent, SpriteComponent>().each([this, &renderer, viewOffsetX, viewOffsetY](auto entity, auto const& position, auto const& sprite) {
        if (auto healthComponent = m_registry.try_get<HealthComponent>(entity)) {
            if (!healthComponent->alive) {
                return;
            }
        }

        SDL_Rect destRect {
            static_cast<int>(position.x) - viewOffsetX - static_cast<int>(sprite.width / 2),
            static_cast<int>(position.y) - viewOffsetY - static_cast<int>(sprite.height / 2),
            static_cast<int>(sprite.width),
            static_cast<int>(sprite.height)
        };
        SDL_RenderCopyEx(renderer, sprite.texture, sprite.source_rect, &destRect, 0.0f, nullptr, sprite.flip);
    });

    m_stateMachine.Draw(renderer);
}

void GameLayer::OnAddedToStack(LayerStack* layerStack) {
    m_layerStack = layerStack;
    m_stateMachine.StateTransition<StatePreGame>();
}

void GameLayer::OnRemovedFromStack() {
    m_layerStack = nullptr;
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
    
    cameraDetails.width = GetLayerWidth();
    cameraDetails.height = GetLayerHeight();
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
        // game over
        m_stateMachine.StateTransition<StatePostGame>();
    };

    playerWeaponComponent.fire_delay = m_worldParameters.m_playerFireDelay;
    playerWeaponComponent.velocity = m_worldParameters.m_playerBulletSpeed;

    m_worldState.m_levelSpeed = m_worldParameters.m_levelSpeedInit;
    m_worldState.m_enemySpawnDelayMin = static_cast<float>(m_worldParameters.m_enemySpawnDelayMinInit);
    m_worldState.m_enemySpawnDelayMax = static_cast<float>(m_worldParameters.m_enemySpawnDelayMaxInit);
    m_worldState.m_enemySpawnTimer = static_cast<uint32_t>(m_random.Range(m_worldState.m_enemySpawnDelayMin, m_worldState.m_enemySpawnDelayMax));
    m_worldState.m_enemyCurrentSpeed = m_worldParameters.m_enemySpeedInit;
}

int GameLayer::GetLayerWidth() const {
    if (nullptr == m_layerStack) {
        return 0;
    }
    return m_layerStack->GetWidth();
}

int GameLayer::GetLayerHeight() const {
    if (nullptr == m_layerStack) {
        return 0;
    }
    return m_layerStack->GetHeight();
}

void GameLayer::DestroySprite(entt::registry& registry, entt::entity entity) {
    auto& spriteComponent = m_registry.get<SpriteComponent>(entity);
    if (!spriteComponent.managed_texture) {
        SDL_DestroyTexture(spriteComponent.texture);
    }
}