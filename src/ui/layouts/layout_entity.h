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

        //LayoutRect& GetBounds() { return m_bounds; }
        //void SetBounds(LayoutRect const& bounds) { m_bounds = bounds; }

        void SetBounds(LayoutRect const& bounds);
        LayoutRect GetBoundsAtTime(float time) const;
        LayoutRect GetBoundsAtFrame(int frame) const;
        //void SetBounds(LayoutRect const& newBounds);

        auto& GetAnimationTracks() const { return m_tracks; }

        //void SetCurrentFrame(int frame);
        //int GetCurrentFrame() const { return m_currentFrame; }

        virtual std::unique_ptr<Node> Instantiate();

        virtual void OnEditDraw();

    protected:
        std::string m_id;
        LayoutEntity* m_parent = nullptr;

        //int m_currentFrame = -1;
        //LayoutRect m_bounds;

        std::map<AnimationTrack::Target, std::shared_ptr<AnimationTrack>> m_tracks;
        void InitTracks(LayoutRect const& initialRect);

        //float GetCurrentValue(AnimationTrack::Target target) const;
        //void SetCurrentValue(AnimationTrack::Target target, float value);
        //void CacheFrameBounds();
    };
}
