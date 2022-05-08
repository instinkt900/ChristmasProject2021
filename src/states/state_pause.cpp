#include "game_pch.h"
#include "state_pause.h"
#include "layers/game_layer.h"
#include "ecs/systems/animation_system.h"
#include "layers/pause_layer.h"
#include "state_game.h"
#include "moth_ui/group.h"

StatePause::StatePause(StateMachine* stateMachine, GameLayer& gameLayer)
    : State(stateMachine)
    , m_gameLayer(gameLayer) {
    m_layer = std::make_shared<PauseLayer>(m_gameLayer);
}

StatePause::~StatePause() {
}

void StatePause::OnEnter() {
    m_gameLayer.GetLayerStack()->PushLayer(m_layer);
}

void StatePause::OnLeave() {
    m_gameLayer.GetLayerStack()->PopLayer();
}

bool StatePause::OnEvent(moth_ui::Event const& event) {
    moth_ui::EventDispatch dispatch(event);
    dispatch.Dispatch(this, &StatePause::OnKey);
    return dispatch.GetHandled();
}

void StatePause::Update(uint32_t ticks, entt::registry& registry) {
    AnimationSystem::Update(ticks, m_gameLayer);
}

void StatePause::Draw(SDL_Renderer& renderer) {
}

bool StatePause::OnKey(moth_ui::EventKey const& event) {
    if (event.GetAction() == moth_ui::KeyAction::Down) {
        if (event.GetKey() == moth_ui::Key::Escape) {
            m_layer->GetUIRoot()->SetAnimation("out");
            return true;
        }
    }
    return false;
}
