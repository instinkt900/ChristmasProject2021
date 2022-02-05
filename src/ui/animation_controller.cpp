#include "game_pch.h"
#include "animation_controller.h"
#include "layouts/animation_track.h"
#include "node.h"

namespace {
    using namespace ui;

    float& GetTargetReference(Node* node, AnimationTrack::Target target) {
        auto& layoutRect = node->GetLayoutRect();
        switch (target) {
        case AnimationTrack::Target::TopOffset:
            return layoutRect.offset.topLeft.y;
        case AnimationTrack::Target::BottomOffset:
            return layoutRect.offset.bottomRight.y;
        case AnimationTrack::Target::LeftOffset:
            return layoutRect.offset.topLeft.x;
        case AnimationTrack::Target::RightOffset:
            return layoutRect.offset.bottomRight.x;
        }

        // just a value dump for unknown targets
        static float dummy = 0;
        return dummy;
    }
}

namespace ui {
    AnimationTrackController::AnimationTrackController(float& target, AnimationTrack& track)
        : m_target(target)
        , m_track(track) {
    }

    void AnimationTrackController::SetTime(float time) {
        m_target = m_track.GetValueAtTime(time);
    }

    AnimationController::AnimationController(Node* node, std::map<AnimationTrack::Target, std::shared_ptr<AnimationTrack>> const& tracks) {
        for (auto&& [target, track] : tracks) {
            m_trackControllers.push_back(std::make_unique<AnimationTrackController>(GetTargetReference(node, target), *track));
        }
    }

    void AnimationController::SetTime(float time) {
        for (auto&& track : m_trackControllers) {
            track->SetTime(time);
        }
    }
}
