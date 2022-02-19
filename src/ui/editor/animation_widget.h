#pragma once

#include "ui/layouts/animation_track.h"
#include "keyframe_context.h"
#include "keyframe_widget.h"
#include "ui/layouts/animation_clip.h"

namespace ui {
    class EditorLayer;
    class Group;
    class CompositeAction;
    class LayoutEntity;

    class AnimationWidget {
    public:
        AnimationWidget(EditorLayer& editorLayer, Group* group);

        void Draw();

        void OnUndo();
        void OnRedo();

    private:
        EditorLayer& m_editorLayer;
        Group* m_group;

        bool DrawWidget();
        char const* GetChildLabel(int index) const;
        char const* GetTrackLabel(AnimationTrack::Target target) const;

        imgui_ext::FocusGroupContext m_clipInputFocusContext;
        AnimationClip* m_targetClip = nullptr;
        AnimationClip m_preModifyClipValues;
        void BeginEditClip(AnimationClip& clip);
        void EndEditClip();

        bool m_clipWindowShown = true;
        void DrawSelectedClipWindow();

        std::vector<KeyframeContext> m_selectedKeyframes;
        KeyframeWidget m_keyframeWidget;
        AnimationClip* m_selectedClip = nullptr;

        void SelectKeyframe(std::shared_ptr<LayoutEntity> entity, AnimationTrack::Target target, int frameNo);
        void DeselectKeyframe(std::shared_ptr<LayoutEntity> entity, AnimationTrack::Target target, int frameNo);
        bool IsKeyframeSelected(std::shared_ptr<LayoutEntity> entity, AnimationTrack::Target target, int frameNo);
        void ClearSelectedKeyframes();
        void ClearNonMatchingKeyframes(std::shared_ptr<LayoutEntity> entity, int frameNo);

        void DeleteSelectedKeyframes();

        void EndMoveKeyframes();

        int m_minFrame = 0;
        int m_maxFrame = 100;
        int m_currentFrame = 0;
        int m_firstFrame = 0;

        std::vector<bool> m_childExpanded;
    };
}
