#pragma once

#include "event.h"

class EventRenderDeviceReset : public Event {
public:
    EventRenderDeviceReset()
        : Event(GetStaticType()) {}
    virtual ~EventRenderDeviceReset() {}

    static int GetStaticType() { return EVENTTYPE_RENDERDEVICERESET; }
};

class EventRenderTargetReset : public Event {
public:
    EventRenderTargetReset()
        : Event(GetStaticType()) {}
    virtual ~EventRenderTargetReset() {}

    static int GetStaticType() { return EVENTTYPE_RENDERTARGETRESET; }
};
