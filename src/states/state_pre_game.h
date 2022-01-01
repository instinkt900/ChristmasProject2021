#pragma once

#include "state.h"

class GameLayer;

class StatePreGame : public State {
public:
    StatePreGame(StateMachine* stateMachine, GameLayer& gameLayer);
    virtual ~StatePreGame();

    void OnEnter() override;
    void Update(uint32_t ticks, entt::registry& registry) override;
    void Draw(SDL_Renderer& renderer) override;

private:
    GameLayer& m_gameLayer;
    uint32_t m_timer = 0;
    uint32_t m_sfxTimer = 0;
    TextureRef m_countDownText[3];
    IntVec2 m_countDownTextDim[3];
};