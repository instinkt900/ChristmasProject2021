#include "game_pch.h"
#include "animation_editor.h"
#include "ui/layouts/layout_entity_group.h"
#include "ui/group.h"
#include "ui/layouts/animation_clip.h"
#include "ui/layouts/animation_track.h"

namespace {
    using namespace ui;

    char const* ToString(AnimationTrack::Target target) {
        switch (target) {
        case AnimationTrack::Target::TopOffset:
            return "TopOffset";
        case AnimationTrack::Target::BottomOffset:
            return "BottomOffset";
        case AnimationTrack::Target::LeftOffset:
            return "LeftOffset";
        case AnimationTrack::Target::RightOffset:
            return "RightOffset";
        default:
            return "Unknown";
        }
    }
}

namespace ui {
    AnimationEditContext::AnimationEditContext(Group* group)
        : m_group(group) {
        Update();
    }

    int AnimationEditContext::GetClipCount() const {
        if (auto layout = static_cast<LayoutEntityGroup*>(m_group->GetLayoutEntity().get())) {
            return static_cast<int>(layout->GetAnimationClips().size());
        }
        return 0;
    }

    void AnimationEditContext::GetClip(int clipIndex, int** startFrame, int** endFrame, char const** clipName, unsigned int* color) {
        if (auto layout = static_cast<LayoutEntityGroup*>(m_group->GetLayoutEntity().get())) {
            auto& clips = layout->GetAnimationClips();

            if (startFrame)
                *startFrame = &clips[clipIndex]->m_startFrame;
            if (endFrame)
                *endFrame = &clips[clipIndex]->m_endFrame;
            if (clipName)
                *clipName = clips[clipIndex]->m_name.c_str();
            if (color)
                *color = 0xFF00CCAA;
        }
    }

    int AnimationEditContext::GetTrackCount() const {
        return static_cast<int>(m_tracks.size());
    }

    char const* AnimationEditContext::GetTrackLabel(int index) const {
        auto child = m_group->GetChildren()[index];
        if (child->GetId() != "") {
            return child->GetId().c_str();
        }
        static std::string tmp;
        tmp = fmt::format("Child {}", index);
        return tmp.c_str();
    }

    void AnimationEditContext::GetKeyframes(int trackIndex, int** keyframes, int* keyframeCount) {
        if (keyframes)
            *keyframes = m_tracks[trackIndex].m_keys.data();
        if (keyframeCount)
            *keyframeCount = static_cast<int>(m_tracks[trackIndex].m_keys.size());
    }

    void AnimationEditContext::Update() {
        m_tracks.clear();
        if (auto layout = static_cast<LayoutEntityGroup*>(m_group->GetLayoutEntity().get())) {
            auto& children = m_group->GetChildren();
            int i = 0;
            for (auto&& child : children) {
                if (auto childLayout = child->GetLayoutEntity()) {
                    m_tracks.push_back({});
                    auto& trackGroup = m_tracks.back();
                    trackGroup.m_name = fmt::format("Child[{}]: {}", i, child->GetId());
                    auto& tracks = childLayout->GetAnimationTracks();
                    std::set<int> allKeyFrames;
                    for (auto&& track : tracks) {
                        trackGroup.m_subTracks.push_back({});
                        auto& subTrack = trackGroup.m_subTracks.back();
                        subTrack.m_name = fmt::format("{}", ToString(track->m_target));
                        for (auto&& keyframe : track->m_keyframes) {
                            subTrack.m_keys.push_back(keyframe.m_frame);
                            allKeyFrames.insert(keyframe.m_frame);
                        }
                    }
                    trackGroup.m_keys.insert(std::end(trackGroup.m_keys), std::begin(allKeyFrames), std::end(allKeyFrames));
                }
                ++i;
            }
        }
    }

    void AnimationEditContext::Draw() {
        if (ImGui::Begin("Animation")) {
            int lastFrame = m_currentFrame;
            ImAnimation::Animation(this, &m_currentFrame, &m_expanded, m_minFrame, m_maxFrame, &m_firstFrame);
            if (lastFrame != m_currentFrame) {
                for (auto&& child : m_group->GetChildren()) {
                    child->SetAnimFrame(m_currentFrame, false);
                }
            }
        }
        ImGui::End();
    }
}
