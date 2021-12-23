#pragma once

#include <entt/entt.hpp>
#include <SDL.h>

namespace EnemySpawnSystem {
    void Update(uint32_t ticks, entt::registry& registry, SDL_Renderer* renderer);
}
