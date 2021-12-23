#include "game_layer.h"

#include <SDL_image.h>

#include "components.h"
#include "layer_stack.h"

#include "input_system.h"
#include "enemy_spawn_system.h"
#include "weapon_system.h"
#include "velocity_system.h"
#include "lifetime_system.h"

GameLayer::GameLayer(SDL_Renderer* renderer)
:m_renderer(renderer) {
}

GameLayer::~GameLayer() {

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
    EnemySpawnSystem::Update(ticks, m_registry, m_renderer);
    WeaponSystem::Update(ticks, m_registry, m_renderer);
    VelocitySystem::Update(ticks, m_registry);
    LifetimeSystem::Update(ticks, m_registry);
}

void GameLayer::Draw(SDL_Renderer* renderer) {
    int viewOffsetX = 0;
    int viewOffsetY = 0;
    bool cameraFound = false;
    m_registry.view<PositionComponent, CameraComponent>().each([&viewOffsetX, &viewOffsetY, &cameraFound](auto const& positionComponent, auto const& cameraComponent) {
        if (!cameraFound && cameraComponent.active) {
            viewOffsetX = static_cast<int>(positionComponent.x) - cameraComponent.width / 2;
            viewOffsetY = static_cast<int>(positionComponent.y) - cameraComponent.height / 2;
            cameraFound = true;
        }
    });

    m_registry.view<PositionComponent, SpriteComponent>().each([&renderer, viewOffsetX, viewOffsetY](auto entity, auto const& position, auto const& sprite) {
        SDL_Rect destRect {
            static_cast<int>(position.x) - viewOffsetX - static_cast<int>(sprite.width / 2),
            static_cast<int>(position.y) - viewOffsetY - static_cast<int>(sprite.height / 2),
            static_cast<int>(sprite.width),
            static_cast<int>(sprite.height)
        };
        SDL_RenderCopy(renderer, sprite.texture, nullptr, &destRect);
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

    // camera
    auto const cameraEntity = m_registry.create();
    auto& cameraPosition = m_registry.emplace<PositionComponent>(cameraEntity);
    cameraPosition.x = 0;
    cameraPosition.y = 0;
    auto& cameraDetails = m_registry.emplace<CameraComponent>(cameraEntity);
    cameraDetails.width = GetLayerWidth();
    cameraDetails.height = GetLayerHeight();
    cameraDetails.active = true;

    // player
    auto playerEntity = m_registry.create();
    auto& playerPosition = m_registry.emplace<PositionComponent>(playerEntity);
    playerPosition.x = 0;
    playerPosition.y = 0;

    m_registry.emplace<InputComponent>(playerEntity);

    auto& playerSprite = m_registry.emplace<SpriteComponent>(playerEntity);
    SDL_Surface* image = IMG_Load("playership.png");
    playerSprite.texture = SDL_CreateTextureFromSurface(m_renderer, image);
    playerSprite.width = 100;
    playerSprite.height = 56;

    auto& playerWeapon = m_registry.emplace<WeaponComponent>(playerEntity);
    playerWeapon.firing = false;
    playerWeapon.fire_delay = 1000;
    playerWeapon.facing_left = false;
    playerWeapon.velocity = 300.0f;
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
    SDL_DestroyTexture(spriteComponent.texture);
}