#pragma once

struct WidgetVertex {
    FloatVec2 anchor;
    FloatVec2 offset;
};

struct WidgetBounds {
    WidgetVertex topLeft{ 0.0f, 0.0f, 0, 0 };
    WidgetVertex bottomRight{ 1.0f, 1.0f, 0, 0 };
};

struct WidgetRect {
    IntVec2 topLeft{ 0, 0 };
    IntVec2 bottomRight{ 0, 0 };
};

struct WidgetImageSourceRect {
    IntVec2 position;
    IntVec2 dimensions;
};

inline void to_json(nlohmann::json& j, IntVec2 const& intVec) {
    j = nlohmann::json{ { "x", intVec.x }, { "y", intVec.y } };
}

inline void to_json(nlohmann::json& j, FloatVec2 const& floatVec) {
    j = nlohmann::json{ { "x", floatVec.x }, { "y", floatVec.y } };
}

inline void to_json(nlohmann::json& j, WidgetVertex const& vertex) {
    j = nlohmann::json{ { "anchor", vertex.anchor }, { "offset", vertex.offset } };
}

inline void to_json(nlohmann::json& j, WidgetBounds const& bounds) {
    j = nlohmann::json{ { "topLeft", bounds.topLeft }, { "bottomRight", bounds.bottomRight } };
}

inline void to_json(nlohmann::json& j, WidgetImageSourceRect const& rect) {
    j = nlohmann::json{ { "position", rect.position }, { "dimensions", rect.dimensions } };
}

inline void from_json(nlohmann::json const& j, FloatVec2& floatVec) {
    j.at("x").get_to(floatVec.x);
    j.at("y").get_to(floatVec.y);
}

inline void from_json(nlohmann::json const& j, IntVec2& intVec) {
    j.at("x").get_to(intVec.x);
    j.at("y").get_to(intVec.y);
}

inline void from_json(nlohmann::json const& j, WidgetVertex& vertex) {
    j.at("anchor").get_to(vertex.anchor);
    j.at("offset").get_to(vertex.offset);
}

inline void from_json(nlohmann::json const& j, WidgetBounds& bounds) {
    j.at("topLeft").get_to(bounds.topLeft);
    j.at("bottomRight").get_to(bounds.bottomRight);
}

inline void from_json(nlohmann::json const& j, WidgetImageSourceRect& rect) {
    j.at("position").get_to(rect.position);
    j.at("dimensions").get_to(rect.dimensions);
}

inline void ImGuiInspectMember(char const* label, WidgetBounds& widgetBounds) {
    ImGui::PushItemWidth(50);
    ImGui::PushID(&widgetBounds.topLeft.offset.y);
    ImGui::InputFloat("", &widgetBounds.topLeft.offset.y, 0, 0, "%.0f");
    ImGui::PopID();
    ImGui::SameLine();
    ImGui::InputFloat("Top", &widgetBounds.topLeft.anchor.y, 0, 0, "%.2f");
    ImGui::PushID(&widgetBounds.bottomRight.offset.y);
    ImGui::InputFloat("", &widgetBounds.bottomRight.offset.y, 0, 0, "%.0f");
    ImGui::PopID();
    ImGui::SameLine();
    ImGui::InputFloat("Bottom", &widgetBounds.bottomRight.anchor.y, 0, 0, "%.2f");
    ImGui::PushID(&widgetBounds.topLeft.offset.x);
    ImGui::InputFloat("", &widgetBounds.topLeft.offset.x, 0, 0, "%.0f");
    ImGui::PopID();
    ImGui::SameLine();
    ImGui::InputFloat("Left", &widgetBounds.topLeft.anchor.x, 0, 0, "%.2f");
    ImGui::PushID(&widgetBounds.bottomRight.offset.x);
    ImGui::InputFloat("", &widgetBounds.bottomRight.offset.x, 0, 0, "%.0f");
    ImGui::PopID();
    ImGui::SameLine();
    ImGui::InputFloat("Right", &widgetBounds.bottomRight.anchor.x, 0, 0, "%.2f");
    ImGui::PopItemWidth();
}

inline void ImGuiInspectMember(char const* label, WidgetRect& widgetRect) {
    ImGui::PushItemWidth(108);
    ImGui::InputInt("Top", &widgetRect.topLeft.y, 0);
    ImGui::InputInt("Left", &widgetRect.topLeft.x, 0);
    ImGui::InputInt("Bottom", &widgetRect.bottomRight.y, 0);
    ImGui::InputInt("Right", &widgetRect.bottomRight.x, 0);
    ImGui::PopItemWidth();
}
