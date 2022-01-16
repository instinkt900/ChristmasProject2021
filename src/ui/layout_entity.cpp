#include "game_pch.h"
#include "layout_entity.h"
#include "group.h"
#include "node_image.h"

AnimationClipInfo::AnimationClipInfo(nlohmann::json const& json, float startTime)
    : m_name(json["name"])
    , m_startFrame(json["start_frame"])
    , m_endFrame(json["end_frame"])
    , m_fps(json["fps"])
    , m_loopType(json["loop_type"])
    , m_startTime(startTime)
    , m_endTime(startTime + ((m_endFrame - m_startFrame) / m_fps)) {
}

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

AnimationTrack::AnimationTrack(AnimationClipInfo const& info, nlohmann::json const& json)
    : m_target(json["target"]) {
    float frameTime;
    for (auto&& keyframeJson : json["keyframes"]) {
        keyframeJson.at("time").get_to(frameTime);
        if (frameTime >= info.GetStartTime()) {
            frameTime -= info.GetStartTime();
            m_keyframes.push_back({ frameTime, keyframeJson["value"] });
        } else if (frameTime > info.GetEndTime()) {
            break;
        }
    }
}

float AnimationTrack::GetValue(float time) const {
    Keyframe const* firstKey = nullptr;
    Keyframe const* secondKey = nullptr;
    for (auto&& keyframe : m_keyframes) {
        if (time < keyframe.m_time) {
            secondKey = &keyframe;
            break;
        }
        firstKey = &keyframe;
    }

    if (firstKey && secondKey) {
        return firstKey->m_value + (secondKey->m_value - firstKey->m_value) * (time - firstKey->m_time);
    }

    return m_keyframes[0].m_value;
}

NLOHMANN_JSON_SERIALIZE_ENUM(AnimationClipInfo::LoopType, {
                                                              { AnimationClipInfo::LoopType::Stop, "stop" },
                                                              { AnimationClipInfo::LoopType::Loop, "loop" },
                                                              { AnimationClipInfo::LoopType::Reset, "reset" },
                                                          })

AnimationClip::AnimationClip(AnimationClipInfo const& info, nlohmann::json const& tracksJson)
    : m_name(info.GetName()) {
    for (auto&& trackJson : tracksJson) {
        m_tracks.push_back(std::make_shared<AnimationTrack>(info, trackJson));
    }
}


LayoutEntity::LayoutEntity(nlohmann::json const& json, LayoutEntityGroup* parent)
    : m_id(json["id"])
    , m_bounds(json["bounds"])
    , m_parent(parent) {

    if (json.contains("tracks")) {
        auto const& animationClips = parent->GetAnimationClips();
        auto const& tracksJson = json["tracks"];
        for (auto&& [clipName, clipInfo] : animationClips) {
            m_animationClips.insert(std::make_pair(clipName, std::make_shared<AnimationClip>(*clipInfo, tracksJson)));
        }
    }
}

std::unique_ptr<Node> LayoutEntity::Instantiate() {
    return std::make_unique<Node>(shared_from_this());
}

LayoutEntityGroup::LayoutEntityGroup(nlohmann::json const& json, LayoutEntityGroup* parent)
    : LayoutEntity(json, parent) {
    if (json.contains(LABEL)) {
        auto data = json[LABEL];
        data.at("layout_path").get_to(m_layoutPath);
        auto subLayout = LoadLayout(m_layoutPath);
        Clone(*subLayout);
    }
}

std::unique_ptr<Node> LayoutEntityGroup::Instantiate() {
    return std::make_unique<Group>(std::static_pointer_cast<LayoutEntityGroup>(shared_from_this()));
}

void LayoutEntityGroup::Clone(LayoutEntityGroup const& other) {
    for (auto&& child : other.m_children) {
        m_children.push_back(child);
        child->SetParent(this);
    }

    m_animationClips = other.m_animationClips;
}

LayoutEntityImage::LayoutEntityImage(nlohmann::json const& json, LayoutEntityGroup* parent)
    : LayoutEntity(json, parent) {
    auto data = json[LABEL];
    data.at("texture_path").get_to(m_texturePath);
    data.at("source_rect").get_to(m_sourceRect);
}

std::unique_ptr<Node> LayoutEntityImage::Instantiate() {
    return std::make_unique<NodeImage>(std::static_pointer_cast<LayoutEntityImage>(shared_from_this()));
}

std::unique_ptr<LayoutEntity> LoadEntity(nlohmann::json const& json, LayoutEntityGroup* parent) {
    std::unique_ptr<LayoutEntity> entity;

    if (json.contains(LayoutEntityImage::LABEL)) {
        entity = std::make_unique<LayoutEntityImage>(json, parent);
    } else if (json.contains(LayoutEntityGroup::LABEL)) {
        entity = std::make_unique<LayoutEntityGroup>(json, parent);
    } else {
        assert(false && "unknown entity type");
    }

    return entity;
}

std::shared_ptr<LayoutEntityGroup> LoadLayout(std::string const& layoutPath) {
    std::ifstream ifile(layoutPath);
    if (!ifile.is_open()) {
        return nullptr;
    }

    nlohmann::json json;
    ifile >> json;
    auto layout = std::make_unique<LayoutEntityGroup>(json, nullptr);

    if (json.contains("clips")) {
        auto& animationClips = layout->GetAnimationClips();
        float startTime = 0;
        for (auto&& clipJson : json["clips"]) {
            auto clip = std::make_shared<AnimationClipInfo>(clipJson, startTime);
            startTime = clip->GetEndTime();
            animationClips.insert(std::make_pair(clip->GetName(), clip));
        }
    }

    if (json.contains("children")) {
        for (auto&& childJson : json["children"]) {
            auto child = LoadEntity(childJson, layout.get());
            layout->GetChildren().push_back(std::move(child));
        }
    }

    return layout;
}
