#include "state_game.h"
#include "components.h"
#include "game_layer.h"

#include "enemy_system.h"
#include "weapon_system.h"
#include "velocity_system.h"
#include "lifetime_system.h"

StateGame::StateGame(StateMachine* stateMachine, GameLayer& gameLayer)
: State(stateMachine)
, m_gameLayer(gameLayer) {

}

StateGame::~StateGame() {

}

void StateGame::OnEnter() {
    auto& registry = m_gameLayer.GetRegistry();
    auto cameraEntity = m_gameLayer.GetCameraEntity();
    auto playerEntity = m_gameLayer.GetPlayerEntity();
    auto& cameraVelocityComponent = registry.get<VelocityComponent>(cameraEntity);
    auto& playerVelocityComponent = registry.get<VelocityComponent>(playerEntity);
    cameraVelocityComponent.x = playerVelocityComponent.x = 100.0f;

    m_controlState.clear();
}

bool StateGame::OnEvent(SDL_Event const& event) {
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
        case SDLK_w: m_controlState[ControlKey::Up] = true; return true;
        case SDLK_s: m_controlState[ControlKey::Down] = true; return true;
        case SDLK_a: m_controlState[ControlKey::Left] = true; return true;
        case SDLK_d: m_controlState[ControlKey::Right] = true; return true;
        case SDLK_SPACE: m_controlState[ControlKey::Fire] = true; return true;
        }
    }
    else if (event.type == SDL_KEYUP) {
        switch (event.key.keysym.sym) {
        case SDLK_w: m_controlState[ControlKey::Up] = false; return true;
        case SDLK_s: m_controlState[ControlKey::Down] = false; return true;
        case SDLK_a: m_controlState[ControlKey::Left] = false; return true;
        case SDLK_d: m_controlState[ControlKey::Right] = false; return true;
        case SDLK_SPACE: m_controlState[ControlKey::Fire] = false; return true;
        }
    }
    return false;
}

void StateGame::Update(uint32_t ticks, entt::registry& registry) {
    auto const& worldParameters = m_gameLayer.GetWorldParameters();
    auto playerEntity = m_gameLayer.GetPlayerEntity();
    auto& playerPositionComponent = registry.get<PositionComponent>(playerEntity);
    float const seconds = ticks / 1000.0f;
    if (m_controlState[ControlKey::Up]) playerPositionComponent.y -= worldParameters.m_playerMoveSpeed * seconds;
    if (m_controlState[ControlKey::Down]) playerPositionComponent.y += worldParameters.m_playerMoveSpeed * seconds;
    if (m_controlState[ControlKey::Left]) playerPositionComponent.x -= worldParameters.m_playerMoveSpeed * seconds;
    if (m_controlState[ControlKey::Right]) playerPositionComponent.x += worldParameters.m_playerMoveSpeed * seconds;
    if (auto weaponComponent = registry.try_get<WeaponComponent>(playerEntity)) {
        weaponComponent->firing = m_controlState[ControlKey::Fire];
    }

    EnemySystem::Update(ticks, m_gameLayer);
    WeaponSystem::Update(ticks, m_gameLayer);
    VelocitySystem::Update(ticks, m_gameLayer);
    LifetimeSystem::Update(ticks, m_gameLayer);
}
