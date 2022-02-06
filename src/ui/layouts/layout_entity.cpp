#include "game_pch.h"
#include "layout_entity.h"
#include "layout_entity_group.h"
#include "ui/node.h"
#include "animation_track.h"
#include "debug/inspectors.h"
#include "ui/inspectors.h"

namespace ui {
    LayoutEntity::LayoutEntity(LayoutRect const& initialBounds) {
        InitTracks(initialBounds);
    }

    LayoutEntity::LayoutEntity(nlohmann::json const& json, LayoutEntityGroup* parent)
        : m_id(json["id"])
        , m_parent(parent) {

        if (json.contains("tracks")) {
            auto const& animationClips = parent->GetAnimationClips();
            auto const& tracksJson = json["tracks"];
            for (auto&& trackJson : tracksJson) {
                auto track = std::make_unique<AnimationTrack>(trackJson);
                track->UpdateTrackTimings(animationClips);
                m_tracks.insert(std::make_pair(track->GetTarget(), std::move(track)));
            }
        }
    }

    void LayoutEntity::SetBounds(LayoutRect const& bounds) {
        auto SetValue = [&](AnimationTrack::Target target, float value) {
            auto track = m_tracks.at(target);
            auto keyframe = track->GetKeyframe(0);
            keyframe->m_value = value;
        };

        SetValue(AnimationTrack::Target::LeftAnchor, bounds.anchor.topLeft.x);
        SetValue(AnimationTrack::Target::TopAnchor, bounds.anchor.topLeft.y);
        SetValue(AnimationTrack::Target::RightAnchor, bounds.anchor.bottomRight.x);
        SetValue(AnimationTrack::Target::BottomAnchor, bounds.anchor.bottomRight.y);
        SetValue(AnimationTrack::Target::LeftOffset, bounds.offset.topLeft.x);
        SetValue(AnimationTrack::Target::TopOffset, bounds.offset.topLeft.y);
        SetValue(AnimationTrack::Target::RightOffset, bounds.offset.bottomRight.x);
        SetValue(AnimationTrack::Target::BottomOffset, bounds.offset.bottomRight.y);
    }

    LayoutRect LayoutEntity::GetBoundsAtTime(float time) const {
        auto GetValue = [&](AnimationTrack::Target target) {
            float value = 0;
            auto const trackIt = m_tracks.find(target);
            if (std::end(m_tracks) != trackIt) {
                const auto track = trackIt->second;
                value = track->GetValueAtTime(time);
            }
            return value;
        };

        LayoutRect bounds;
        bounds.anchor.topLeft.x = GetValue(AnimationTrack::Target::LeftAnchor);
        bounds.anchor.topLeft.y = GetValue(AnimationTrack::Target::TopAnchor);
        bounds.anchor.bottomRight.x = GetValue(AnimationTrack::Target::RightAnchor);
        bounds.anchor.bottomRight.y = GetValue(AnimationTrack::Target::BottomAnchor);
        bounds.offset.topLeft.x = GetValue(AnimationTrack::Target::LeftOffset);
        bounds.offset.topLeft.y = GetValue(AnimationTrack::Target::TopOffset);
        bounds.offset.bottomRight.x = GetValue(AnimationTrack::Target::RightOffset);
        bounds.offset.bottomRight.y = GetValue(AnimationTrack::Target::BottomOffset);
        return bounds;
    }

    LayoutRect LayoutEntity::GetBoundsAtFrame(int frame) const {
        auto GetValue = [&](AnimationTrack::Target target) {
            float value = 0;
            auto const trackIt = m_tracks.find(target);
            if (std::end(m_tracks) != trackIt) {
                const auto track = trackIt->second;
                value = track->GetValueAtFrame(frame);
            }
            return value;
        };

        LayoutRect bounds;
        bounds.anchor.topLeft.x = GetValue(AnimationTrack::Target::LeftAnchor);
        bounds.anchor.topLeft.y = GetValue(AnimationTrack::Target::TopAnchor);
        bounds.anchor.bottomRight.x = GetValue(AnimationTrack::Target::RightAnchor);
        bounds.anchor.bottomRight.y = GetValue(AnimationTrack::Target::BottomAnchor);
        bounds.offset.topLeft.x = GetValue(AnimationTrack::Target::LeftOffset);
        bounds.offset.topLeft.y = GetValue(AnimationTrack::Target::TopOffset);
        bounds.offset.bottomRight.x = GetValue(AnimationTrack::Target::RightOffset);
        bounds.offset.bottomRight.y = GetValue(AnimationTrack::Target::BottomOffset);
        return bounds;
    }

    std::unique_ptr<Node> LayoutEntity::Instantiate() {
        return std::make_unique<Node>(shared_from_this());
    }

    void LayoutEntity::OnEditDraw() {
        if (ImGui::TreeNode("node")) {
            ImGuiInspectMember("id", m_id);
            //ImGuiInspectMember("bounds", m_bounds);
            ImGui::TreePop();
        }
    }

    void LayoutEntity::InitTracks(LayoutRect const& initialRect) {
        auto const targetList = {
            std::make_pair(AnimationTrack::Target::TopAnchor, initialRect.anchor.topLeft.y),
            std::make_pair(AnimationTrack::Target::LeftAnchor, initialRect.anchor.topLeft.x),
            std::make_pair(AnimationTrack::Target::BottomAnchor, initialRect.anchor.bottomRight.y),
            std::make_pair(AnimationTrack::Target::RightAnchor, initialRect.anchor.bottomRight.x),
            std::make_pair(AnimationTrack::Target::TopOffset, initialRect.offset.topLeft.y),
            std::make_pair(AnimationTrack::Target::LeftOffset, initialRect.offset.topLeft.x),
            std::make_pair(AnimationTrack::Target::BottomOffset, initialRect.offset.bottomRight.y),
            std::make_pair(AnimationTrack::Target::RightOffset, initialRect.offset.bottomRight.x),
        };
        for (auto&& [target, value] : targetList) {
            if (std::end(m_tracks) == m_tracks.find(target)) {
                m_tracks.insert(std::make_pair(target, std::make_shared<AnimationTrack>(target, value)));
            }
        }
    }
}
