#pragma once

#include "event.h"

class EventWindowSize : public Event {
public:
    EventWindowSize(int width, int height)
        : Event(GetStaticType())
        , m_width(width)
        , m_height(height) {}
    virtual ~EventWindowSize() {}

    static EventType GetStaticType() { return EventType::WindowSize; }

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

private:
    int m_width = 0;
    int m_height = 0;
};