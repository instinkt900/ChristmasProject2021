#pragma once

#include "layout_types.h"

class LayoutEntityGroup;
class Node;

class AnimationClipInfo {
public:
    enum class LoopType {
        Stop,
        Loop,
        Reset,
    };

    AnimationClipInfo(nlohmann::json const& json, float startTime);

    std::string const& GetName() const { return m_name; }
    int GetStartFrame() const { return m_startFrame; }
    int GetEndFrame() const { return m_endFrame; }
    float GetFPS() const { return m_fps; }
    LoopType GetLoopType() const { return m_loopType; }
    float GetStartTime() const { return m_startTime; }
    float GetEndTime() const { return m_endTime; }
    float GetDuration() const { return m_endTime - m_startTime; }

private:
    std::string m_name;
    int m_startFrame;
    int m_endFrame;
    float m_fps;
    float m_startTime;
    float m_endTime;
    LoopType m_loopType;
};

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

class AnimationClip {
public:
    AnimationClip(AnimationClipInfo const& info, nlohmann::json const& tracksJson);

    auto const& GetTracks() const { return m_tracks; }

private:
    std::string m_name;
    std::vector<std::shared_ptr<AnimationTrack>> m_tracks;
};

class LayoutEntity : public std::enable_shared_from_this<LayoutEntity> {
public:
    LayoutEntity(nlohmann::json const& json, LayoutEntityGroup* parent);

    std::string GetId() const { return m_id; }
    LayoutRect const& GetBounds() const { return m_bounds; }

    void SetParent(LayoutEntity* parent) { m_parent = parent; }
    LayoutEntity* GetParent() const { return m_parent; }

    auto& GetAnimationClips() { return m_animationClips; }
    auto const& GetAnimationClips() const { return m_animationClips; }

    virtual std::unique_ptr<Node> Instantiate();

protected:
    std::string m_id;
    LayoutRect m_bounds;
    LayoutEntity* m_parent = nullptr;

    std::map<std::string, std::shared_ptr<AnimationClip>> m_animationClips;
};

class LayoutEntityGroup : public LayoutEntity {
public:
    LayoutEntityGroup(nlohmann::json const& json, LayoutEntityGroup* parent);

    auto& GetChildren() { return m_children; }
    auto const& GetChildren() const { return m_children; }

    auto& GetAnimationClips() { return m_animationClips; }
    auto const& GetAnimationClips() const { return m_animationClips; }

    std::unique_ptr<Node> Instantiate() override;

    static constexpr char const* LABEL = "layout_data";

protected:
    std::string m_layoutPath;
    std::vector<std::shared_ptr<LayoutEntity>> m_children;
    std::map<std::string, std::shared_ptr<AnimationClipInfo>> m_animationClips;

    void Clone(LayoutEntityGroup const& other);
};

class LayoutEntityImage : public LayoutEntity {
public:
    LayoutEntityImage(nlohmann::json const& json, LayoutEntityGroup* parent);

    std::string const& GetTexturePath() const { return m_texturePath; }
    IntRect const& GetSourceRect() const { return m_sourceRect; }

    std::unique_ptr<Node> Instantiate() override;

    static constexpr char const* LABEL = "image_data";

protected:
    std::string m_texturePath;
    IntRect m_sourceRect;
};

std::shared_ptr<LayoutEntityGroup> LoadLayout(std::string const& layoutPath);
