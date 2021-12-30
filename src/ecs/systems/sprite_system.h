#pragma once

class GameLayer;
struct ViewParameters;

namespace SpriteSystem {
    void Draw(SDL_Renderer* renderer, entt::registry& registry, ViewParameters const& view);
}
