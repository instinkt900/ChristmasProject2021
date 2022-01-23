#pragma once

#include "layout_entity.h"

namespace ui {
    class AnimationClipInfo;

    class LayoutEntityGroup : public LayoutEntity {
    public:
        LayoutEntityGroup(nlohmann::json const& json, LayoutEntityGroup* parent);

        auto& GetChildren() { return m_children; }
        auto const& GetChildren() const { return m_children; }

        auto& GetAnimationClips() { return m_animationClips; }
        auto const& GetAnimationClips() const { return m_animationClips; }

        std::unique_ptr<Node> Instantiate() override;

        void OnEditDraw() override;

        static constexpr char const* LABEL = "layout_data";

    protected:
        std::string m_layoutPath;
        std::vector<std::shared_ptr<LayoutEntity>> m_children;
        std::map<std::string, std::shared_ptr<AnimationClipInfo>> m_animationClips;

        void Clone(LayoutEntityGroup const& other);
    };
}
