#pragma once

struct ViewParameters {
    int m_offsetX;
    int m_offsetY;
    int m_width;
    int m_height;
};

template <typename T>
struct Vec2 {
    T x{ 0 };
    T y{ 0 };
};

using IntVec2 = Vec2<int>;
using FloatVec2 = Vec2<float>;

struct PositionComponent;

IntVec2 ResolvePosition(entt::registry& registry, entt::entity entity);
IntVec2 ResolvePosition(entt::registry& registry, PositionComponent const& positionComponent);

inline void to_json(nlohmann::json& j, IntVec2 const& intVec) {
    j = nlohmann::json{ { "x", intVec.x }, { "y", intVec.y } };
}

inline void from_json(nlohmann::json const& j, IntVec2& intVec) {
    j.at("x").get_to(intVec.x);
    j.at("y").get_to(intVec.y);
}

inline void to_json(nlohmann::json& j, FloatVec2 const& floatVec) {
    j = nlohmann::json{ { "x", floatVec.x }, { "y", floatVec.y } };
}

inline void from_json(nlohmann::json const& j, FloatVec2& floatVec) {
    j.at("x").get_to(floatVec.x);
    j.at("y").get_to(floatVec.y);
}
