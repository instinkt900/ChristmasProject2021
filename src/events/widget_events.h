#pragma once

#include "event.h"

class EventWidgetResize : public Event {
public:
    EventWidgetResize():Event(GetStaticType()) {}
    virtual ~EventWidgetResize() {}

    static int GetStaticType() { return EVENTTYPE_WIDGET_RESIZE; }
};
