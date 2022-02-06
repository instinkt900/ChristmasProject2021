#pragma once

#include "layout_types.h"
#include "animation_track.h"

namespace ui {
    class LayoutEntityGroup;
    class Node;

    class LayoutEntity : public std::enable_shared_from_this<LayoutEntity> {
    public:
        LayoutEntity(LayoutRect const& initialBounds);
        LayoutEntity(nlohmann::json const& json, LayoutEntityGroup* parent);

        std::string GetId() const { return m_id; }

        void SetParent(LayoutEntity* parent) { m_parent = parent; }
        LayoutEntity* GetParent() const { return m_parent; }

        void SetBounds(LayoutRect const& bounds);
        LayoutRect GetBoundsAtTime(float time) const;
        LayoutRect GetBoundsAtFrame(int frame) const;

        auto& GetAnimationTracks() const { return m_tracks; }

        virtual std::unique_ptr<Node> Instantiate();

        virtual void OnEditDraw();

    protected:
        std::string m_id;
        LayoutEntity* m_parent = nullptr;

        std::map<AnimationTrack::Target, std::shared_ptr<AnimationTrack>> m_tracks;
        void InitTracks(LayoutRect const& initialRect);
    };
}
