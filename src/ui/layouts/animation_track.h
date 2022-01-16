#pragma once

namespace ui {
    class AnimationClipInfo;

    class Keyframe {
    public:
        float m_time;
        float m_value;
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

        AnimationTrack(AnimationClipInfo const& info, nlohmann::json const& json);

        Target GetTarget() const { return m_target; }
        float GetValue(float time) const;

    private:
        Target m_target;
        std::vector<Keyframe> m_keyframes;
    };
}
