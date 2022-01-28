#include "game_pch.h"
#include "layout_entity.h"
#include "layout_entity_group.h"
#include "ui/node.h"
#include "animation_track.h"
#include "debug/inspectors.h"
#include "ui/inspectors.h"

namespace ui {
    LayoutEntity::LayoutEntity(nlohmann::json const& json, LayoutEntityGroup* parent)
        : m_id(json["id"])
        , m_bounds(json["bounds"])
        , m_parent(parent) {

        if (json.contains("tracks")) {
            auto const& animationClips = parent->GetAnimationClips();
            auto const& tracksJson = json["tracks"];
            for (auto&& trackJson : tracksJson) {
                auto track = std::make_unique<AnimationTrack>(trackJson);
                track->UpdateTrackTimings(animationClips);
                m_tracks.push_back(std::move(track));

            }
        }
    }

    std::unique_ptr<Node> LayoutEntity::Instantiate() {
        return std::make_unique<Node>(shared_from_this());
    }

    void LayoutEntity::OnEditDraw() {
        if (ImGui::TreeNode("node")) {
            ImGuiInspectMember("id", m_id);
            ImGuiInspectMember("bounds", m_bounds);
            ImGui::TreePop();
        }
    }
}
