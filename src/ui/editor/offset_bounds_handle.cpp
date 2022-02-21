#include "game_pch.h"
#include "offset_bounds_handle.h"
#include "ui/node.h"
#include "ui/layouts/layout_entity.h"
#include "bounds_widget.h"
#include "editor_layer.h"

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

        auto const scaleFactor = m_widget.GetEditorLayer().GetScaleFactor();

        auto const& bounds = m_target->GetScreenRect();

        FloatVec2 const dim = static_cast<FloatVec2>(bounds.bottomRight - bounds.topLeft);
        FloatVec2 const anchor = { m_anchor.Left ? 0.0f : (m_anchor.Right ? 1.0f : 0.5f), m_anchor.Top ? 0.0f : (m_anchor.Bottom ? 1.0f : 0.5f) };
        m_position = static_cast<FloatVec2>(bounds.topLeft) + dim * anchor;

        FloatVec2 const handleSize = FloatVec2{ m_size, m_size };
        FloatVec2 const halfHandleSize = handleSize / 2.0f;

        FloatVec2 const offset = (m_position / scaleFactor) - halfHandleSize;

        SDL_FRect const r{ offset.x, offset.y, handleSize.x, handleSize.y };
        SDL_RenderFillRectF(&renderer, &r);
    }

    bool OffsetBoundsHandle::IsInBounds(IntVec2 const& pos) const {
        auto const scaleFactor = m_widget.GetEditorLayer().GetScaleFactor();
        int const halfSize = static_cast<int>(m_size / 2 * scaleFactor);
        IntRect r;
        r.topLeft = static_cast<IntVec2>(m_position - halfSize);
        r.bottomRight = static_cast<IntVec2>(m_position + halfSize);
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
            m_target->RecalculateBounds();
        }
        return false;
    }
}
