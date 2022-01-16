#pragma once

#include "layout_types.h"

namespace ui {
    class LayoutEntityGroup;
    class Node;
    class AnimationClip;

    class LayoutEntity : public std::enable_shared_from_this<LayoutEntity> {
    public:
        LayoutEntity(nlohmann::json const& json, LayoutEntityGroup* parent);

        std::string GetId() const { return m_id; }
        LayoutRect const& GetBounds() const { return m_bounds; }

        void SetParent(LayoutEntity* parent) { m_parent = parent; }
        LayoutEntity* GetParent() const { return m_parent; }

        auto& GetAnimationClips() { return m_animationClips; }
        auto const& GetAnimationClips() const { return m_animationClips; }

        virtual std::unique_ptr<Node> Instantiate();

    protected:
        std::string m_id;
        LayoutRect m_bounds;
        LayoutEntity* m_parent = nullptr;

        std::map<std::string, std::shared_ptr<AnimationClip>> m_animationClips;
    };
}
