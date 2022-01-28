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
            return layoutRect.topLeft.offset.y;
        case AnimationTrack::Target::BottomOffset:
            return layoutRect.bottomRight.offset.y;
        case AnimationTrack::Target::LeftOffset:
            return layoutRect.topLeft.offset.x;
        case AnimationTrack::Target::RightOffset:
            return layoutRect.bottomRight.offset.x;
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
        auto endKeyframeIt = std::end(m_track.m_keyframes);
        auto firstKeyframeIt = std::begin(m_track.m_keyframes);
        auto secondKeyframeIt = firstKeyframeIt;
        while (secondKeyframeIt != endKeyframeIt && secondKeyframeIt->m_time < time) {
            firstKeyframeIt = secondKeyframeIt;
            ++secondKeyframeIt;
        }

        if (endKeyframeIt == firstKeyframeIt && endKeyframeIt == secondKeyframeIt) {
            // no keyframes no change
        } else if (endKeyframeIt != firstKeyframeIt && endKeyframeIt != secondKeyframeIt) {
            float const deltaTime = time - firstKeyframeIt->m_time;
            m_target = lerp(firstKeyframeIt->m_value, secondKeyframeIt->m_value, deltaTime);
        } else {
            auto validKeyframeIt = firstKeyframeIt != endKeyframeIt ? firstKeyframeIt : secondKeyframeIt;
            m_target = validKeyframeIt->m_value;
        }
    }

    AnimationController::AnimationController(Node* node, std::vector<std::shared_ptr<AnimationTrack>> tracks) {
        for (auto&& track : tracks) {
            m_trackControllers.push_back(std::make_unique<AnimationTrackController>(GetTargetReference(node, track->m_target), *track));
        }
    }

    void AnimationController::SetTime(float time) {
        for (auto&& track : m_trackControllers) {
            track->SetTime(time);
        }
    }
}
