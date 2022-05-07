#pragma once

#include "state.h"

class GameLayer;

class StateGame : public State {
public:
    StateGame(StateMachine* stateMachine, GameLayer& gameLayer);
    virtual ~StateGame();

    void OnEnter() override;
    bool OnEvent(moth_ui::Event const& event) override;
    void Update(uint32_t ticks, entt::registry& registry) override;

private:
    GameLayer& m_gameLayer;

    enum class ControlKey {
        Up,
        Down,
        Left,
        Right,
        Fire
    };

    std::map<ControlKey, bool> m_controlState;
};