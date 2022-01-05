#pragma once

#include "event.h"

enum class MouseButton {
    Unknown,
    Left,
    Middle,
    Right,
};

inline MouseButton FromSDLMouse(uint8_t button) {
    switch (button) {
    case 1:
        return MouseButton::Left;
    case 2:
        return MouseButton::Middle;
    case 3:
        return MouseButton::Right;
    default:
        return MouseButton::Unknown;
    }
}

class EventMouseDown : public Event {
public:
    EventMouseDown(MouseButton button, IntVec2 const& position)
        : Event(GetStaticType())
        , m_button(button)
        , m_position(position) {}
    virtual ~EventMouseDown() {}

    static int GetStaticType() { return EVENTTYPE_MOUSE_DOWN; }

    MouseButton GetButton() const { return m_button; }
    IntVec2 const& GetPosition() const { return m_position; }

public:
    MouseButton m_button;
    IntVec2 m_position;
};

class EventMouseUp : public Event {
public:
    EventMouseUp(MouseButton button, IntVec2 const& position)
        : Event(GetStaticType())
        , m_button(button)
        , m_position(position) {}
    virtual ~EventMouseUp() {}

    static int GetStaticType() { return EVENTTYPE_MOUSE_UP; }

    MouseButton GetButton() const { return m_button; }
    IntVec2 const& GetPosition() const { return m_position; }

public:
    MouseButton m_button;
    IntVec2 m_position;
};
