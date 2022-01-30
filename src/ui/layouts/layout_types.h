#pragma once

namespace ui {
    struct LayoutVertex {
        FloatVec2 anchor;
        FloatVec2 offset;
    };

    struct LayoutRect {
        LayoutVertex topLeft{ 0.0f, 0.0f, 0, 0 };
        LayoutVertex bottomRight{ 1.0f, 1.0f, 0, 0 };
    };

    inline LayoutVertex& operator+=(LayoutVertex& a, LayoutVertex const& b) {
        a.anchor += b.anchor;
        a.offset += b.offset;
        return a;
    }

    inline LayoutVertex& operator-=(LayoutVertex& a, LayoutVertex const& b) {
        a.anchor -= b.anchor;
        a.offset -= b.offset;
        return a;
    }

    inline LayoutRect& operator+=(LayoutRect& a, LayoutRect const& b) {
        a.topLeft += b.topLeft;
        a.bottomRight += b.bottomRight;
        return a;
    }

    inline LayoutRect& operator-=(LayoutRect& a, LayoutRect const& b) {
        a.topLeft -= b.topLeft;
        a.bottomRight -= b.bottomRight;
        return a;
    }

    inline LayoutRect operator+(LayoutRect const& a, LayoutRect const& b) {
        LayoutRect r = a;
        r += b;
        return r;
    }

    inline LayoutRect operator-(LayoutRect const& a, LayoutRect const& b) {
        LayoutRect r = a;
        r -= b;
        return r;
    }

    inline void to_json(nlohmann::json& j, LayoutVertex const& vertex) {
        j = nlohmann::json{ { "anchor", vertex.anchor }, { "offset", vertex.offset } };
    }

    inline void from_json(nlohmann::json const& j, LayoutVertex& vertex) {
        j.at("anchor").get_to(vertex.anchor);
        j.at("offset").get_to(vertex.offset);
    }

    inline void to_json(nlohmann::json& j, LayoutRect const& rect) {
        j = nlohmann::json{ { "topLeft", rect.topLeft }, { "bottomRight", rect.bottomRight } };
    }

    inline void from_json(nlohmann::json const& j, LayoutRect& rect) {
        j.at("topLeft").get_to(rect.topLeft);
        j.at("bottomRight").get_to(rect.bottomRight);
    }
}
