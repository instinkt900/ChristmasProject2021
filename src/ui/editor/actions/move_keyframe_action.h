#pragma once

#include "editor_action.h"
#include "ui/layouts/animation_track.h"
#include "ui/ui_fwd.h"

namespace ui {
    class MoveKeyframeAction : public IEditorAction {
    public:
        MoveKeyframeAction(std::shared_ptr<LayoutEntity> entity, AnimationTrack::Target target, int initialFrame, int finalFrame, std::optional<KeyframeValue> replacedValue);
        virtual ~MoveKeyframeAction();

        void Do() override;
        void Undo() override;

        void OnImGui() override;

    protected:
        std::shared_ptr<LayoutEntity> m_entity;
        AnimationTrack::Target m_target;
        int m_initialFrame;
        int m_finalFrame;
        std::optional<KeyframeValue> m_replacedValue;
    };
}
