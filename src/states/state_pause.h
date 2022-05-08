#pragma once

#include "state.h"

class GameLayer;
class PauseLayer;

class StatePause : public State {
public:
    StatePause(StateMachine* stateMachine, GameLayer& gameLayer);
    virtual ~StatePause();

    void OnEnter() override;
    void OnLeave() override;
    bool OnEvent(moth_ui::Event const& event) override;
    void Update(uint32_t ticks, entt::registry& registry) override;
    void Draw(SDL_Renderer& renderer) override;

private:
    GameLayer& m_gameLayer;
    std::shared_ptr<PauseLayer> m_layer;

    bool OnKey(moth_ui::EventKey const& event);
};