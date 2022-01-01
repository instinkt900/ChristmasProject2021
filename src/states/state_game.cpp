#include "game_pch.h"
#include "state_game.h"
#include "state_post_game.h"
#include "layers/game_layer.h"
#include "ecs/systems/enemy_system.h"
#include "ecs/systems/weapon_system.h"
#include "ecs/systems/velocity_system.h"
#include "ecs/systems/lifetime_system.h"
#include "ecs/systems/animation_system.h"
#include "ecs/systems/cleanup_system.h"
#include "ecs/components/components.h"

StateGame::StateGame(StateMachine* stateMachine, GameLayer& gameLayer)
    : State(stateMachine)
    , m_gameLayer(gameLayer) {
}

StateGame::~StateGame() {
}

void StateGame::OnEnter() {
    m_controlState.clear();
    Mix_PlayChannel(-1, m_gameLayer.GetStartSFX().get(), 0);
}

bool StateGame::OnEvent(SDL_Event const& event) {
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
        case SDLK_w:
            m_controlState[ControlKey::Up] = true;
            return true;
        case SDLK_s:
            m_controlState[ControlKey::Down] = true;
            return true;
        case SDLK_a:
            m_controlState[ControlKey::Left] = true;
            return true;
        case SDLK_d:
            m_controlState[ControlKey::Right] = true;
            return true;
        case SDLK_SPACE:
            m_controlState[ControlKey::Fire] = true;
            return true;
        }
    } else if (event.type == SDL_KEYUP) {
        switch (event.key.keysym.sym) {
        case SDLK_w:
            m_controlState[ControlKey::Up] = false;
            return true;
        case SDLK_s:
            m_controlState[ControlKey::Down] = false;
            return true;
        case SDLK_a:
            m_controlState[ControlKey::Left] = false;
            return true;
        case SDLK_d:
            m_controlState[ControlKey::Right] = false;
            return true;
        case SDLK_SPACE:
            m_controlState[ControlKey::Fire] = false;
            return true;
        }
    }
    return false;
}

void StateGame::Update(uint32_t ticks, entt::registry& registry) {
    float const seconds = ticks / 1000.0f;
    auto const& worldParameters = m_gameLayer.GetWorldParameters();
    auto& worldState = m_gameLayer.GetWorldState();

    // update score
    worldState.m_score += static_cast<int>(worldParameters.m_scorePerSecond * seconds);

    // update move speed
    worldState.m_levelSpeed += worldParameters.m_levelSpeedIncrease * seconds;
    auto& playerVelocityComponent = registry.get<VelocityComponent>(m_gameLayer.GetPlayerEntity());
    auto& cameraVelocityComponent = registry.get<VelocityComponent>(m_gameLayer.GetCameraEntity());
    playerVelocityComponent.x = cameraVelocityComponent.x = worldState.m_levelSpeed;

    // input handling
    auto playerEntity = m_gameLayer.GetPlayerEntity();
    auto& playerPositionComponent = registry.get<PositionComponent>(playerEntity);
    if (m_controlState[ControlKey::Up])
        playerPositionComponent.y -= worldParameters.m_playerMoveSpeed * seconds;
    if (m_controlState[ControlKey::Down])
        playerPositionComponent.y += worldParameters.m_playerMoveSpeed * seconds;
    if (m_controlState[ControlKey::Left])
        playerPositionComponent.x -= worldParameters.m_playerMoveSpeed * seconds;
    if (m_controlState[ControlKey::Right])
        playerPositionComponent.x += worldParameters.m_playerMoveSpeed * seconds;
    if (auto weaponComponent = registry.try_get<WeaponComponent>(playerEntity)) {
        weaponComponent->firing = m_controlState[ControlKey::Fire];
    }

    // game systems
    EnemySystem::Update(ticks, m_gameLayer);
    WeaponSystem::Update(ticks, m_gameLayer);
    VelocitySystem::Update(ticks, m_gameLayer);
    LifetimeSystem::Update(ticks, m_gameLayer);
    AnimationSystem::Update(ticks, m_gameLayer);
    CleanupSystem::Update(ticks, m_gameLayer);

    if (!registry.valid(playerEntity)) {
        // game over
        Mix_PlayChannel(-1, m_gameLayer.GetPlayerDiedSFX().get(), 0);
        m_stateMachine->StateTransition<StatePostGame>();
    }
}
