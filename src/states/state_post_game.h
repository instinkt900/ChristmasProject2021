#pragma once

#include "state.h"

class GameLayer;

class StatePostGame : public State {
public:
    StatePostGame(StateMachine* stateMachine, GameLayer& gameLayer);
    virtual ~StatePostGame();

    void OnEnter() override;
    void OnLeave() override;
    bool OnEvent(moth_ui::Event const& event) override;
    void Update(uint32_t ticks, entt::registry& registry) override;
    void Draw(SDL_Renderer& renderer) override;

private:
    GameLayer& m_gameLayer;
};