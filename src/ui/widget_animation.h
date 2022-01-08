#pragma once

class Widget;

enum class WidgetAnimationKeyframeType {
    Step,
    Linear,
};

enum class WidgetAnimationLoopType {
    Stop,
    Reset,
    Loop,
};

enum class WidgetAnimationTrackType {
    AnchorLeft,
    AnchorRight,
    AnchorTop,
    AnchorBottom,
    OffsetTop,
    OffsetBottom,
    OffsetLeft,
    OffsetRight,
};

struct WidgetAnimationKeyframe {
    float m_time;
    float m_value;
    WidgetAnimationKeyframeType m_type;
};

struct WidgetAnimationTrackDesc {
    std::vector<WidgetAnimationKeyframe> keyframes;
};

struct WidgetAnimationDesc {
    float startTime;
    float endTime;
};

struct WidgetAnimationTracksDesc {
    std ::map<WidgetAnimationTrackType, std::vector<WidgetAnimationTrackDesc>> tracks;
    std::map<std::string, WidgetAnimationDesc> animations;
};

class WidgetAnimationTrack {
public:
    WidgetAnimationTrack(float& value);

    void SetTime(float time);

private:
    float& m_value;
    std::vector<WidgetAnimationKeyframe> m_keyframes;
};

class WidgetTracks {
public:
    WidgetTracks(WidgetAnimationTracksDesc const& animationDesc, Widget& parentWidget);

    void SetTime(float time) { m_currentTime = time; }
    float GetTime() const { return m_currentTime; }

private:
    float m_currentTime = 0;
    std::vector<WidgetAnimationTrack> tracks;
    float& GetValueRef(Widget& parentWidget, WidgetAnimationTrackType type) const;
};
