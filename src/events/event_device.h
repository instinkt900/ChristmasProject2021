#pragma once

#include "event.h"

class EventRenderDeviceReset : public Event {
public:
    EventRenderDeviceReset()
        : Event(GetStaticType()) {}
    virtual ~EventRenderDeviceReset() {}

    static EventType GetStaticType() { return EventType::RenderDeviceReset; }
};

class EventRenderTargetReset : public Event {
public:
    EventRenderTargetReset()
        : Event(GetStaticType()) {}
    virtual ~EventRenderTargetReset() {}

    static EventType GetStaticType() { return EventType::RenderTargetReset; }
};
