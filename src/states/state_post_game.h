#pragma once

#include "state.h"

class GameLayer;

class StatePostGame : public State {
public:
    StatePostGame(StateMachine* stateMachine, GameLayer& gameLayer);
    virtual ~StatePostGame();

    void OnEnter() override;
    bool OnEvent(SDL_Event const& event) override;
    void Update(uint32_t ticks, entt::registry& registry) override;
    void Draw(SDL_Renderer& renderer) override;

private:
    GameLayer& m_gameLayer;
    TextureRef m_gameOverText;
    TextureRef m_highScoreText;
    IntVec2 m_gameOverTextDim;
    IntVec2 m_highScoreTextDim;
    bool m_exitPending = false;
    bool m_newHighScore = false;
};