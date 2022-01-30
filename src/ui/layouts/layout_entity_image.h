#pragma once

#include "layout_entity.h"

namespace ui {
    class LayoutEntityImage : public LayoutEntity {
    public:
        LayoutEntityImage();
        LayoutEntityImage(nlohmann::json const& json, LayoutEntityGroup* parent);

        std::unique_ptr<Node> Instantiate() override;

        static constexpr char const* LABEL = "image_data";

        std::string m_texturePath;
        IntRect m_sourceRect;
    };
}
