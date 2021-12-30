#pragma once

class GameLayer;
struct ViewParameters;

namespace SpriteSystem {
    void Update(uint32_t ticks, GameLayer& gameLayer);
    void Draw(SDL_Renderer* renderer, entt::registry& registry, ViewParameters const& view);
}
