#pragma once

#include "im_animation_tracks.h"

namespace ui {
    class Group;

    class AnimationEditContext : public ImAnimation::IAnimation {
    public:
        AnimationEditContext(Group* group);

        void BeginEdit(int /*index*/) override {}
        void EndEdit() override {}

        int GetClipCount() const override;
        void GetClip(int clipIndex, int** startFrame, int** endFrame, char const** clipName, unsigned int* color) override;

        int GetTrackCount() const override;
        char const* GetTrackLabel(int index) const override;
        void GetKeyframes(int trackIndex, int** keyframes, int* keyframeCount) override;

        void DoubleClick(int /*index*/) override {}

        void Update();
        void Draw();

    private:
        struct TrackDesc {
            std::string m_name;
            std::vector<int> m_keys;
        };
        struct TrackGroup : public TrackDesc {
            std::vector<TrackDesc> m_subTracks;
        };
        Group* m_group;
        std::vector<TrackGroup> m_tracks;

        int m_minFrame = 0;
        int m_maxFrame = 100;
        int m_currentFrame = 0;
        int m_firstFrame = 0;
        bool m_expanded = true;
    };
}
