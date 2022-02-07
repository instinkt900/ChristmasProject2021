#include "game_pch.h"
#include "bounds_handle.h"
#include "events/event_dispatch.h"
#include "bounds_widget.h"

namespace ui {
    BoundsHandle::BoundsHandle(BoundsWidget& widget, BoundsHandleAnchor const& anchor)
        : m_widget(widget)
        , m_anchor(anchor) {
    }

    BoundsHandle::~BoundsHandle() {
    }

    void BoundsHandle::SetTarget(Node* node) {
        m_target = node;
    }

    bool BoundsHandle::OnEvent(Event const& event) {
        EventDispatch dispatch(event);
        dispatch.Dispatch(this, &BoundsHandle::OnMouseDown);
        dispatch.Dispatch(this, &BoundsHandle::OnMouseUp);
        dispatch.Dispatch(this, &BoundsHandle::OnMouseMove);
        return dispatch.GetHandled();
    }

    bool BoundsHandle::OnMouseDown(EventMouseDown const& event) {
        if (event.GetButton() != MouseButton::Left) {
            return false;
        }

        if (IsInBounds(event.GetPosition())) {
            m_holding = true;
            m_widget.BeginEdit();
            return true;
        }

        return false;
    }

    bool BoundsHandle::OnMouseUp(EventMouseUp const& event) {
        if (event.GetButton() != MouseButton::Left) {
            return false;
        }

        if (m_holding) {
            m_widget.EndEdit();
        }

        m_holding = false;

        return false;
    }
}