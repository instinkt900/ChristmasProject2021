#pragma once

#include <entt/entt.hpp>

class TileMap;

namespace VelocitySystem {
    void Update(uint32_t ticks, entt::registry& registry, TileMap& tilemap);
}
