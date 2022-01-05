#pragma once

#include "debug/inspectors.h"

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

inline void ImGuiInspect(PositionComponent& component) {
    if (ImGui::CollapsingHeader("PositionComponent")) {
        ImGuiInspectMember("x", component.x);
        ImGuiInspectMember("y", component.y);
        ImGuiInspectMember("parent", component.parent);
    }
}

inline void ImGuiInspect(VelocityComponent& component) {
    if (ImGui::CollapsingHeader("VelocityComponent")) {
        ImGuiInspectMember("x", component.x);
        ImGuiInspectMember("y", component.y);
    }
}

inline void ImGuiInspect(AnimationComponent& component) {
    if (ImGui::CollapsingHeader("AnimationComponent")) {
        ImGuiInspectMember("frames", component.frames);
        ImGuiInspectMember("ticks_per_frame", component.ticks_per_frame);
        ImGuiInspectMember("anim_type", static_cast<int>(component.anim_type));
        ImGuiInspectMember("current_time", component.current_time);
        ImGuiInspectMember("playing", component.playing);
    }
}

inline void ImGuiInspect(SpriteComponent& component) {
    if (ImGui::CollapsingHeader("SpriteComponent")) {
        ImGuiInspectMember("texture", component.texture);
        ImGuiInspectMember("source_rect", component.source_rect);
        ImGuiInspectMember("width", component.width);
        ImGuiInspectMember("height", component.height);
        ImGuiInspectMember("visible", component.visible);
        ImGuiInspectMember("flip", static_cast<int>(component.flip));
    }
}

inline void ImGuiInspect(CameraComponent& component) {
    if (ImGui::CollapsingHeader("CameraComponent")) {
        ImGuiInspectMember("active", component.active);
    }
}

inline void ImGuiInspect(WeaponComponent& component) {
    if (ImGui::CollapsingHeader("WeaponComponent")) {
        ImGuiInspectMember("firing", component.firing);
        ImGuiInspectMember("fire_delay", component.fire_delay);
        ImGuiInspectMember("timer", component.timer);
        ImGuiInspectMember("facing_left", component.facing_left);
        ImGuiInspectMember("velocity", component.velocity);
    }
}

inline void ImGuiInspect(LifetimeComponent& component) {
    if (ImGui::CollapsingHeader("LifetimeComponent")) {
        ImGuiInspectMember("lifetime", component.lifetime);
    }
}

inline void ImGuiInspect(CollisionComponent& component) {
    if (ImGui::CollapsingHeader("CollisionComponent")) {
        ImGuiInspectMember("width", component.width);
        ImGuiInspectMember("height", component.height);
        ImGuiInspectMember("flags", component.flags);
        ImGuiInspectMember("flag_mask", component.flag_mask);
        ImGuiInspectMember("collision_bits", component.collision_bits);
        ImGuiInspectMember("tilemap_collision", component.tilemap_collision);
        ImGuiInspectMember("on_collision", component.on_collision);
    }
}

inline void ImGuiInspect(HealthComponent& component) {
    if (ImGui::CollapsingHeader("HealthComponent")) {
        ImGuiInspectMember("alive", component.alive);
    }
}

template <typename T>
void ImGuiInspect(entt::registry& registry, entt::entity entity) {
    if (T* comp = registry.try_get<T>(entity)) {
        ImGuiInspect(*comp);
    }
}

template <typename... Ts>
void ImGuiInspectMultiple(entt::registry& registry, entt::entity entity) {
    (ImGuiInspect<Ts>(registry, entity), ...); // fold expression
}

inline void ImGuiInspectAll(entt::registry& registry, entt::entity entity) {
    ImGuiInspectMultiple<PositionComponent,
                         VelocityComponent,
                         AnimationComponent,
                         SpriteComponent,
                         CameraComponent,
                         WeaponComponent,
                         CollisionComponent,
                         HealthComponent>(registry, entity);
}
