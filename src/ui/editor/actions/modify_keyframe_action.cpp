#include "game_pch.h"
#include "modify_keyframe_action.h"
#include "ui/layouts/layout_entity.h"

namespace ui {
    ModifyKeyframeAction::ModifyKeyframeAction(std::shared_ptr<LayoutEntity> entity, AnimationTrack::Target target, int frameNo, float oldValue, float newValue)
        : m_entity(entity)
        , m_target(target)
        , m_frameNo(frameNo)
        , m_oldValue(oldValue)
        , m_newValue(newValue) {
    }

    ModifyKeyframeAction::~ModifyKeyframeAction() {
    }

    void ModifyKeyframeAction::Do() {
        auto& track = m_entity->GetAnimationTracks().at(m_target);
        auto keyframe = track->GetKeyframe(m_frameNo);
        keyframe->m_value = m_newValue;
    }

    void ModifyKeyframeAction::Undo() {
        auto& track = m_entity->GetAnimationTracks().at(m_target);
        auto keyframe = track->GetKeyframe(m_frameNo);
        keyframe->m_value = m_oldValue;
    }

    void ModifyKeyframeAction::OnImGui() {
        if (ImGui::CollapsingHeader("AddKeyframeAction")) {
            ImGui::LabelText("Frame", "%d", m_frameNo);
            ImGui::LabelText("Old Value", "%f", m_oldValue);
            ImGui::LabelText("New Value", "%f", m_newValue);
        }
    }
}
