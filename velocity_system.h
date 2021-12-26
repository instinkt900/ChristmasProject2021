#pragma once

#include <entt/entt.hpp>

class GameLayer;

namespace VelocitySystem {
    void Update(uint32_t ticks, GameLayer& gameLayer);
}
