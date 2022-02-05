#include "game_pch.h"
#include "layout_entity.h"
#include "layout_entity_group.h"
#include "ui/node.h"
#include "animation_track.h"
#include "debug/inspectors.h"
#include "ui/inspectors.h"

namespace ui {
    LayoutEntity::LayoutEntity() {
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

    LayoutRect const& LayoutEntity::GetBounds() {
        CacheFrameBounds();
        return m_cachedBounds;
    }

    void LayoutEntity::SetBounds(LayoutRect const& newBounds) {
        CacheFrameBounds();
        auto const boundsDelta = m_cachedBounds - newBounds;
        if (boundsDelta.anchor.topLeft.x != 0) {
            SetCurrentValue(AnimationTrack::Target::LeftAnchor, newBounds.anchor.topLeft.x);
        }
        if (boundsDelta.anchor.topLeft.y != 0) {
            SetCurrentValue(AnimationTrack::Target::TopAnchor, newBounds.anchor.topLeft.y);
        }
        if (boundsDelta.anchor.bottomRight.x != 0) {
            SetCurrentValue(AnimationTrack::Target::RightAnchor, newBounds.anchor.bottomRight.x);
        }
        if (boundsDelta.anchor.bottomRight.y != 0) {
            SetCurrentValue(AnimationTrack::Target::BottomAnchor, newBounds.anchor.bottomRight.y);
        }
        if (boundsDelta.offset.topLeft.x != 0) {
            SetCurrentValue(AnimationTrack::Target::LeftOffset, newBounds.offset.topLeft.x);
        }
        if (boundsDelta.offset.topLeft.y != 0) {
            SetCurrentValue(AnimationTrack::Target::TopOffset, newBounds.offset.topLeft.y);
        }
        if (boundsDelta.offset.bottomRight.x != 0) {
            SetCurrentValue(AnimationTrack::Target::RightOffset, newBounds.offset.bottomRight.x);
        }
        if (boundsDelta.offset.bottomRight.y != 0) {
            SetCurrentValue(AnimationTrack::Target::BottomOffset, newBounds.offset.bottomRight.y);
        }
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

    float LayoutEntity::GetCurrentValue(AnimationTrack::Target target) const {
        float value = 0;

        auto const trackIt = m_tracks.find(target);
        if (std::end(m_tracks) != trackIt) {
            const auto track = trackIt->second;
            value = track->GetValueAtFrame(m_currentFrame);
        }

        return value;
    }

    void LayoutEntity::SetCurrentValue(AnimationTrack::Target target, float value) {
        // first get or create the track
        auto track = [&]() {
            auto trackIt = m_tracks.find(target);
            if (std::end(m_tracks) == trackIt) {
                // track doesnt already exist
                auto track = std::make_unique<AnimationTrack>(target);
                auto insertResult = m_tracks.insert(std::make_pair(track->GetTarget(), std::move(track)));
                assert(insertResult.second);
                trackIt = insertResult.first;
            }
            return trackIt->second;
        }();

        auto& keyframe = track->GetKeyframe(m_currentFrame);
        keyframe.m_value = value;
        m_cacheDirty = true;
    }

    void LayoutEntity::CacheFrameBounds() {
        if (m_cacheDirty) {
            m_cachedBounds.anchor.topLeft.x = GetCurrentValue(AnimationTrack::Target::LeftAnchor);
            m_cachedBounds.anchor.topLeft.y = GetCurrentValue(AnimationTrack::Target::TopAnchor);
            m_cachedBounds.anchor.bottomRight.x = GetCurrentValue(AnimationTrack::Target::RightAnchor);
            m_cachedBounds.anchor.bottomRight.y = GetCurrentValue(AnimationTrack::Target::BottomAnchor);
            m_cachedBounds.offset.topLeft.x = GetCurrentValue(AnimationTrack::Target::LeftOffset);
            m_cachedBounds.offset.topLeft.y = GetCurrentValue(AnimationTrack::Target::TopOffset);
            m_cachedBounds.offset.bottomRight.x = GetCurrentValue(AnimationTrack::Target::RightOffset);
            m_cachedBounds.offset.bottomRight.y = GetCurrentValue(AnimationTrack::Target::BottomOffset);
            m_cacheDirty = false;
        }
    }
}
