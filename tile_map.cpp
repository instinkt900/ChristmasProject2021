#include "tile_map.h"

#include <SDL_image.h>
#include <cmath>
#include "components.h"

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
            SDL_Rect sourceRect;
            if (GetTile(tileX, tileY, &sourceRect)) {
                SDL_Rect destRect;
                destRect.x = startPosX + c * m_tileSizeX;
                destRect.y = startPosY + r * m_tileSizeY;
                destRect.w = m_tileSizeX;
                destRect.h = m_tileSizeY;
                SDL_RenderCopy(renderer, m_tileset, &sourceRect, &destRect);
            }
        }
    }
}

void TileMap::UpdateCollisions(entt::registry& registry) {
    registry.view<PositionComponent, CollisionComponent>().each([this](auto const& positionComponent, auto& collisionComponent) {
        int const collisionWidth = static_cast<int>(collisionComponent.width);
        int const collisionHeight = static_cast<int>(collisionComponent.height);
        int const collisionX = static_cast<int>(positionComponent.x) - collisionWidth / 2;
        int const collisionY = static_cast<int>(positionComponent.y) - collisionHeight / 2;
        int const cols = collisionWidth / m_tileSizeX;
        int const rows = collisionHeight / m_tileSizeY;
        int const startTileX = collisionX / m_tileSizeX;
        int const startTileY = collisionY / m_tileSizeY;
        int const startPosX = -collisionX % m_tileSizeX;
        int const startPosY = -collisionY % m_tileSizeY;
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                int const tileX = startTileX + c;
                int const tileY = startTileY + r;
                if (GetTile(tileX, tileY, nullptr)) {
                    collisionComponent.tilemap_collision = true;
                }
            }
        }
    });
}

bool TileMap::GetTile(int x, int y, SDL_Rect* tilesetRect) {
    float const mapHeight = 480.0f / m_tileSizeY;

    float const layer1Scale = 50.0f;
    float const layer1Amp = mapHeight / 4.0f;
    float const mid = std::sin(x / layer1Scale) * layer1Amp;

    int const caveStartX = 50;
    float const noiseAmp = 20.0f;
    int const noiseOctaves = 3;
    float const topOffset = -20.0f;
    float const bottomOffset = 20.0f;

    if (x > caveStartX) {
        if (y < mid) {
            float const extra = (mid + topOffset) + m_noise.fractal(noiseOctaves, static_cast<float>(x)) * noiseAmp;
            if (y < extra) {
                if (nullptr != tilesetRect) {
                    *tilesetRect = { 0, 0, 64, 64 };
                }
                return true;
            }
        }
        else if (y > mid) {
            float const extra = (mid + bottomOffset) + m_noise.fractal(noiseOctaves, static_cast<float>(x) + 62833.0f) * noiseAmp;
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
