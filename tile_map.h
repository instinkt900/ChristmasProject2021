#pragma once

#include <SDL.h>

class TileMap {
public:
    TileMap(SDL_Renderer* renderer, int tileSizeX, int tileSizeY);
    ~TileMap();

    void Draw(SDL_Renderer* renderer, int viewOffsetX, int viewOffsetY, int viewWidth, int viewHeight);

private:
    int m_tileSizeX = 0;
    int m_tileSizeY = 0;
    SDL_Texture* m_tileset = nullptr;

    SDL_Rect GetTile(int x, int y);
};
