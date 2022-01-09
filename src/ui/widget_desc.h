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
    j = nlohmann::json{ { "id", desc.id }, { "type", desc.type }, { "layoutBounds", desc.layoutBounds }, { "imagePath", desc.imagePath }, { "imageRect", desc.imageRect }, { "children", desc.children } };
}

inline void from_json(nlohmann::json const& j, WidgetDesc& desc) {
    j.at("id").get_to(desc.id);
    j.at("type").get_to(desc.type);
    j.at("layoutBounds").get_to(desc.layoutBounds);
    if (j.contains("imagePath")) {
        j.at("imagePath").get_to(desc.imagePath);
        j.at("imageRect").get_to(desc.imageRect);
    }
    if (j.contains("children")) {
        j.at("children").get_to(desc.children);
    }
}
