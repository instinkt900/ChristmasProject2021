#include "game_pch.h"
#include "animation_track.h"
#include "animation_clip.h"

namespace ui {
    NLOHMANN_JSON_SERIALIZE_ENUM(AnimationTrack::Target, {
                                                             { AnimationTrack::Target::TopOffset, "top_offset" },
                                                             { AnimationTrack::Target::BottomOffset, "bottom_offset" },
                                                             { AnimationTrack::Target::LeftOffset, "left_offset" },
                                                             { AnimationTrack::Target::RightOffset, "right_offset" },
                                                             { AnimationTrack::Target::TopAnchor, "top_anchor" },
                                                             { AnimationTrack::Target::BottomAnchor, "bottom_anchor" },
                                                             { AnimationTrack::Target::LeftAnchor, "left_anchor" },
                                                             { AnimationTrack::Target::RightAnchor, "right_anchor" },
                                                         })

    AnimationTrack::AnimationTrack(nlohmann::json const& json)
        : m_target(json["target"]) {
        for (auto&& keyframeJson : json["keyframes"]) {
            m_keyframes.push_back({ keyframeJson["frame"], keyframeJson["value"] });
        }
    }

    void AnimationTrack::UpdateTrackTimings(std::vector<std::unique_ptr<AnimationClip>> const& clips) {
        auto keyframeIt = std::begin(m_keyframes);
        for (auto&& clip : clips) {
            // seek to the start of the clip
            while (keyframeIt != std::end(m_keyframes) && keyframeIt->m_frame < clip->GetStartFrame()) {
                ++keyframeIt;
            }

            // cycle through all keyframes in clip
            while (keyframeIt != std::end(m_keyframes) && keyframeIt->m_frame <= clip->GetEndFrame()) {
                int const frameDelta = keyframeIt->m_frame - clip->GetStartFrame();
                float const timeDelta = frameDelta / clip->GetFPS();
                keyframeIt->m_time = clip->GetStartTime() + timeDelta;
                ++keyframeIt;
            }
        }
    }
}
