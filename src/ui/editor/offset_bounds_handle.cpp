#include "game_pch.h"
#include "offset_bounds_handle.h"
#include "ui/node.h"
#include "ui/layouts/layout_entity.h"

namespace ui {
    OffsetBoundsHandle::OffsetBoundsHandle(BoundsWidget& widget, BoundsHandleAnchor const& anchor)
        : BoundsHandle(widget, anchor) {
    }

    OffsetBoundsHandle::~OffsetBoundsHandle() {
    }

    void OffsetBoundsHandle::Draw(SDL_Renderer& renderer) {
        if (!m_target) {
            return;
        }

        auto const& bounds = m_target->GetScreenRect();
        auto const width = bounds.bottomRight.x - bounds.topLeft.x;
        auto const height = bounds.bottomRight.y - bounds.topLeft.y;

        // clang-format off
        float const xAnchor = m_anchor.Left ? 0.0f : m_anchor.Right ? 1.0f : 0.5f;
        float const yAnchor = m_anchor.Top ? 0.0f : m_anchor.Bottom ? 1.0f : 0.5f;
        // clang-format on

        m_position.x = static_cast<int>(bounds.topLeft.x + width * xAnchor);
        m_position.y = static_cast<int>(bounds.topLeft.y + height * yAnchor);

        int const halfSize = m_size / 2;
        SDL_Rect const r{ m_position.x - halfSize, m_position.y - halfSize, m_size, m_size };
        SDL_RenderFillRect(&renderer, &r);
    }

    bool OffsetBoundsHandle::IsInBounds(IntVec2 const& pos) const {
        int const halfSize = m_size / 2;
        IntRect r;
        r.topLeft.x = m_position.x - halfSize;
        r.topLeft.y = m_position.y - halfSize;
        r.bottomRight.x = m_position.x + halfSize;
        r.bottomRight.y = m_position.y + halfSize;
        return IsInRect(pos, r);
    }

    bool OffsetBoundsHandle::OnMouseMove(EventMouseMove const& event) {
        if (nullptr == m_target) {
            return false;
        }

        if (m_holding) {
            auto& bounds = m_target->GetLayoutRect();
            bounds.offset.topLeft.x += event.GetDelta().x * m_anchor.Left;
            bounds.offset.bottomRight.x += event.GetDelta().x * m_anchor.Right;
            bounds.offset.topLeft.y += event.GetDelta().y * m_anchor.Top;
            bounds.offset.bottomRight.y += event.GetDelta().y * m_anchor.Bottom;
            //m_target->GetLayoutEntity()->SetBounds(bounds);
            //m_target->RefreshBounds();
            m_target->RecalculateBounds();
        }
        return false;
    }
}
