#include "game_pch.h"
#include "layout_entity_group.h"
#include "layout.h"
#include "ui/group.h"
#include "debug/inspectors.h"
#include "ui/inspectors.h"

namespace ui {
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

    void LayoutEntityGroup::OnEditDraw() {
        if (ImGui::TreeNode("group")) {
            ImGuiInspectMember("id", m_id);
            ImGuiInspectMember("bounds", m_bounds);
            for (auto&& child : m_children) {
                child->OnEditDraw();
            }
            ImGui::TreePop();
        }
    }
}
