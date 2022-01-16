#include "game_pch.h"
#include "animation_track.h"
#include "animation_clip_info.h"

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

    AnimationTrack::AnimationTrack(AnimationClipInfo const& info, nlohmann::json const& json)
        : m_target(json["target"]) {
        float frameTime;
        for (auto&& keyframeJson : json["keyframes"]) {
            keyframeJson.at("time").get_to(frameTime);
            if (frameTime >= info.GetStartTime()) {
                frameTime -= info.GetStartTime();
                m_keyframes.push_back({ frameTime, keyframeJson["value"] });
            } else if (frameTime > info.GetEndTime()) {
                break;
            }
        }
    }

    float AnimationTrack::GetValue(float time) const {
        Keyframe const* firstKey = nullptr;
        Keyframe const* secondKey = nullptr;
        for (auto&& keyframe : m_keyframes) {
            if (time < keyframe.m_time) {
                secondKey = &keyframe;
                break;
            }
            firstKey = &keyframe;
        }

        if (firstKey && secondKey) {
            return firstKey->m_value + (secondKey->m_value - firstKey->m_value) * (time - firstKey->m_time);
        }

        return m_keyframes[0].m_value;
    }
}
