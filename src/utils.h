#pragma once

struct ViewParameters {
    int m_offsetX;
    int m_offsetY;
    int m_width;
    int m_height;
};

struct IntVec2 {
    int x = 0;
    int y = 0;
};

struct PositionComponent;

IntVec2 ResolvePosition(entt::registry& registry, entt::entity entity);
IntVec2 ResolvePosition(entt::registry& registry, PositionComponent const& positionComponent);