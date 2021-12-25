#include "game_layer.h"

#include <SDL_image.h>

#include "components.h"
#include "layer_stack.h"

#include "input_system.h"
#include "enemy_system.h"
#include "weapon_system.h"
#include "velocity_system.h"
#include "lifetime_system.h"

GameLayer::GameLayer(SDL_Renderer* renderer)
:m_renderer(renderer) {
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
    if (InputSystem::OnEvent(event)) {
        return true;
    }
    return false;
}

void GameLayer::Update(uint32_t ticks) {
    InputSystem::Update(ticks, m_registry);
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
}

void GameLayer::OnAddedToStack(LayerStack* layerStack) {
    m_layerStack = layerStack;
    Setup();
}

void GameLayer::OnRemovedFromStack() {
    m_layerStack = nullptr;
}

void GameLayer::Setup() {
    m_registry.on_destroy<SpriteComponent>().connect<&GameLayer::DestroySprite>(this);

    m_tileMap = std::make_unique<TileMap>(m_renderer, 8, 8);

    // camera
    auto const cameraEntity = m_registry.create();
    auto& cameraPosition = m_registry.emplace<PositionComponent>(cameraEntity);
    cameraPosition.x = 0;
    cameraPosition.y = 0;
    auto& cameraDetails = m_registry.emplace<CameraComponent>(cameraEntity);
    cameraDetails.width = GetLayerWidth();
    cameraDetails.height = GetLayerHeight();
    cameraDetails.active = true;
    auto& cameraVelocity = m_registry.emplace<VelocityComponent>(cameraEntity);
    cameraVelocity.x = 100.0f;

    // player
    auto playerEntity = m_registry.create();
    auto& playerPosition = m_registry.emplace<PositionComponent>(playerEntity);
    playerPosition.x = 0;
    playerPosition.y = 0;
    auto& playerVelocity = m_registry.emplace<VelocityComponent>(playerEntity);
    playerVelocity.x = 100.0f;

    m_registry.emplace<InputComponent>(playerEntity);

    auto& collisionComponent = m_registry.emplace<CollisionComponent>(playerEntity);
    collisionComponent.width = 50;
    collisionComponent.height = 19;
    collisionComponent.flags = COLLISION_FLAG_PLAYER;
    collisionComponent.flag_mask = COLLISION_FLAG_MAP | COLLISION_FLAG_ENEMY;
    collisionComponent.on_collision = [this, cameraEntity](entt::entity otherEntity) {
        // game over
        auto& cameraVelocityComponent = m_registry.get<VelocityComponent>(cameraEntity);
        cameraVelocityComponent.x = 0;
        cameraVelocityComponent.y = 0;
    };

    auto& playerSprite = m_registry.emplace<SpriteComponent>(playerEntity);
    SDL_Surface* image = IMG_Load("ship003.png");
    playerSprite.texture = SDL_CreateTextureFromSurface(m_renderer, image);
    SDL_FreeSurface(image);
    playerSprite.width = 62;
    playerSprite.height = 32;

    auto& playerWeapon = m_registry.emplace<WeaponComponent>(playerEntity);
    playerWeapon.firing = false;
    playerWeapon.fire_delay = 300;
    playerWeapon.facing_left = false;
    playerWeapon.velocity = 600.0f;

    auto enemyBrain = m_registry.create();
    auto& enemySpawnerComponent = m_registry.emplace<EnemySpawnerComponent>(enemyBrain);
    enemySpawnerComponent.spawn_delay = 6000;
    enemySpawnerComponent.speed = 100.0f;
    enemySpawnerComponent.speed_increase = 5.0f;

    SDL_Surface* bg = IMG_Load("background.jpg");
    m_backgroundTexture = SDL_CreateTextureFromSurface(m_renderer, bg);
    SDL_FreeSurface(bg);
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