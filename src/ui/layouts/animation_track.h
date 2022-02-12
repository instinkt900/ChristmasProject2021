#pragma once

namespace ui {
    struct AnimationClip;

    class Keyframe {
    public:
        int m_frame;
        float m_value;

        float m_time; // calculated based on clips

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Keyframe, m_frame, m_value);
    };

    class AnimationTrack {
    public:
        enum class Target {
            Unknown,
            TopOffset,
            BottomOffset,
            LeftOffset,
            RightOffset,
            TopAnchor,
            BottomAnchor,
            LeftAnchor,
            RightAnchor
        };

        AnimationTrack() = default;
        AnimationTrack(Target target, float initialValue);
        explicit AnimationTrack(nlohmann::json const& json);

        Target GetTarget() const { return m_target; }

        Keyframe* GetKeyframe(int frameNo);
        Keyframe& GetOrCreateKeyframe(int frameNo);
        void DeleteKeyframe(int frameNo);
        void DeleteKeyframe(Keyframe* frame);

        void UpdateTrackTimings(std::vector<std::unique_ptr<AnimationClip>> const& clips);
        float GetValueAtTime(float time) const;
        float GetValueAtFrame(int frame) const;

        auto& GetKeyframes() { return m_keyframes; }

        void SortKeyframes();

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(AnimationTrack, m_target, m_keyframes);

    private:
        Target m_target;
        std::vector<Keyframe> m_keyframes;
    };
}
