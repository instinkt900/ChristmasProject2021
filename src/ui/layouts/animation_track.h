#pragma once

namespace ui {
    struct AnimationClip;

    class Keyframe {
    public:
        int m_frame;
        float m_value;

        float m_time; // calculated based on clips
    };

    class AnimationTrack {
    public:
        enum class Target {
            TopOffset,
            BottomOffset,
            LeftOffset,
            RightOffset,
            TopAnchor,
            BottomAnchor,
            LeftAnchor,
            RightAnchor
        };

        AnimationTrack(nlohmann::json const& json);

        void UpdateTrackTimings(std::vector<std::unique_ptr<AnimationClip>> const& clips);

        Target m_target;
        std::vector<Keyframe> m_keyframes;
    };
}
