#pragma once

struct ViewParameters {
    int m_offsetX;
    int m_offsetY;
    int m_width;
    int m_height;
};

template<typename T>
struct Vec2 {
    T x{ 0 };
    T y{ 0 };
};

using IntVec2 = Vec2<int>;
using FloatVec2 = Vec2<float>;

struct PositionComponent;

IntVec2 ResolvePosition(entt::registry& registry, entt::entity entity);
IntVec2 ResolvePosition(entt::registry& registry, PositionComponent const& positionComponent);