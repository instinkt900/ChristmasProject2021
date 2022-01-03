#pragma once

class State;

class StateMachine {
public:
    StateMachine();
    ~StateMachine();

    template <class T, class... Args>
    void AddState(Args&&... args) {
        m_states.insert(std::make_pair(entt::type_index<T>::value(), std::make_unique<T>(this, std::forward<Args>(args)...)));
    }

    template <class T>
    void StateTransition() {
        auto const stateId = entt::type_index<T>::value();
        auto const it = m_states.find(stateId);
        if (std::end(m_states) != it) {
            StateTransition(stateId, it->second.get());
        }
    }

    template <typename T>
    bool IsInState() const {
        auto const stateId = entt::type_index<T>::value();
        return m_currentStateId == stateId;
    }

    bool OnEvent(Event const& event);
    void Update(uint32_t ticks, entt::registry& registry);
    void Draw(SDL_Renderer& renderer);

private:
    std::map<entt::id_type, std::unique_ptr<State>> m_states;
    entt::id_type m_currentStateId;
    State* m_currentState = nullptr;

    void StateTransition(entt::id_type id, State* newState);
};