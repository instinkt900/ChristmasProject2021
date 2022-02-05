#pragma once

#include "layout_types.h"
#include "animation_track.h"

namespace ui {
    class LayoutEntityGroup;
    class Node;

    class LayoutEntity : public std::enable_shared_from_this<LayoutEntity> {
    public:
        LayoutEntity();
        LayoutEntity(nlohmann::json const& json, LayoutEntityGroup* parent);

        std::string GetId() const { return m_id; }

        void SetParent(LayoutEntity* parent) { m_parent = parent; }
        LayoutEntity* GetParent() const { return m_parent; }

        LayoutRect const& GetBounds();
        void SetBounds(LayoutRect const& newBounds);

        auto& GetAnimationTracks() const { return m_tracks; }

        void SetCurrentFrame(int frame) {
            m_currentFrame = frame;
            m_cacheDirty = true;
        }

        int GetCurrentFrame() const { return m_currentFrame; }

        virtual std::unique_ptr<Node> Instantiate();

        virtual void OnEditDraw();

    protected:
        std::string m_id;
        LayoutEntity* m_parent = nullptr;
        int m_currentFrame = 0;

        LayoutRect m_cachedBounds;
        bool m_cacheDirty = true;

        std::map<AnimationTrack::Target, std::shared_ptr<AnimationTrack>> m_tracks;

        float GetCurrentValue(AnimationTrack::Target target) const;
        void SetCurrentValue(AnimationTrack::Target target, float value);
        void CacheFrameBounds();
    };
}
