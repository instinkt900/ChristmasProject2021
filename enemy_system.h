#pragma once

#include <entt/entt.hpp>
#include <SDL.h>

class TileMap;

namespace EnemySystem {
    void Update(uint32_t ticks, entt::registry& registry, SDL_Renderer* renderer, TileMap& tileMap);
}
