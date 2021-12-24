#include "tile_map.h"

#include <SDL_image.h>
#include <cmath>

TileMap::TileMap(SDL_Renderer* renderer, int tileSizeX, int tileSizeY)
    :m_tileSizeX(tileSizeX)
    , m_tileSizeY(tileSizeY) {
    auto image = IMG_Load("tileset.png");
    m_tileset = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image);
}

TileMap::~TileMap() {
    SDL_DestroyTexture(m_tileset);
}

void TileMap::Draw(SDL_Renderer* renderer, int viewOffsetX, int viewOffsetY, int viewWidth, int viewHeight) {
    int const rows = static_cast<int>(std::ceil(viewHeight / static_cast<float>(m_tileSizeX)));
    int const cols = static_cast<int>(std::ceil(viewWidth / static_cast<float>(m_tileSizeY))) + 1;
    int const startTileX = viewOffsetX / m_tileSizeX;
    int const startTileY = viewOffsetY / m_tileSizeY;
    int const startPosX = -viewOffsetX % m_tileSizeX;
    int const startPosY = -viewOffsetY % m_tileSizeY;
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            int const tileX = startTileX + c;
            int const tileY = startTileY + r;
            SDL_Rect sourceRect = GetTile(tileX, tileY);
            SDL_Rect destRect;
            destRect.x = startPosX + c * m_tileSizeX;
            destRect.y = startPosY + r * m_tileSizeY;
            destRect.w = m_tileSizeX;
            destRect.h = m_tileSizeY;
            SDL_RenderCopy(renderer, m_tileset, &sourceRect, &destRect);
        }
    }
}

SDL_Rect TileMap::GetTile(int x, int y)
{
    if (y > 3 && y < 6) {
        return { 0, 64, 64, 64 };
    }
    return { x % 4 * 64,0,64,64 };
}