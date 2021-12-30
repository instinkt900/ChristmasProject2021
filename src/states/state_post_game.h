#pragma once

#include "state.h"

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
    SDL_Texture* m_highScoreText = nullptr;
    std::tuple<int, int> m_gameOverTextDim;
    std::tuple<int, int> m_highScoreTextDim;
    bool m_exitPending = false;
    bool m_newHighScore = false;
};