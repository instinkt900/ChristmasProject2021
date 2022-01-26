#include "game_pch.h"
#include "node.h"
#include "group.h"
#include "events/event_dispatch.h"
#include "debug/inspectors.h"
#include "inspectors.h"
#include "layouts/layout_entity.h"
#include "layouts/animation_clip.h"
#include "layouts/animation_track.h"

namespace ui {
    Node::Node() {
    }

    Node::Node(std::shared_ptr<LayoutEntity> layoutEntity)
        : m_layout(layoutEntity)
        , m_id(m_layout->GetId())
        , m_layoutRect(m_layout->GetBounds()) {
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

    void Node::ActivateClip(std::string const& name) {
        if (m_layout) {
            auto& animationClips = m_layout->GetAnimationClips();
            auto it = animationClips.find(name);
            if (std::end(animationClips) != it) {
                m_currentAnimationClip = it->second.get();
            }
        }
    }

    void Node::DeactivateClip() {
        m_currentAnimationClip = nullptr;
    }

    void Node::SetAnimTime(float time) {
        if (m_currentAnimationClip) {
            for (auto&& track : m_currentAnimationClip->GetTracks()) {
                switch (track->GetTarget()) {
                case AnimationTrack::Target::TopOffset:
                    m_layoutRect.topLeft.offset.y = track->GetValue(time);
                    break;
                case AnimationTrack::Target::BottomOffset:
                    m_layoutRect.bottomRight.offset.y = track->GetValue(time);
                    break;
                case AnimationTrack::Target::LeftOffset:
                    m_layoutRect.topLeft.offset.x = track->GetValue(time);
                    break;
                case AnimationTrack::Target::RightOffset:
                    m_layoutRect.bottomRight.offset.x = track->GetValue(time);
                    break;
                }
            }
            RecalculateBounds();
        }
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
