#pragma once

struct IntRect {
    IntVec2 topLeft;
    IntVec2 bottomRight;
};

struct LayoutVertex {
    FloatVec2 anchor;
    FloatVec2 offset;
};

struct LayoutRect {
    LayoutVertex topLeft{ 0.0f, 0.0f, 0, 0 };
    LayoutVertex bottomRight{ 1.0f, 1.0f, 0, 0 };
};

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

inline void to_json(nlohmann::json& j, IntRect const& rect) {
    j = nlohmann::json{ { "topLeft", rect.topLeft }, { "bottomRight", rect.bottomRight } };
}

inline void from_json(nlohmann::json const& j, IntRect& rect) {
    j.at("topLeft").get_to(rect.topLeft);
    j.at("bottomRight").get_to(rect.bottomRight);
}
