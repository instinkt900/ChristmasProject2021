#pragma once

#include "event.h"

class EventQuit : public Event {
public:
    EventQuit()
        : Event(GetStaticType()) {}
    virtual ~EventQuit() {}

    static int GetStaticType() { return EVENTTYPE_QUIT; }
};
