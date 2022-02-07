#include "game_pch.h"
#include "delete_keyframe_action.h"
#include "ui/layouts/layout_entity.h"

namespace ui {
    DeleteKeyframeAction::DeleteKeyframeAction(std::shared_ptr<LayoutEntity> entity, AnimationTrack::Target target, int frameNo, float oldValue)
        : m_entity(entity)
        , m_target(target)
        , m_frameNo(frameNo)
        , m_oldValue(oldValue) {
    }

    DeleteKeyframeAction::~DeleteKeyframeAction() {
    }

    void DeleteKeyframeAction::Do() {
        auto& track = m_entity->GetAnimationTracks().at(m_target);
        track->DeleteKeyframe(m_frameNo);
    }

    void DeleteKeyframeAction::Undo() {
        auto& track = m_entity->GetAnimationTracks().at(m_target);
        auto keyframe = track->GetOrCreateKeyframe(m_frameNo);
        keyframe.m_value = m_oldValue;
    }

    void DeleteKeyframeAction::OnImGui() {
        if (ImGui::CollapsingHeader("DeleteKeyframeAction")) {
            ImGui::LabelText("Frame", "%d", m_frameNo);
            ImGui::LabelText("Old Value", "%f", m_oldValue);
        }
    }
}