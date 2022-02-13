#pragma once

#include "editor_action.h"
#include "ui/layouts/animation_track.h"

namespace ui {
    class LayoutEntity;

    class ModifyKeyframeAction : public IEditorAction {
    public:
        ModifyKeyframeAction(std::shared_ptr<LayoutEntity> entity, AnimationTrack::Target target, int frameNo, float oldValue, float newValue);
        virtual ~ModifyKeyframeAction();

        void Do() override;
        void Undo() override;

        void OnImGui() override;

    protected:
        std::shared_ptr<LayoutEntity> m_entity;
        AnimationTrack::Target m_target;
        int m_frameNo;
        float m_oldValue;
        float m_newValue;
    };
}
