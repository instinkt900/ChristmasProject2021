#include "game_layer.h"

#include <SDL_image.h>
#include <SDL_ttf.h>

#include "components.h"
#include "layer_stack.h"

#include "enemy_system.h"
#include "weapon_system.h"
#include "velocity_system.h"
#include "lifetime_system.h"

#include "state_pre_game.h"
#include "state_game.h"
#include "state_post_game.h"

GameLayer::GameLayer(SDL_Renderer* renderer)
:m_renderer(renderer) {
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
            m_registry.view<CameraComponent>().each([&event](auto& cameraComponent) {
                cameraComponent.width = event.window.data1;
                cameraComponent.height = event.window.data2;
                });
            break;
        }
    }

    return m_stateMachine.OnEvent(event);
}

void GameLayer::Update(uint32_t ticks) {
    m_stateMachine.Update(ticks, m_registry);
    EnemySystem::Update(ticks, m_registry, m_renderer, *m_tileMap);
    WeaponSystem::Update(ticks, m_registry, m_renderer);
    VelocitySystem::Update(ticks, m_registry, *m_tileMap);
    LifetimeSystem::Update(ticks, m_registry);
}

void GameLayer::Draw(SDL_Renderer* renderer) {
    SDL_RenderCopy(renderer, m_backgroundTexture, nullptr, nullptr);

    int viewOffsetX = 0;
    int viewOffsetY = 0;
    int viewWidth = 0;
    int viewHeight = 0;
    bool cameraFound = false;
    m_registry.view<PositionComponent, CameraComponent>().each([&](auto const& positionComponent, auto const& cameraComponent) {
        if (!cameraFound && cameraComponent.active) {
            viewWidth = cameraComponent.width;
            viewHeight = cameraComponent.height;
            viewOffsetX = static_cast<int>(positionComponent.x) - viewWidth / 2;
            viewOffsetY = static_cast<int>(positionComponent.y) - viewHeight / 2;
            cameraFound = true;
        }
    });

    m_tileMap->Draw(m_renderer, viewOffsetX, viewOffsetY, GetLayerWidth(), GetLayerHeight());

    m_registry.view<PositionComponent, SpriteComponent>().each([&renderer, viewOffsetX, viewOffsetY](auto entity, auto const& position, auto const& sprite) {
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

    // camera
    m_cameraEntity = m_registry.create();
    auto& cameraPosition = m_registry.emplace<PositionComponent>(m_cameraEntity);
    auto& cameraDetails = m_registry.emplace<CameraComponent>(m_cameraEntity);
    auto& cameraVelocity = m_registry.emplace<VelocityComponent>(m_cameraEntity);

    cameraPosition.x = 0;
    cameraPosition.y = 0;
    
    cameraDetails.width = GetLayerWidth();
    cameraDetails.height = GetLayerHeight();
    cameraDetails.active = true;

    // player
    m_playerEntity = m_registry.create();
    m_registry.emplace<VelocityComponent>(m_playerEntity);
    auto& playerPositionComponent = m_registry.emplace<PositionComponent>(m_playerEntity);
    auto& playerCollisionComponent = m_registry.emplace<CollisionComponent>(m_playerEntity);
    auto& playerSpriteComponent = m_registry.emplace<SpriteComponent>(m_playerEntity);
    auto& playerWeaponComponent = m_registry.emplace<WeaponComponent>(m_playerEntity);

    SDL_Surface* playerImage = IMG_Load("ship003.png");
    playerSpriteComponent.texture = SDL_CreateTextureFromSurface(m_renderer, playerImage);
    SDL_FreeSurface(playerImage);
    playerSpriteComponent.width = 62;
    playerSpriteComponent.height = 32;

    playerPositionComponent.x = 0;
    playerPositionComponent.y = 0;

    playerCollisionComponent.width = 50;
    playerCollisionComponent.height = 19;
    playerCollisionComponent.flags = COLLISION_FLAG_PLAYER;
    playerCollisionComponent.flag_mask = COLLISION_FLAG_MAP | COLLISION_FLAG_ENEMY;
    playerCollisionComponent.on_collision = [this](entt::entity otherEntity) {
        // game over
        m_stateMachine.StateTransition<StatePostGame>();
        //StateTransition(GameState::PostGame);
    };

    playerWeaponComponent.fire_delay = 300;
    playerWeaponComponent.velocity = 600.0f;

    // enemy spawner state (probably move this to a world state object)
    auto enemyBrain = m_registry.create();
    auto& enemySpawnerComponent = m_registry.emplace<EnemySpawnerComponent>(enemyBrain);
    enemySpawnerComponent.spawn_delay = 6000;
    enemySpawnerComponent.speed = 100.0f;
    enemySpawnerComponent.speed_increase = 5.0f;
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