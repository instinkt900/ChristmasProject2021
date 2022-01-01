#pragma once

struct PositionComponent {
    float x = 0;
    float y = 0;
    entt::entity parent = entt::null;
};

struct VelocityComponent {
    float x = 0;
    float y = 0;
};

enum class AnimType {
    Stop,
    Loop,
    Reset
};

struct AnimationComponent {
    std::vector<SDL_Rect> frames;
    int ticks_per_frame = 0;
    AnimType anim_type = AnimType::Stop;
    int current_time = 0;
    bool playing = false;
};

struct SpriteComponent {
    TextureRef texture;
    SDL_Rect* source_rect = nullptr;
    int width = 0;
    int height = 0;
    bool visible = true;
    SDL_RendererFlip flip = SDL_FLIP_NONE;
};

struct CameraComponent {
    bool active = true;
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
    int width = 0;
    int height = 0;
    uint32_t flags = 0;
    uint32_t flag_mask = 0;
    uint32_t collision_bits = 0;
    bool tilemap_collision = false;
    std::function<void(entt::entity)> on_collision;
};

struct EnemyComponent {
};

struct HealthComponent {
    bool alive = true;
};