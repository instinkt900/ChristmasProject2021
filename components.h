#pragma once

#include <SDL.h>

struct PositionComponent {
    float x = 0;
    float y = 0;
};

struct VelocityComponent {
    float x = 0;
    float y = 0;
};

struct InputComponent {
};

struct SpriteComponent {
    SDL_Texture* texture;
    float width = 0.0f;
    float height = 0.0f;
};

struct CameraComponent {
    bool active = true;
    int width = 0;
    int height = 0;
};

struct WeaponComponent {
    bool firing = false;
    int fire_delay = 0;
    int timer = 0;
    bool facing_left = false;
    float velocity = 0;
};

struct LifetimeComponent {
    uint32_t lifetime = 0;
};
