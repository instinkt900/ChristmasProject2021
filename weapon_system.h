#pragma once

#include <entt/entt.hpp>
#include <SDL.h>

namespace WeaponSystem {
    void Update(uint32_t ticks, entt::registry& registry, SDL_Renderer* renderer);
}
