#pragma once

#include <SDL.h>

struct PositionComponent {
    float x;
    float y;
};

struct InputComponent {
};

struct SpriteComponent {
    SDL_Texture* texture;
    float width;
    float height;
};
