#include "game_pch.h"
#include "state_post_game.h"
#include "state_pre_game.h"
#include "layers/game_layer.h"
#include "ecs/components/components.h"
#include "ecs/systems/animation_system.h"
#include "moth_ui/events/event_key.h"
#include "../layers/game_over_layer.h"

StatePostGame::StatePostGame(StateMachine* stateMachine, GameLayer& gameLayer)
    : State(stateMachine)
    , m_gameLayer(gameLayer) {
}

StatePostGame::~StatePostGame() {
}

void StatePostGame::OnEnter() {
    auto gameOverLayer = std::make_unique<GameOverLayer>(m_gameLayer.GetGame());

    auto& worldState = m_gameLayer.GetWorldState();
    if (worldState.m_score > worldState.m_highScore) {
        worldState.m_highScore = worldState.m_score;
        gameOverLayer->SetNewHighScore(worldState.m_highScore);
        m_gameLayer.SaveScore();
    }

    m_gameLayer.GetLayerStack()->PushLayer(std::move(gameOverLayer));
}

void StatePostGame::OnLeave() {
    m_gameLayer.GetLayerStack()->PopLayer();
}

bool StatePostGame::OnEvent(moth_ui::Event const& event) {
    return false;
}

void StatePostGame::Update(uint32_t ticks, entt::registry& registry) {
    AnimationSystem::Update(ticks, m_gameLayer);
}

void StatePostGame::Draw(SDL_Renderer& renderer) {
}
