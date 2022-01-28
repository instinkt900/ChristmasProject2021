#pragma once

#include "layout_entity.h"

namespace ui {
    class LayoutEntityImage : public LayoutEntity {
    public:
        LayoutEntityImage();
        LayoutEntityImage(nlohmann::json const& json, LayoutEntityGroup* parent);

        std::string const& GetTexturePath() const { return m_texturePath; }
        IntRect const& GetSourceRect() const { return m_sourceRect; }

        std::unique_ptr<Node> Instantiate() override;

        static constexpr char const* LABEL = "image_data";

    protected:
        std::string m_texturePath;
        IntRect m_sourceRect;
    };
}
