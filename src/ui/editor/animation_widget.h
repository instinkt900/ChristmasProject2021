#pragma once

#include "im_animation_tracks.h"
#include "ui/layouts/animation_track.h"

namespace ui {
    class EditorLayer;
    class Group;
    class CompositeAction;
    class LayoutEntity;

    class AnimationWidget {
    public:
        AnimationWidget(EditorLayer& editorLayer, Group* group);

        void Draw();

    private:
        EditorLayer& m_editorLayer;
        Group* m_group;

        bool DrawWidget();
        int GetClipCount() const;
        void GetClip(int clipIndex, int** startFrame, int** endFrame, char const** clipName, unsigned int* color);
        void BeginEditClip(int clipIndex) {}
        void EndEditClip() {}
        char const* GetChildLabel(int index) const;
        char const* GetTrackLabel(AnimationTrack::Target target) const;

        struct KeyframeContext {
            std::shared_ptr<LayoutEntity> entity;
            AnimationTrack::Target target;
            int initialFrame;
        };
        std::map<Keyframe*, KeyframeContext> m_movingKeyframes;

        void BeginMoveKeyframe(Keyframe* keyframe, std::shared_ptr<LayoutEntity> entity, AnimationTrack::Target target);
        void EndMoveKeyframe();

        int m_minFrame = 0;
        int m_maxFrame = 100;
        int m_currentFrame = 0;
        int m_firstFrame = 0;

        std::vector<bool> m_childExpanded;
    };
}
