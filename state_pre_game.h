#pragma once

#include "state.h"
#include <tuple>

class GameLayer;

class StatePreGame : public State {
public:
    StatePreGame(StateMachine* stateMachine, GameLayer& gameLayer);
    virtual ~StatePreGame();

    void OnEnter() override;
    void Update(uint32_t ticks, entt::registry& registry) override;
    void Draw(SDL_Renderer* renderer) override;

private:
    GameLayer& m_gameLayer;
    uint32_t m_timer = 0;
    uint32_t m_sfxTimer = 0;
    SDL_Texture* m_countDownText[3] = { nullptr, nullptr, nullptr };
    std::tuple<int, int> m_countDownTextDim[3];
};