#pragma once

#include <entt/entt.hpp>
#include <SDL.h>

class GameLayer;

namespace WeaponSystem {
    void Update(uint32_t ticks, GameLayer& gameLayer);
}
