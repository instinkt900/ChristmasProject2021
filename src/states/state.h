#pragma once

class StateMachine;

class State {
public:
    State(StateMachine* stateMachine) :m_stateMachine(stateMachine) {}
    virtual ~State() {}
    virtual void OnEnter() {}
    virtual void OnLeave() {}
    virtual bool OnEvent(moth_ui::Event const& event) { return false; }
    virtual void Update(uint32_t ticks, entt::registry& registry) {}
    virtual void Draw(SDL_Renderer& renderer) {}

protected:
    StateMachine* m_stateMachine = nullptr;
};
