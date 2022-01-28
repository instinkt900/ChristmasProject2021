#include "game_pch.h"
#include "layout_entity_image.h"
#include "ui/node_image.h"

namespace ui {
    LayoutEntityImage::LayoutEntityImage() {
    }

    LayoutEntityImage::LayoutEntityImage(nlohmann::json const& json, LayoutEntityGroup* parent)
        : LayoutEntity(json, parent) {
        auto data = json[LABEL];
        data.at("texture_path").get_to(m_texturePath);
        data.at("source_rect").get_to(m_sourceRect);
    }

    std::unique_ptr<Node> LayoutEntityImage::Instantiate() {
        return std::make_unique<NodeImage>(std::static_pointer_cast<LayoutEntityImage>(shared_from_this()));
    }
}
