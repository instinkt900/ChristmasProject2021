#pragma once

#include "event.h"
#include "event_listener.h"
#include "moth_ui/events/event.h"
#include "moth_ui/event_listener.h"
#include "ui_utils.h"

class EventDispatch {
public:
    EventDispatch(Event const& event)
        : m_event(event) {}

    bool GetHandled() const { return m_handled; }

    void Dispatch(EventListener* listener) {
        if (m_handled)
            return;
        if (listener && listener->OnEvent(m_event)) {
            m_handled = true;
        }
    }

    void Dispatch(moth_ui::EventListener* listener) {
        if (m_handled)
            return;
        auto const uiEvent = ToUIEvent(m_event);
        if (listener && uiEvent && listener->OnEvent(*uiEvent)) {
            m_handled = true;
        }
    }

    template <typename T, typename E>
    void Dispatch(T* obj, bool (T::*func)(E const& event)) {
        if (m_handled)
            return;
        if (auto specificEvent = event_cast<E>(m_event)) {
            if (std::invoke(func, obj, *specificEvent)) {
                m_handled = true;
            }
        }
    }

    template <typename T>
    void Dispatch(std::function<bool(T const&)> const& func) {
        if (m_handled) {
            return;
        }
        if (auto specificEvent = event_cast<T>(m_event)) {
            if (func(*specificEvent)) {
                m_handled = true;
            }
        }
    }

private:
    Event const& m_event;
    bool m_handled = false;
};