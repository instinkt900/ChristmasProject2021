#include "game_pch.h"
#include "state_pre_game.h"
#include "state_game.h"
#include "layers/game_layer.h"
#include "ecs/systems/animation_system.h"
#include "moth_ui/node_text.h"

StatePreGame::StatePreGame(StateMachine* stateMachine, GameLayer& gameLayer)
    : State(stateMachine)
    , m_gameLayer(gameLayer) {
}

void StatePreGame::OnEnter() {
    m_timer = 3000;
    m_sfxTimer = 0;
    m_gameLayer.SetupLevel();
    m_gameLayer.GetCountText()->SetVisible(true);
}

void StatePreGame::OnLeave() {
    m_gameLayer.GetCountText()->SetVisible(false);
}

void StatePreGame::Update(uint32_t ticks, entt::registry& registry) {
    if (ticks > m_timer) {
        m_stateMachine->StateTransition<StateGame>();
        return;
    }
    m_timer -= ticks;

    if (ticks > m_sfxTimer) {
        Mix_PlayChannel(-1, m_gameLayer.GetAudioFactory().GetCountSFX().get(), 0);
        m_sfxTimer += 1000;
    }
    m_sfxTimer -= ticks;

    AnimationSystem::Update(ticks, m_gameLayer);
}

void StatePreGame::Draw(SDL_Renderer& renderer) {
    int const number = 1 + std::min(2, static_cast<int>(std::floor(m_timer / 1000)));
    m_gameLayer.GetCountText()->SetText(fmt::format("{}", number));
}
