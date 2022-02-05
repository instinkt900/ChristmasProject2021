#pragma once

#include "im_animation_tracks.h"
#include "ui/layouts/animation_track.h"

namespace ui {
    class Group;

    class AnimationWidget {
    public:
        AnimationWidget(Group* group);

        void Draw();

    private:
        Group* m_group;

        bool DrawWidget();
        int GetClipCount() const;
        void GetClip(int clipIndex, int** startFrame, int** endFrame, char const** clipName, unsigned int* color);
        void BeginEditClip(int clipIndex) {}
        void EndEditClip() {}
        char const* GetChildLabel(int index) const;
        char const* GetTrackLabel(AnimationTrack::Target target) const;

        int m_minFrame = 0;
        int m_maxFrame = 100;
        int m_currentFrame = 0;
        int m_firstFrame = 0;

        bool m_expanded = true;
        std::vector<bool> m_childExpanded;
    };
}
