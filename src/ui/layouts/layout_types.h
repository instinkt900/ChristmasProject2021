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
