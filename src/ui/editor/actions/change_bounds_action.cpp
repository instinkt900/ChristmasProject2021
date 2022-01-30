#include "game_pch.h"
#include "change_bounds_action.h"
#include "ui/node.h"
#include "ui/layouts/layout_entity.h"
#include "ui/inspectors.h"

namespace ui {
    ChangeBoundsAction::ChangeBoundsAction(std::shared_ptr<Node> node)
        : m_node(node)
        , m_delta(m_node->GetLayoutRect()) {
    }

    ChangeBoundsAction::~ChangeBoundsAction() {
    }

    void ChangeBoundsAction::EndChange() {
        auto const& newBounds = m_node->GetLayoutRect();
        m_delta = newBounds - m_delta;
    }

    bool ChangeBoundsAction::IsZero() const {
        // clang-format off
        return m_delta.topLeft.anchor.x == 0
            && m_delta.topLeft.anchor.y == 0
            && m_delta.bottomRight.anchor.x == 0
            && m_delta.bottomRight.anchor.y == 0
            && m_delta.topLeft.offset.x == 0
            && m_delta.topLeft.offset.y == 0
            && m_delta.bottomRight.offset.x == 0
            && m_delta.bottomRight.offset.y == 0;
        // clang-format on
    }

    void ChangeBoundsAction::Do() {
        // layout entity
        auto layoutEntity = m_node->GetLayoutEntity();
        auto& layoutBounds = layoutEntity->m_bounds;
        layoutBounds += m_delta;

        // node instance
        m_node->GetLayoutRect() += m_delta;
        m_node->RecalculateBounds();
    }

    void ChangeBoundsAction::Undo() {
        // layout entity
        auto layoutEntity = m_node->GetLayoutEntity();
        auto& layoutBounds = layoutEntity->m_bounds;
        layoutBounds -= m_delta;

        // node instance
        m_node->GetLayoutRect() -= m_delta;
        m_node->RecalculateBounds();
    }

    void ChangeBoundsAction::OnImGui() {
        if (ImGui::CollapsingHeader("ChangeBoundsAction")) {
            ImGui::LabelText("Node", "%p", m_node.get());
            ImGuiInspectMember("Delta", m_delta);
        }
    }
}
