#include "game_pch.h"
#include "bounds_widget.h"
#include "anchor_bounds_handle.h"
#include "offset_bounds_handle.h"
#include "events/event_dispatch.h"
#include "ui/node.h"
#include "ui/layouts/layout_entity.h"
#include "ui/editor/editor_layer.h"

namespace ui {
    BoundsWidget::BoundsWidget(EditorLayer& editorLayer)
        : m_editorLayer(editorLayer) {
        // anchor manipulators
        m_handles[0] = std::make_unique<AnchorBoundsHandle>(*this, BoundsHandle::TopLeft);
        m_handles[1] = std::make_unique<AnchorBoundsHandle>(*this, BoundsHandle::TopRight);
        m_handles[2] = std::make_unique<AnchorBoundsHandle>(*this, BoundsHandle::BottomLeft);
        m_handles[3] = std::make_unique<AnchorBoundsHandle>(*this, BoundsHandle::BottomRight);
        m_handles[4] = std::make_unique<AnchorBoundsHandle>(*this, BoundsHandle::Top);
        m_handles[5] = std::make_unique<AnchorBoundsHandle>(*this, BoundsHandle::Left);
        m_handles[6] = std::make_unique<AnchorBoundsHandle>(*this, BoundsHandle::Right);
        m_handles[7] = std::make_unique<AnchorBoundsHandle>(*this, BoundsHandle::Bottom);

        // offset manipulators
        m_handles[8] = std::make_unique<OffsetBoundsHandle>(*this, BoundsHandle::TopLeft);
        m_handles[9] = std::make_unique<OffsetBoundsHandle>(*this, BoundsHandle::TopRight);
        m_handles[10] = std::make_unique<OffsetBoundsHandle>(*this, BoundsHandle::BottomLeft);
        m_handles[11] = std::make_unique<OffsetBoundsHandle>(*this, BoundsHandle::BottomRight);
        m_handles[12] = std::make_unique<OffsetBoundsHandle>(*this, BoundsHandle::Top);
        m_handles[13] = std::make_unique<OffsetBoundsHandle>(*this, BoundsHandle::Left);
        m_handles[14] = std::make_unique<OffsetBoundsHandle>(*this, BoundsHandle::Right);
        m_handles[15] = std::make_unique<OffsetBoundsHandle>(*this, BoundsHandle::Bottom);
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

    void BoundsWidget::BeginEdit() {
        m_editorLayer.BeginEditBounds();
    }

    void BoundsWidget::EndEdit() {
        m_editorLayer.EndEditBounds();
    }

    void BoundsWidget::Draw(SDL_Renderer& renderer) {
        auto const selection = m_editorLayer.GetSelection();
        if (selection && selection->GetParent()) {
            for (auto&& handle : m_handles) {
                handle->SetTarget(selection.get());
            }

            SDL_SetRenderDrawColor(&renderer, 0x00, 0x00, 0xFF, 0xFF);
            auto const& screenRect = selection->GetScreenRect();
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
        if (event.GetButton() != MouseButton::Left) {
            return false;
        }

        auto selection = m_editorLayer.GetSelection();

        // if we clicked outside the bounds of the selection check for a new selection
        if (!selection || !selection->IsVisible() || !selection->IsInBounds(event.GetPosition())) {
            auto const oldSelection = selection;
            selection = nullptr;

            for (auto&& child : m_editorLayer.GetRoot()->GetChildren()) {
                if (child->IsVisible() && child->IsInBounds(event.GetPosition())) {
                    selection = child;
                    break;
                }
            }

            m_editorLayer.SetSelection(selection);
        }

        if (selection) {
            m_holding = true;
            BeginEdit();
            return true;
        }
        return false;
    }

    bool BoundsWidget::OnMouseUp(EventMouseUp const& event) {
        if (event.GetButton() != MouseButton::Left) {
            return false;
        }
        if (m_holding) {
            m_holding = false;
            EndEdit();
        }
        return false;
    }

    bool BoundsWidget::OnMouseMove(EventMouseMove const& event) {
        if (m_holding) {
            if (auto const selection = m_editorLayer.GetSelection()) {
                auto& bounds = selection->GetLayoutRect();
                bounds.offset.topLeft.x += event.GetDelta().x;
                bounds.offset.bottomRight.x += event.GetDelta().x;
                bounds.offset.topLeft.y += event.GetDelta().y;
                bounds.offset.bottomRight.y += event.GetDelta().y;
                selection->RecalculateBounds();
            }
        }
        return false;
    }
}
