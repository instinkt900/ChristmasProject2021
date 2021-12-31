#include "game_pch.h"
#include "state_machine.h"
#include "state.h"

StateMachine::StateMachine() {

}

StateMachine::~StateMachine() {

}

bool StateMachine::OnEvent(SDL_Event const& event) {
    if (m_currentState) {
        return m_currentState->OnEvent(event);
    }
    return false;
}

void StateMachine::Update(uint32_t ticks, entt::registry& registry) {
    if (m_currentState) {
        m_currentState->Update(ticks, registry);
    }
}

void StateMachine::Draw(SDL_Renderer& renderer) {
    if (m_currentState) {
        m_currentState->Draw(renderer);
    }
}

void StateMachine::StateTransition(entt::id_type id, State* newState)
{
    if (nullptr != m_currentState) {
        m_currentState->OnLeave();
    }
    m_currentStateId = id;
    m_currentState = newState;
    if (nullptr != m_currentState) {
        m_currentState->OnEnter();
    }
}