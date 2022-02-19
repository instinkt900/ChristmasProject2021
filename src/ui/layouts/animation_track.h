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

    class AnimationEvent {
    public:
        int m_frame;
        std::string m_name;

        float m_time; // calculated based on clips

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(AnimationEvent, m_frame, m_name);
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

        AnimationEvent* GetEvent(int frameNo);
        AnimationEvent& GetOrCreateEvent(int frameNo);
        void DeleteEvent(int frameNo);
        void DeleteEvent(AnimationEvent* event);

        void ForEventsOverTime(float startTime, float endTime, std::function<void(AnimationTrack::Target, std::string const&)> const& callback);

        void UpdateTrackTimings(std::vector<std::unique_ptr<AnimationClip>> const& clips);
        float GetValueAtTime(float time) const;
        float GetValueAtFrame(int frame) const;

        auto& GetKeyframes() { return m_keyframes; }
        auto& GetEvents() { return m_events; }

        void SortKeyframes();

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(AnimationTrack, m_target, m_keyframes, m_events);

    private:
        Target m_target;
        std::vector<Keyframe> m_keyframes;
        std::vector<AnimationEvent> m_events;
    };
}
