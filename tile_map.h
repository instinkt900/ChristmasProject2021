#pragma once

#include "simplex_noise.h"

struct ViewParameters;

class TileMap {
public:
    TileMap(SDL_Renderer* renderer, int tileSizeX, int tileSizeY);
    ~TileMap();

    void Draw(SDL_Renderer* renderer, ViewParameters const& view) const;
    bool Collides(int x, int y, int width, int height) const;

private:
    SimplexNoise m_noise;
    int m_tileSizeX = 0;
    int m_tileSizeY = 0;
    SDL_Texture* m_tileset = nullptr;

    bool GetTile(int x, int y, SDL_Rect* tilesetRect) const;
};
