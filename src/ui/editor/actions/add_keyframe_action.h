#pragma once

#include "editor_action.h"
#include "ui/layouts/animation_track.h"

namespace ui {
    class LayoutEntity;

    class AddKeyframeAction : public IEditorAction {
    public:
        AddKeyframeAction(std::shared_ptr<LayoutEntity> entity, AnimationTrack::Target target, int frameNo, float value);
        virtual ~AddKeyframeAction();

        void Do() override;
        void Undo() override;

        void OnImGui() override;

    protected:
        std::shared_ptr<LayoutEntity> m_entity;
        AnimationTrack::Target m_target;
        int m_frameNo;
        float m_value;
    };
}
