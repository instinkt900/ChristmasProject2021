#pragma once

class Widget;

enum class WidgetAnimationKeyframeType {
    Step,
    Linear,
};

struct WidgetAnimationKeyframe {
    float m_time;
    float m_value;
    WidgetAnimationKeyframeType m_type;
};

class WidgetAnimationTrack {
public:
    WidgetAnimationTrack(float& value);

    void SetTime(float time);

private:
    float& m_value;
    std::vector<WidgetAnimationKeyframe> m_keyframes;
};

enum class WidgetAnimationLoopType {
    Stop,
    Reset,
    Loop,
};

class WidgetAnimation {
public:
    WidgetAnimation(Widget& parentWidget);

    void Update(float deltaTime);

private:
    bool m_playing = false;
    float m_currentTime = 0;
    float m_maxTime = 0;
    WidgetAnimationLoopType m_loopType = WidgetAnimationLoopType::Stop;
    std::vector<WidgetAnimationTrack> m_tracks;

    void OnEnd();
};
