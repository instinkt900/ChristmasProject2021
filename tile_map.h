#pragma once

#include <SDL.h>
#include "simplex_noise.h"
#include <entt/entt.hpp>

class TileMap {
public:
    TileMap(SDL_Renderer* renderer, int tileSizeX, int tileSizeY);
    ~TileMap();

    void Draw(SDL_Renderer* renderer, int viewOffsetX, int viewOffsetY, int viewWidth, int viewHeight) const;
    void UpdateCollisions(entt::registry& registry) const;
    bool Collides(int x, int y, int width, int height) const;

private:
    SimplexNoise m_noise;
    int m_tileSizeX = 0;
    int m_tileSizeY = 0;
    SDL_Texture* m_tileset = nullptr;

    bool GetTile(int x, int y, SDL_Rect* tilesetRect) const;
};
