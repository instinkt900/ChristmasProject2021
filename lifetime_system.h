#pragma once

#include <entt/entt.hpp>

class GameLayer;

namespace LifetimeSystem {
    void Update(uint32_t ticks, GameLayer& gameLayer);
}
