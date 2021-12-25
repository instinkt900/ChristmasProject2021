#pragma once

#include <entt/entt.hpp>
#include <SDL.h>

class TileMap;

namespace EnemySpawnSystem {
    void Update(uint32_t ticks, entt::registry& registry, SDL_Renderer* renderer, TileMap& tileMap);
}
