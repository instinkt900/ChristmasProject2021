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
    float time;
    float value;
    WidgetAnimationKeyframeType type;
};

inline void to_json(nlohmann::json& j, WidgetAnimationKeyframe const& keyframe) {
    j = nlohmann::json{
        { "time", keyframe.time },
        { "value", keyframe.value },
        { "type", keyframe.type }
    };
}

inline void from_json(nlohmann::json const& j, WidgetAnimationKeyframe& keyframe) {
    j.at("time").get_to(keyframe.time);
    j.at("value").get_to(keyframe.value);
    j.at("type").get_to(keyframe.type);
}

struct WidgetAnimationTrackDesc {
    std::vector<WidgetAnimationKeyframe> keyframes;
};

inline void to_json(nlohmann::json& j, WidgetAnimationTrackDesc const& track) {
    j = nlohmann::json{
        { "keyframes", track.keyframes }
    };
}

inline void from_json(nlohmann::json const& j, WidgetAnimationTrackDesc& track) {
    j.at("keyframes").get_to(track.keyframes);
}

struct WidgetAnimationDesc {
    std::string name;
    float startTime;
    float endTime;
    WidgetAnimationLoopType loopType;
};

inline void to_json(nlohmann::json& j, WidgetAnimationDesc const& animation) {
    j = nlohmann::json{
        { "name", animation.name },
        { "startTime", animation.startTime },
        { "endTime", animation.endTime },
        { "loopType", animation.loopType }
    };
}

inline void from_json(nlohmann::json const& j, WidgetAnimationDesc& animation) {
    j.at("name").get_to(animation.name);
    j.at("startTime").get_to(animation.startTime);
    j.at("endTime").get_to(animation.endTime);
    j.at("loopType").get_to(animation.loopType);
}

struct WidgetAnimationTracksDesc {
    std ::map<WidgetAnimationTrackType, std::vector<WidgetAnimationTrackDesc>> tracks;
    std::map<std::string, WidgetAnimationDesc> animations;
};

inline void to_json(nlohmann::json& j, WidgetAnimationTracksDesc const& tracks) {
    j = nlohmann::json{
        { "tracks", tracks.tracks },
        { "animations", tracks.animations }
    };
}

inline void from_json(nlohmann::json const& j, WidgetAnimationTracksDesc& tracks) {
    j.at("tracks").get_to(tracks.tracks);
    j.at("animations").get_to(tracks.animations);
}

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
    WidgetTracks(WidgetAnimationTracksDesc const& tracksDesc, Widget& parentWidget);

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
