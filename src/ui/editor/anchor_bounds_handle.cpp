#include "game_pch.h"
#include "anchor_bounds_handle.h"
#include "ui/node.h"
#include "ui/group.h"
#include "ui/layouts/layout_entity.h"

namespace ui {
    AnchorBoundsHandle::AnchorBoundsHandle(BoundsHandleAnchor const& anchor)
        : BoundsHandle(anchor) {
    }

    AnchorBoundsHandle::~AnchorBoundsHandle() {
    }

    void AnchorBoundsHandle::Draw(SDL_Renderer& renderer) {
        if (!m_target) {
            return;
        }

        auto const& parentScreenRect = m_target->GetParent()->GetScreenRect();
        auto const canvasWidth = parentScreenRect.bottomRight.x - parentScreenRect.topLeft.x;
        auto const canvasHeight = parentScreenRect.bottomRight.y - parentScreenRect.topLeft.y;

        auto const& layoutRect = m_target->GetLayoutRect();

        // clang-format off
        float const xAnchor = m_anchor.Left ? 0.0f : m_anchor.Right ? 1.0f : 0.5f;
        float const yAnchor = m_anchor.Top ? 0.0f : m_anchor.Bottom ? 1.0f : 0.5f;
        // clang-format on

        auto const x = lerp(layoutRect.topLeft.anchor.x, layoutRect.bottomRight.anchor.x, xAnchor);
        auto const y = lerp(layoutRect.topLeft.anchor.y, layoutRect.bottomRight.anchor.y, yAnchor);

        m_position.x = static_cast<int>(parentScreenRect.topLeft.x + canvasWidth * x);
        m_position.y = static_cast<int>(parentScreenRect.topLeft.y + canvasHeight * y);

        int const halfSize = m_size / 2;

        int x1 = 0;
        int x2 = 0;
        int y1 = 0;
        int y2 = 0;
        int const offset = halfSize + 3;

        if (!m_anchor.Top || !m_anchor.Bottom) {
            x1 = m_position.x - offset;
            x2 = m_position.x + offset;
            y1 = m_position.y - offset + static_cast<int>(offset * 2 * yAnchor);
            SDL_RenderDrawLine(&renderer, x1, y1, x2, y1);
        }

        if (!m_anchor.Left || !m_anchor.Right) {
            y1 = m_position.y - offset;
            y2 = m_position.y + offset;
            x1 = m_position.x - offset + static_cast<int>(offset * 2 * xAnchor);
            SDL_RenderDrawLine(&renderer, x1, y1, x1, y2);
        }

        if (m_holding) {
            y1 = m_position.y - offset;
            y2 = m_position.y + offset;
            x1 = m_position.x - offset + static_cast<int>(offset * 2 * xAnchor);
            SDL_RenderDrawLine(&renderer, m_position.x - 3, m_position.y, m_position.x + 3, m_position.y);
            SDL_RenderDrawLine(&renderer, m_position.x, m_position.y - 3, m_position.x, m_position.y + 3);
        }
    }

    bool AnchorBoundsHandle::IsInBounds(IntVec2 const& pos) const {
        // clang-format off
        float const xAnchor = m_anchor.Left ? 0.0f : m_anchor.Right ? 1.0f : 0.5f;
        float const yAnchor = m_anchor.Top ? 0.0f : m_anchor.Bottom ? 1.0f : 0.5f;
        // clang-format on

        int const halfSize = m_size / 2;

        int x1 = 0;
        int x2 = 0;
        int y1 = 0;
        int y2 = 0;
        int const offset = halfSize + 3;

        bool inside = false;

        if (!m_anchor.Top || !m_anchor.Bottom) {
            IntRect r1;
            r1.topLeft.x = m_position.x - offset;
            r1.bottomRight.x = r1.topLeft.x + offset * 2;
            r1.topLeft.y = m_position.y - offset + static_cast<int>(offset * 2 * yAnchor) - 3;
            r1.bottomRight.y = r1.topLeft.y + 6;
            if (IsInRect(pos, r1)) {
                return true;
            }
        }

        if (!m_anchor.Left || !m_anchor.Right) {
            IntRect r1;
            r1.topLeft.y = m_position.y - offset;
            r1.bottomRight.y = r1.topLeft.y + offset * 2;
            r1.topLeft.x = m_position.x - offset + static_cast<int>(offset * 2 * xAnchor) - 3;
            r1.bottomRight.x = r1.topLeft.x + 6;
            if (IsInRect(pos, r1)) {
                return true;
            }
        }

        return false;
    }

    bool AnchorBoundsHandle::OnMouseMove(EventMouseMove const& event) {
        if (m_holding) {
            auto const& parentScreenRect = m_target->GetParent()->GetScreenRect();
            auto const canvasWidth = parentScreenRect.bottomRight.x - parentScreenRect.topLeft.x;
            auto const canvasHeight = parentScreenRect.bottomRight.y - parentScreenRect.topLeft.y;

            auto const xFact = event.GetDelta().x / static_cast<float>(canvasWidth);
            auto const yFact = event.GetDelta().y / static_cast<float>(canvasHeight);

            auto& bounds = m_target->GetLayoutEntity()->GetBounds();
            bounds.topLeft.anchor.x += xFact * m_anchor.Left;
            bounds.bottomRight.anchor.x += xFact * m_anchor.Right;
            bounds.topLeft.anchor.y += yFact * m_anchor.Top;
            bounds.bottomRight.anchor.y += yFact * m_anchor.Bottom;
            m_target->RefreshBounds();
            //m_target->RecalculateBounds();
        }
        return false;
    }
}
