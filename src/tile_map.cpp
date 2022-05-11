#include "game_pch.h"
#include "tile_map.h"
#include "ecs/components/components.h"
#include "utils.h"

TileMap::TileMap(SDL_Renderer& renderer, int tileSizeX, int tileSizeY)
    : m_renderer(renderer)
    , m_noise(1 / 16.0f, 10000.0f)
    , m_tileSizeX(tileSizeX)
    , m_tileSizeY(tileSizeY) {
    m_tileset = CreateTextureRef(&m_renderer, "resources/images/tileset.png");
}

TileMap::~TileMap() {
}

void TileMap::Draw(ViewParameters const& view) const {
    int const rows = static_cast<int>(std::ceil(view.m_height / static_cast<float>(m_tileSizeX)));
    int const cols = static_cast<int>(std::ceil(view.m_width / static_cast<float>(m_tileSizeY))) + 1;
    int const startTileX = view.m_offsetX / m_tileSizeX;
    int const startTileY = view.m_offsetY / m_tileSizeY;
    int const startPosX = -view.m_offsetX % m_tileSizeX;
    int const startPosY = -view.m_offsetY % m_tileSizeY;
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            int const tileX = startTileX + c;
            int const tileY = startTileY + r;
            SDL_Rect sourceRect;
            if (GetTile(tileX, tileY, &sourceRect)) {
                SDL_Rect destRect;
                destRect.x = startPosX + c * m_tileSizeX;
                destRect.y = startPosY + r * m_tileSizeY;
                destRect.w = m_tileSizeX;
                destRect.h = m_tileSizeY;
                SDL_RenderCopy(&m_renderer, m_tileset.get(), &sourceRect, &destRect);
            }
        }
    }
}

bool TileMap::Collides(int x, int y, int width, int height) const {
    int const cols = width / m_tileSizeX;
    int const rows = height / m_tileSizeY;
    int const left = x - width / 2;
    int const top = y - height / 2;
    int const startTileX = left / m_tileSizeX;
    int const startTileY = top / m_tileSizeY;
    int const startPosX = -left % m_tileSizeX;
    int const startPosY = -top % m_tileSizeY;
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            int const tileX = startTileX + c;
            int const tileY = startTileY + r;
            if (GetTile(tileX, tileY, nullptr)) {
                return true;
            }
        }
    }
    return false;
}

// TODO generate chunks instead of on demand per tile
// then we can do clever things like sampling around each
// tile and building a better looking map using wang tiling or something
bool TileMap::GetTile(int x, int y, SDL_Rect* tilesetRect) const {
    float const mapHeight = 480.0f / m_tileSizeY;

    float const layer1Scale = 50.0f;
    float const layer1Amp = mapHeight / 6.0f;
    float const mid = std::sin((x + 100) / layer1Scale) * layer1Amp;

    int const caveStartX = 50;
    float const noiseAmp = 20.0f;
    int const noiseOctaves = 3;
    float const topOffset = -20.0f;
    float const bottomOffset = 20.0f;

    float const topNoiseSeed = 411250.0f;
    float const bottomNoiseSeed = 918321.0f;

    if (x > caveStartX) {
        if (y < mid) {
            float const extra = (mid + topOffset) + m_noise.fractal(noiseOctaves, static_cast<float>(x) + topNoiseSeed) * noiseAmp;
            if (y < extra) {
                if (nullptr != tilesetRect) {
                    *tilesetRect = { 0, 0, 64, 64 };
                }
                return true;
            }
        } else if (y > mid) {
            float const extra = (mid + bottomOffset) + m_noise.fractal(noiseOctaves, static_cast<float>(x) + bottomNoiseSeed) * noiseAmp;
            if (y > extra) {
                if (nullptr != tilesetRect) {
                    *tilesetRect = { 0, 0, 64, 64 };
                }
                return true;
            }
        }
    }
    return false;
}
