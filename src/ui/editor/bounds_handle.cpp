#include "game_pch.h"
#include "bounds_handle.h"
#include "events/event_dispatch.h"

namespace ui {
    BoundsHandle::BoundsHandle(BoundsHandleAnchor const& anchor)
        : m_anchor(anchor) {
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
            return true;
        }

        return false;
    }

    bool BoundsHandle::OnMouseUp(EventMouseUp const& event) {
        if (event.GetButton() != MouseButton::Left) {
            return false;
        }

        m_holding = false;

        return false;
    }
}
