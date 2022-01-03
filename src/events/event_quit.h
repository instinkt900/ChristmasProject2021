#pragma once

#include "event.h"

class EventQuit : public Event {
public:
    EventQuit()
        : Event(GetStaticType()) {}
    virtual ~EventQuit() {}

    static EventType GetStaticType() {
        return EventType::Quit;
    }
};
