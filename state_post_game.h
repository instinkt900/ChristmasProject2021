#pragma once

#include "state.h"
#include <tuple>

class GameLayer;

class StatePostGame : public State {
public:
    StatePostGame(StateMachine* stateMachine, GameLayer& gameLayer);
    virtual ~StatePostGame();

    void OnEnter() override;
    bool OnEvent(SDL_Event const& event) override;
    void Draw(SDL_Renderer* renderer) override;

private:
    GameLayer& m_gameLayer;
    SDL_Texture* m_gameOverText = nullptr;
    std::tuple<int, int> m_gameOverTextDim;
    bool m_exitPending = false;
};