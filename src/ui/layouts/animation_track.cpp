#include "game_pch.h"
#include "animation_track.h"
#include "animation_clip.h"

namespace ui {
    AnimationTrack::AnimationTrack(Target target, float initialValue)
        : m_target(target) {
        m_keyframes.push_back({ 0, initialValue });
    }

    AnimationTrack::AnimationTrack(nlohmann::json const& json)
        /*: m_target(json["m_target"])*/ {
        //for (auto&& keyframeJson : json["m_keyframes"]) {
        //    m_keyframes.push_back({ keyframeJson["frame"], keyframeJson["value"] });
        //}

        *this = json;

        SortKeyframes();
    }

    Keyframe* AnimationTrack::GetKeyframe(int frameNo) {
        auto keyframeIt = std::find_if(std::begin(m_keyframes), std::end(m_keyframes), [&](auto const& kf) { return kf.m_frame == frameNo; });
        if (std::end(m_keyframes) != keyframeIt && keyframeIt->m_frame == frameNo) {
            // found an existing frame
            return &(*keyframeIt);
        }
        return nullptr;
    }

    Keyframe& AnimationTrack::GetOrCreateKeyframe(int frameNo) {
        // find the frame or the first iterator after where it would be
        auto keyframeIt = std::find_if(std::begin(m_keyframes), std::end(m_keyframes), [&](auto const& kf) { return kf.m_frame >= frameNo; });
        if (std::end(m_keyframes) != keyframeIt && keyframeIt->m_frame == frameNo) {
            // found an existing frame
            return *keyframeIt;
        }

        // didnt find the frame. keyframeIt will be one above the new one
        auto newFrameIt = m_keyframes.insert(keyframeIt, { frameNo, 0.0 });
        return *newFrameIt;
    }

    void AnimationTrack::DeleteKeyframe(int frameNo) {
        auto keyframeIt = std::find_if(std::begin(m_keyframes), std::end(m_keyframes), [&](auto const& kf) { return kf.m_frame == frameNo; });
        if (std::end(m_keyframes) != keyframeIt) {
            m_keyframes.erase(keyframeIt);
        }
    }

    void AnimationTrack::DeleteKeyframe(Keyframe* frame) {
        auto keyframeIt = std::find_if(std::begin(m_keyframes), std::end(m_keyframes), [&](auto const& kf) { return &kf == frame; });
        if (std::end(m_keyframes) != keyframeIt) {
            m_keyframes.erase(keyframeIt);
        }
    }

    void AnimationTrack::UpdateTrackTimings(std::vector<std::unique_ptr<AnimationClip>> const& clips) {
        auto keyframeIt = std::begin(m_keyframes);
        for (auto&& clip : clips) {
            // seek to the start of the clip
            while (keyframeIt != std::end(m_keyframes) && keyframeIt->m_frame < clip->m_startFrame) {
                ++keyframeIt;
            }

            // cycle through all keyframes in clip
            while (keyframeIt != std::end(m_keyframes) && keyframeIt->m_frame <= clip->m_endFrame) {
                int const frameDelta = keyframeIt->m_frame - clip->m_startFrame;
                float const timeDelta = frameDelta / clip->m_fps;
                keyframeIt->m_time = clip->m_startTime + timeDelta;
                ++keyframeIt;
            }
        }
    }

    float AnimationTrack::GetValueAtTime(float time) const {
        float value = 0;

        auto endKeyframeIt = std::end(m_keyframes);
        auto firstKeyframeIt = std::begin(m_keyframes);
        auto secondKeyframeIt = firstKeyframeIt;

        // find the possible two keyframes bounding the current time
        while (secondKeyframeIt != endKeyframeIt && secondKeyframeIt->m_time < time) {
            firstKeyframeIt = secondKeyframeIt;
            ++secondKeyframeIt;
        }

        if (endKeyframeIt == firstKeyframeIt && endKeyframeIt == secondKeyframeIt) {
            // did not find any frames
        } else if (endKeyframeIt != firstKeyframeIt && endKeyframeIt != secondKeyframeIt) {
            // found a start and end keyframe
            float const deltaTime = time - firstKeyframeIt->m_time;
            float const totalTime = secondKeyframeIt->m_time - firstKeyframeIt->m_time;
            float const factor = totalTime == 0 ? 0 : deltaTime / totalTime;
            value = lerp(firstKeyframeIt->m_value, secondKeyframeIt->m_value, factor);
        } else {
            // found one keyframe
            auto validKeyframeIt = firstKeyframeIt != endKeyframeIt ? firstKeyframeIt : secondKeyframeIt;
            value = validKeyframeIt->m_value;
        }

        return value;
    }

    float AnimationTrack::GetValueAtFrame(int frame) const {
        float value = 0;

        const auto endKeyframeIt = std::end(m_keyframes);
        auto firstKeyframeIt = std::begin(m_keyframes);
        auto secondKeyframeIt = firstKeyframeIt;

        // find the possible two keyframes bounding the current frame
        while (secondKeyframeIt != endKeyframeIt && secondKeyframeIt->m_frame < frame) {
            firstKeyframeIt = secondKeyframeIt;
            ++secondKeyframeIt;
        }

        if (endKeyframeIt == firstKeyframeIt && endKeyframeIt == secondKeyframeIt) {
            // did not find any frames
        } else if (endKeyframeIt != firstKeyframeIt && endKeyframeIt != secondKeyframeIt) {
            // found a start and end keyframe
            int const deltaFrames = frame - firstKeyframeIt->m_frame;
            int const totalFrames = secondKeyframeIt->m_frame - firstKeyframeIt->m_frame;
            float const factor = totalFrames == 0 ? 0 : deltaFrames / static_cast<float>(totalFrames);
            value = lerp(firstKeyframeIt->m_value, secondKeyframeIt->m_value, factor);
        } else {
            // found one keyframe
            auto validKeyframeIt = firstKeyframeIt != endKeyframeIt ? firstKeyframeIt : secondKeyframeIt;
            value = validKeyframeIt->m_value;
        }

        return value;
    }

    void AnimationTrack::SortKeyframes() {
        std::sort(std::begin(m_keyframes), std::end(m_keyframes), [](auto const& a, auto const& b) {
            return a.m_frame < b.m_frame;
        });
    }
}
