#pragma once

#include "widget_types.h"

enum class WidgetType {
    Node,
    Image,
};

struct WidgetDesc {
    std::string id;
    WidgetType type;
    WidgetBounds layoutBounds;
    std::string imagePath;
    WidgetImageSourceRect imageRect;
    std::vector<WidgetDesc> children;
};

inline void to_json(nlohmann::json& j, WidgetDesc const& desc) {
    j = nlohmann::json{ { "type", desc.type }, { "id", desc.id }, { "layoutBounds", desc.layoutBounds }, { "imagePath", desc.imagePath }, { "imageRect", desc.imageRect }, { "children", desc.children } };
}

inline void from_json(nlohmann::json const& j, WidgetDesc& desc) {
    j.at("id").get_to(desc.id);
    j.at("type").get_to(desc.type);
    j.at("layoutBounds").get_to(desc.layoutBounds);
    j.at("imagePath").get_to(desc.imagePath);
    j.at("imageRect").get_to(desc.imageRect);
    j.at("children").get_to(desc.children);
}
