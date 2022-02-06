#include "game_pch.h"
#include "add_keyframe_action.h"
#include "ui/layouts/layout_entity.h"

namespace ui {
    AddKeyframeAction::AddKeyframeAction(std::shared_ptr<LayoutEntity> entity, AnimationTrack::Target target, int frameNo, float value)
        : m_entity(entity)
        , m_target(target)
        , m_frameNo(frameNo)
        , m_value(value) {
    }

    AddKeyframeAction::~AddKeyframeAction() {
    }

    void AddKeyframeAction::Do() {
        auto& track = m_entity->GetAnimationTracks().at(m_target);
        auto& keyframe = track->GetOrCreateKeyframe(m_frameNo);
        keyframe.m_value = m_value;
    }

    void AddKeyframeAction::Undo() {
        auto& track = m_entity->GetAnimationTracks().at(m_target);
        track->DeleteKeyframe(m_frameNo);
    }

    void AddKeyframeAction::OnImGui() {
        if (ImGui::CollapsingHeader("AddKeyframeAction")) {
            ImGui::LabelText("Frame", "%d", m_frameNo);
            ImGui::LabelText("Value", "%f", m_value);
        }
    }
}
