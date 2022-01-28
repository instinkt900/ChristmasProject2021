#pragma once

#include "layout_types.h"

namespace ui {
    class LayoutEntityGroup;
    class Node;
    class AnimationTrack;

    class LayoutEntity : public std::enable_shared_from_this<LayoutEntity> {
    public:
        LayoutEntity();
        LayoutEntity(nlohmann::json const& json, LayoutEntityGroup* parent);

        std::string GetId() const { return m_id; }

        LayoutRect& GetBounds() { return m_bounds; }
        LayoutRect const& GetBounds() const { return m_bounds; }

        void SetParent(LayoutEntity* parent) { m_parent = parent; }
        LayoutEntity* GetParent() const { return m_parent; }

        auto& GetAnimationTracks() const { return m_tracks; }

        virtual std::unique_ptr<Node> Instantiate();

        virtual void OnEditDraw();

    protected:
        std::string m_id;
        LayoutRect m_bounds;
        LayoutEntity* m_parent = nullptr;

        std::vector<std::shared_ptr<AnimationTrack>> m_tracks;
    };
}
