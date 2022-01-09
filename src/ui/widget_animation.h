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
    std::string name;
    float startTime;
    float endTime;
    WidgetAnimationLoopType loopType;
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

class WidgetTracks;

class WidgetAnimation {
public:
    WidgetAnimation(WidgetTracks& tracks, WidgetAnimationDesc const& desc);

    void Update(float deltaTime);

private:
    WidgetTracks& m_tracks;
    std::string m_name;
    float m_startTime = 0;
    float m_endTime = 0;
    float m_currentTime = 0;
    WidgetAnimationLoopType m_loopType = WidgetAnimationLoopType::Stop;
};

class WidgetTracks {
public:
    WidgetTracks(WidgetAnimationTracksDesc const& tracksDesc, std::vector<WidgetAnimationDesc> animationList, Widget & parentWidget);

    void Update(float deltaTime);

    void SetTime(float time);
    float GetTime() const { return m_currentTime; }

    bool HasAnimation(std::string const& name);
    bool SetAnimation(std::string const& name);
    void StopAnimation();

private:
    float m_currentTime = 0;
    std::vector<WidgetAnimationTrack> m_tracks;
    std::map<std::string, WidgetAnimation> m_animations;
    WidgetAnimation* m_currentAnimation = nullptr;

    float& GetValueRef(Widget& parentWidget, WidgetAnimationTrackType type) const;
};
