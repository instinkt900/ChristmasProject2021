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
    SDL_Rect* source_rect = nullptr;
    bool managed_texture = false;
    SDL_RendererFlip flip = SDL_FLIP_NONE;
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

uint32_t constexpr COLLISION_FLAG_MAP = 1 << 0;
uint32_t constexpr COLLISION_FLAG_PLAYER = 1 << 1;
uint32_t constexpr COLLISION_FLAG_ENEMY = 1 << 2;
uint32_t constexpr COLLISION_FLAG_BULLET = 1 << 3;

struct CollisionComponent {
    float width = 0;
    float height = 0;
    uint32_t flags = 0;
    uint32_t flag_mask = 0;
    uint32_t collision_bits = 0;
    bool tilemap_collision = false;
    std::function<void(entt::entity)> on_collision;
};

struct EnemySpawnerComponent {
    uint32_t spawn_delay;
    float speed = 0;
    float speed_increase = 0;
};

struct EnemyComponent {
    bool dead = false;
};