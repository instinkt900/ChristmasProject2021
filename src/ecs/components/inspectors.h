#pragma once

void ImGuiInspectMember(char const* name, bool& value) {
    ImGui::Checkbox(name, &value);
}

void ImGuiInspectMember(char const* name, int& value) {
    ImGui::InputInt(name, &value);
}

void ImGuiInspectMember(char const* name, int const& value) {
    ImGui::LabelText(name, "%d", value);
}

void ImGuiInspectMember(char const* name, float& value) {
    ImGui::InputFloat(name, &value);
}

void ImGuiInspectMember(char const* name, char const* value) {
    ImGui::LabelText(name, "%s", value);
}

void ImGuiInspectMember(char const* name, entt::entity value) {
    ImGui::LabelText(name, "%d", static_cast<uint32_t>(value));
}

void ImGuiInspectMember(char const* name, SDL_Rect& value) {
    if (ImGui::CollapsingHeader(name)) {
        ImGuiInspectMember("x", value.x);
        ImGuiInspectMember("y", value.y);
        ImGuiInspectMember("w", value.w);
        ImGuiInspectMember("h", value.h);
    }
}

void ImGuiInspectMember(char const* name, TextureRef& value) {
    ImGui::Image(value.get(), ImVec2(100, 100));
}

template <typename T>
void ImGuiInspectMember(char const* name, std::vector<T>& value) {
    if (ImGui::CollapsingHeader(name)) {
        for (int i = 0; i < value.size(); ++i) {
            auto const label = fmt::format("{}", i);
            ImGuiInspectMember(label.c_str(), value[i]);
        }
    }
}

template <typename T>
void ImGuiInspectMember(char const* name, std::function<T> const& value) {
    ImGui::LabelText(name, "<function>");
}

template <typename T>
void ImGuiInspectMember(char const* name, T* value) {
    if (nullptr == value) {
        ImGui::LabelText(name, "nullptr");
    } else {
        ImGuiInspectMember(name, *value);
    }
}

void ImGuiInspect(PositionComponent& component) {
    if (ImGui::CollapsingHeader("PositionComponent")) {
        ImGuiInspectMember("x", component.x);
        ImGuiInspectMember("y", component.y);
        ImGuiInspectMember("parent", component.parent);
    }
}

void ImGuiInspect(VelocityComponent& component) {
    if (ImGui::CollapsingHeader("VelocityComponent")) {
        ImGuiInspectMember("x", component.x);
        ImGuiInspectMember("y", component.y);
    }
}

void ImGuiInspect(AnimationComponent& component) {
    if (ImGui::CollapsingHeader("AnimationComponent")) {
        ImGuiInspectMember("frames", component.frames);
        ImGuiInspectMember("ticks_per_frame", component.ticks_per_frame);
        ImGuiInspectMember("anim_type", static_cast<int>(component.anim_type));
        ImGuiInspectMember("current_time", component.current_time);
        ImGuiInspectMember("playing", component.playing);
    }
}

void ImGuiInspect(SpriteComponent& component) {
    if (ImGui::CollapsingHeader("SpriteComponent")) {
        ImGuiInspectMember("texture", component.texture);
        ImGuiInspectMember("source_rect", component.source_rect);
        ImGuiInspectMember("width", component.width);
        ImGuiInspectMember("height", component.height);
        ImGuiInspectMember("visible", component.visible);
        ImGuiInspectMember("flip", static_cast<int>(component.flip));
    }
}

void ImGuiInspect(CameraComponent& component) {
    if (ImGui::CollapsingHeader("CameraComponent")) {
        ImGuiInspectMember("active", component.active);
    }
}

void ImGuiInspect(WeaponComponent& component) {
    if (ImGui::CollapsingHeader("WeaponComponent")) {
        ImGuiInspectMember("firing", component.firing);
        ImGuiInspectMember("fire_delay", component.fire_delay);
        ImGuiInspectMember("timer", component.timer);
        ImGuiInspectMember("facing_left", component.facing_left);
        ImGuiInspectMember("velocity", component.velocity);
    }
}

void ImGuiInspect(LifetimeComponent& component) {
    if (ImGui::CollapsingHeader("LifetimeComponent")) {
        ImGuiInspectMember("lifetime", component.lifetime);
    }
}

void ImGuiInspect(CollisionComponent& component) {
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

void ImGuiInspect(HealthComponent& component) {
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

void ImGuiInspectAll(entt::registry& registry, entt::entity entity) {
    ImGuiInspectMultiple<PositionComponent,
                         VelocityComponent,
                         AnimationComponent,
                         SpriteComponent,
                         CameraComponent,
                         WeaponComponent,
                         CollisionComponent,
                         HealthComponent>(registry, entity);
}
