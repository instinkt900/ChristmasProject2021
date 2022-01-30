#include "game_pch.h"
#include "bounds_widget.h"
#include "anchor_bounds_handle.h"
#include "offset_bounds_handle.h"
#include "events/event_dispatch.h"
#include "ui/node.h"
#include "ui/layouts/layout_entity.h"

namespace ui {
    BoundsWidget::BoundsWidget() {
        // anchor manipulators
        m_handles[0] = std::make_unique<AnchorBoundsHandle>(BoundsHandle::TopLeft);
        m_handles[1] = std::make_unique<AnchorBoundsHandle>(BoundsHandle::TopRight);
        m_handles[2] = std::make_unique<AnchorBoundsHandle>(BoundsHandle::BottomLeft);
        m_handles[3] = std::make_unique<AnchorBoundsHandle>(BoundsHandle::BottomRight);
        m_handles[4] = std::make_unique<AnchorBoundsHandle>(BoundsHandle::Top);
        m_handles[5] = std::make_unique<AnchorBoundsHandle>(BoundsHandle::Left);
        m_handles[6] = std::make_unique<AnchorBoundsHandle>(BoundsHandle::Right);
        m_handles[7] = std::make_unique<AnchorBoundsHandle>(BoundsHandle::Bottom);

        // offset manipulators
        m_handles[8] = std::make_unique<OffsetBoundsHandle>(BoundsHandle::TopLeft);
        m_handles[9] = std::make_unique<OffsetBoundsHandle>(BoundsHandle::TopRight);
        m_handles[10] = std::make_unique<OffsetBoundsHandle>(BoundsHandle::BottomLeft);
        m_handles[11] = std::make_unique<OffsetBoundsHandle>(BoundsHandle::BottomRight);
        m_handles[12] = std::make_unique<OffsetBoundsHandle>(BoundsHandle::Top);
        m_handles[13] = std::make_unique<OffsetBoundsHandle>(BoundsHandle::Left);
        m_handles[14] = std::make_unique<OffsetBoundsHandle>(BoundsHandle::Right);
        m_handles[15] = std::make_unique<OffsetBoundsHandle>(BoundsHandle::Bottom);
    }

    BoundsWidget::~BoundsWidget() {
    }

    bool BoundsWidget::OnEvent(Event const& event) {
        EventDispatch dispatch(event);
        for (auto&& handle : m_handles) {
            dispatch.Dispatch(handle.get());
        }
        dispatch.Dispatch(this, &BoundsWidget::OnMouseDown);
        dispatch.Dispatch(this, &BoundsWidget::OnMouseUp);
        dispatch.Dispatch(this, &BoundsWidget::OnMouseMove);
        return dispatch.GetHandled();
    }

    void BoundsWidget::SetSelection(Node* selection) {
        m_holding = false;
        m_selectedNode = selection;

        for (auto&& handle : m_handles) {
            handle->SetTarget(m_selectedNode);
        }
    }

    void BoundsWidget::Draw(SDL_Renderer& renderer) {
        if (m_selectedNode) {
            SDL_SetRenderDrawColor(&renderer, 0x00, 0x00, 0xFF, 0xFF);
            auto const& screenRect = m_selectedNode->GetScreenRect();
            auto const width = screenRect.bottomRight.x - screenRect.topLeft.x;
            auto const height = screenRect.bottomRight.y - screenRect.topLeft.y;
            SDL_Rect const rect{ screenRect.topLeft.x, screenRect.topLeft.y, width, height };
            SDL_RenderDrawRect(&renderer, &rect);
            for (auto& handle : m_handles) {
                handle->Draw(renderer);
            }
        }
    }

    bool BoundsWidget::OnMouseDown(EventMouseDown const& event) {
        if (!m_selectedNode || event.GetButton() != MouseButton::Left) {
            return false;
        }

        if (IsInRect(event.GetPosition(), m_selectedNode->GetScreenRect())) {
            m_holding = true;
            return true;
        }

        return false;
    }

    bool BoundsWidget::OnMouseUp(EventMouseUp const& event) {
        if (event.GetButton() != MouseButton::Left) {
            return false;
        }
        m_holding = false;
        return false;
    }

    bool BoundsWidget::OnMouseMove(EventMouseMove const& event) {
        if (m_holding) {
            auto& bounds = m_selectedNode->GetLayoutEntity()->GetBounds();
            bounds.topLeft.offset.x += event.GetDelta().x;
            bounds.bottomRight.offset.x += event.GetDelta().x;
            bounds.topLeft.offset.y += event.GetDelta().y;
            bounds.bottomRight.offset.y += event.GetDelta().y;
            m_selectedNode->RefreshBounds();
            //m_selectedNode->RecalculateBounds();
        }
        return false;
    }
}
