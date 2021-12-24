#include "tile_map.h"

#include <SDL_image.h>
#include <cmath>

TileMap::TileMap(SDL_Renderer* renderer, int tileSizeX, int tileSizeY)
    : m_noise(1/16.0f, 10000.0f)
    , m_tileSizeX(tileSizeX)
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
    float const mapHeight = 480.0f / m_tileSizeY;
    float const midPoint = mapHeight / 2.0f;

    float const layer1Scale = 50.0f;
    float const layer1Amp = midPoint / 4.0f;
    float const mid = std::sin(x / layer1Scale) * layer1Amp + midPoint;

    if (x > 100) {
        if (y < mid) {
            float const extra = (mid - 20.0f) + m_noise.fractal(3, x) * 20.0f;
            if (y < extra) {
                return { 0,0,64,64 };
            }
        }
        else if (y > mid) {
            float const extra = (mid + 20.0f) + m_noise.fractal(3, x + 62833.0f) * 20.0f;
            if (y > extra) {
                return { 0,0,64,64 };
            }
        }
    }
    return { 0, 64, 64, 64 };
}
