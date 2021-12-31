#pragma once

#include "simplex_noise.h"

struct ViewParameters;

class TileMap {
public:
    TileMap(SDL_Renderer& renderer, int tileSizeX, int tileSizeY);
    ~TileMap();

    void Draw(ViewParameters const& view) const;
    bool Collides(int x, int y, int width, int height) const;

private:
    SDL_Renderer& m_renderer;
    SimplexNoise m_noise;
    int m_tileSizeX = 0;
    int m_tileSizeY = 0;
    TextureRef m_tileset;

    bool GetTile(int x, int y, SDL_Rect* tilesetRect) const;
};
