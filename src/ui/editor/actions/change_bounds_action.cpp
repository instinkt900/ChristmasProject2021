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
        return m_delta.anchor.topLeft.x == 0
            && m_delta.anchor.topLeft.y == 0
            && m_delta.anchor.bottomRight.x == 0
            && m_delta.anchor.bottomRight.y == 0
            && m_delta.offset.topLeft.x == 0
            && m_delta.offset.topLeft.y == 0
            && m_delta.offset.bottomRight.x == 0
            && m_delta.offset.bottomRight.y == 0;
        // clang-format on
    }

    void ChangeBoundsAction::Do() {
        // layout entity
        auto layoutEntity = m_node->GetLayoutEntity();
        auto& layoutBounds = layoutEntity->GetBounds();
        layoutEntity->SetBounds(layoutBounds + m_delta);

        // node instance
        m_node->GetLayoutRect() += m_delta;
        m_node->RecalculateBounds();
    }

    void ChangeBoundsAction::Undo() {
        // layout entity
        auto layoutEntity = m_node->GetLayoutEntity();
        auto& layoutBounds = layoutEntity->GetBounds();
        layoutEntity->SetBounds(layoutBounds - m_delta);

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
