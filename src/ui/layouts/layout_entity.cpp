#include "game_pch.h"
#include "layout_entity.h"
#include "layout_entity_group.h"
#include "ui/node.h"
#include "animation_clip.h"

namespace ui {
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
}
