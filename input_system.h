#pragma once

#include <entt/entt.hpp>
#include <SDL.h>

namespace InputSystem {
    bool OnEvent(SDL_Event& event);
    void Update(uint32_t ticks, entt::registry& registry);
}
