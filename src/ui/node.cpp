#include "game_pch.h"
#include "node.h"
#include "group.h"
#include "events/event_dispatch.h"
#include "debug/inspectors.h"
#include "inspectors.h"
#include "layouts/layout_entity.h"
#include "layouts/animation_clip.h"
#include "layouts/animation_track.h"
#include "animation_controller.h"

namespace ui {
    Node::Node() {
    }

    Node::Node(std::shared_ptr<LayoutEntity> layoutEntity)
        : m_layout(layoutEntity)
        , m_id(m_layout->GetId())
        , m_layoutRect(m_layout->GetBounds()) {
        m_animationController = std::make_unique<AnimationController>(this, m_layout->GetAnimationTracks());
    }

    Node::~Node() {
    }

    bool Node::OnEvent(Event const& event) {
        return false;
    }

    void Node::Update(uint32_t ticks) {
    }

    void Node::Draw(SDL_Renderer& renderer) {
        if (!IsVisible()) {
            return;
        }

        if (m_showRect) {
            SDL_Rect rect{ m_screenRect.topLeft.x, m_screenRect.topLeft.y, m_screenRect.bottomRight.x - m_screenRect.topLeft.x, m_screenRect.bottomRight.y - m_screenRect.topLeft.y };
            SDL_SetRenderDrawColor(&renderer, 0xFF, 0x00, 0x00, 0xFF);
            SDL_RenderDrawRect(&renderer, &rect);
        }
    }

    void Node::SetScreenRect(IntRect const& rect) {
        m_overrideScreenRect = true;
        m_screenRect = rect;
        UpdateChildBounds();
    }

    void Node::RecalculateBounds() {
        if (nullptr != m_parent && !m_overrideScreenRect) {
            auto const& parentBounds = m_parent->GetScreenRect();
            auto const parentWidth = parentBounds.bottomRight.x - parentBounds.topLeft.x;
            auto const parentHeight = parentBounds.bottomRight.y - parentBounds.topLeft.y;
            m_screenRect.topLeft.x = parentBounds.topLeft.x + static_cast<int>(m_layoutRect.topLeft.offset.x + parentWidth * m_layoutRect.topLeft.anchor.x);
            m_screenRect.topLeft.y = parentBounds.topLeft.y + static_cast<int>(m_layoutRect.topLeft.offset.y + parentHeight * m_layoutRect.topLeft.anchor.y);
            m_screenRect.bottomRight.x = parentBounds.topLeft.x + static_cast<int>(m_layoutRect.bottomRight.offset.x + parentWidth * m_layoutRect.bottomRight.anchor.x);
            m_screenRect.bottomRight.y = parentBounds.topLeft.y + static_cast<int>(m_layoutRect.bottomRight.offset.y + parentHeight * m_layoutRect.bottomRight.anchor.y);
        }
        UpdateChildBounds();
    }

    bool Node::IsInBounds(IntVec2 const& point) const {
        return IsInRect(point, m_screenRect);
    }

    IntVec2 Node::TranslatePosition(IntVec2 const& point) const {
        IntVec2 translated = point;
        translated.x -= m_screenRect.topLeft.x;
        translated.y -= m_screenRect.topLeft.y;
        return translated;
    }

    void Node::SetAnimTime(float time) {
        m_animationController->SetTime(time);
        RecalculateBounds();
    }

    void Node::SetAnimFrame(int frame) {
        m_animationController->SetFrame(frame);
        RecalculateBounds();
    }

    void Node::DebugDraw() {
        if (ImGui::TreeNode("Node")) {
            ImGuiInspectMember("id", m_id);
            ImGuiInspectMember("visible", m_visible);
            ImGuiInspectMember("show rect", m_showRect);
            ImGuiInspectMember("override bounds", m_overrideScreenRect);
            bool boundsChanged = false;
            boundsChanged |= ImGuiInspectMember("bounds", m_layoutRect);
            boundsChanged |= ImGuiInspectMember("screen rect", m_screenRect);
            if (boundsChanged) {
                RecalculateBounds();
            }
            ImGui::TreePop();
        }
    }
}
