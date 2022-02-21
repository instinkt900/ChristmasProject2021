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
        , m_layoutRect(m_layout->GetBoundsAtFrame(0)) {
        m_animationController = std::make_unique<AnimationController>(this, m_layout->GetAnimationTracks());
    }

    Node::~Node() {
    }

    bool Node::OnEvent(Event const& event) {
        return false;
    }

    void Node::Update(uint32_t ticks) {
        m_animationController->Update(ticks / 1000.0f);
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

    void Node::SendEvent(Event const& event) {
        if (m_eventHandler && m_eventHandler(this, event)) {
            return;
        }
        if (m_parent) {
            m_parent->SendEvent(event);
        }
    }

    void Node::SetScreenRect(IntRect const& rect) {
        m_overrideScreenRect = true;
        if (m_screenRect != rect) {
            m_screenRect = rect;
            UpdateChildBounds();
        }
    }

    void Node::RefreshBounds(int frameNo) {
        m_layoutRect = m_layout->GetBoundsAtFrame(frameNo);
        RecalculateBounds();
    }

    void Node::RecalculateBounds() {
        if (nullptr != m_parent && !m_overrideScreenRect) {
            IntRect const& parentBounds = m_parent->GetScreenRect();
            FloatVec2 const parentOffset = static_cast<FloatVec2>(parentBounds.topLeft);
            FloatVec2 const parentDimensions = static_cast<FloatVec2>(parentBounds.bottomRight - parentBounds.topLeft);
            m_screenRect.topLeft = static_cast<IntVec2>(parentOffset + m_layoutRect.offset.topLeft + parentDimensions * m_layoutRect.anchor.topLeft);
            m_screenRect.bottomRight = static_cast<IntVec2>(parentOffset + m_layoutRect.offset.bottomRight + parentDimensions * m_layoutRect.anchor.bottomRight);
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

    void Node::SetAnimationClip(AnimationClip* clip) {
        m_animationController->SetClip(clip);
    }

    void Node::UpdateAnimTime(float delta) {
        m_animationController->Update(delta);
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
