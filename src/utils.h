#pragma once

template <typename T>
struct Vec2 {
    T x{ 0 };
    T y{ 0 };
};

template <typename T>
struct Rect {
    Vec2<T> topLeft;
    Vec2<T> bottomRight;
};

using IntVec2 = Vec2<int>;
using FloatVec2 = Vec2<float>;

using IntRect = Rect<int>;
using FloatRect = Rect<float>;

template <typename T>
inline Vec2<T>& operator+=(Vec2<T>& a, Vec2<T> const& b) {
    a.x += b.x;
    a.y += b.y;
    return a;
}

template <typename T>
inline Vec2<T>& operator-=(Vec2<T>& a, Vec2<T> const& b) {
    a.x -= b.x;
    a.y -= b.y;
    return a;
}

template <typename T>
inline Rect<T>& operator+=(Rect<T>& a, Rect<T> const& b) {
    a.topLeft += b.topLeft;
    a.bottomRight += b.bottomRight;
    return a;
}

template <typename T>
inline Rect<T>& operator-=(Rect<T>& a, Rect<T> const& b) {
    a.topLeft -= b.topLeft;
    a.bottomRight -= b.bottomRight;
    return a;
}

template <typename T>
inline void to_json(nlohmann::json& j, Vec2<T> const& vec) {
    j = nlohmann::json{ { "x", vec.x }, { "y", vec.y } };
}

template <typename T>
inline void from_json(nlohmann::json const& j, Vec2<T>& vec) {
    j.at("x").get_to(vec.x);
    j.at("y").get_to(vec.y);
}

template <typename T>
inline void to_json(nlohmann::json& j, Rect<T> const& rect) {
    j = nlohmann::json{ { "topLeft", rect.topLeft }, { "bottomRight", rect.bottomRight } };
}

template <typename T>
inline void from_json(nlohmann::json const& j, Rect<T>& rect) {
    j.at("topLeft").get_to(rect.topLeft);
    j.at("bottomRight").get_to(rect.bottomRight);
}

template <typename T, typename U>
inline bool IsInRect(Vec2<T> const& point, Rect<U> const& rect) {
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

struct ViewParameters {
    int m_offsetX;
    int m_offsetY;
    int m_width;
    int m_height;
};

struct PositionComponent;

IntVec2 ResolvePosition(entt::registry& registry, entt::entity entity);
IntVec2 ResolvePosition(entt::registry& registry, PositionComponent const& positionComponent);
