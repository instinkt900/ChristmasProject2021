#include "game_pch.h"
#include "animation_clip.h"
#include "animation_clip_info.h"
#include "animation_track.h"

namespace ui {
    AnimationClip::AnimationClip(AnimationClipInfo const& info, nlohmann::json const& tracksJson)
        : m_name(info.GetName()) {
        for (auto&& trackJson : tracksJson) {
            m_tracks.push_back(std::make_shared<AnimationTrack>(info, trackJson));
        }
    }
}
