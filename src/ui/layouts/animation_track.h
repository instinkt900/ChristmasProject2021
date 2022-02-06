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

        AnimationTrack(Target target, float initialValue);
        AnimationTrack(nlohmann::json const& json);

        Target GetTarget() const { return m_target; }

        Keyframe* GetKeyframe(int frameNo);
        Keyframe& GetOrCreateKeyframe(int frameNo);
        void DeleteKeyframe(int frameNo);

        void UpdateTrackTimings(std::vector<std::unique_ptr<AnimationClip>> const& clips);
        float GetValueAtTime(float time) const;
        float GetValueAtFrame(int frame) const;

        auto& GetKeyframes() { return m_keyframes; }

        void SortKeyframes();
    private:
        Target m_target;
        std::vector<Keyframe> m_keyframes;
    };

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
}
