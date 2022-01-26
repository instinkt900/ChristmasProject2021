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

struct IntRect {
    IntVec2 topLeft;
    IntVec2 bottomRight;
};

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

inline void to_json(nlohmann::json& j, IntRect const& rect) {
    j = nlohmann::json{ { "topLeft", rect.topLeft }, { "bottomRight", rect.bottomRight } };
}

inline void from_json(nlohmann::json const& j, IntRect& rect) {
    j.at("topLeft").get_to(rect.topLeft);
    j.at("bottomRight").get_to(rect.bottomRight);
}

inline bool IsInRect(IntVec2 const& point, IntRect const& rect) {
    if (point.x > rect.bottomRight.x || point.x < rect.topLeft.x ||
        point.y > rect.bottomRight.y || point.y < rect.topLeft.y) {
        return false;
    }
    return true;
}

template <typename T>
inline T lerp(T const& a, T const& b, float factor) {
    return a + (b - a) * factor;
}
