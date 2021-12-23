#pragma once

#include <map>
#include <entt/entt.hpp>
#include <SDL.h>

class InputSystem {
public:
    InputSystem();
    ~InputSystem();

    bool OnEvent(SDL_Event& event);
    void Update(uint32_t ticks, entt::registry& registry);

private:
    enum class ControlKey {
        Up,
        Down,
        Left,
        Right
    };

    std::map<ControlKey, bool> m_controlState;
};