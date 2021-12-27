#pragma once

#include <SDL.h>
#include <entt/entt.hpp>

class GameLayer;

namespace SpriteSystem {
    void Update(uint32_t ticks, GameLayer& gameLayer);
    void Draw(SDL_Renderer* renderer, entt::registry& registry, int viewOffsetX, int viewOffsetY, int viewWidth, int viewHeight);
}
