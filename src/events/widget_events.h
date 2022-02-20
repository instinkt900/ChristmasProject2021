#pragma once

#include "event.h"

class EventWidgetResize : public Event {
public:
    EventWidgetResize()
        : Event(GetStaticType()) {}
    virtual ~EventWidgetResize() {}

    static constexpr int GetStaticType() { return EVENTTYPE_WIDGET_RESIZE; }

    std::unique_ptr<Event> Clone() const override {
        return std::make_unique<EventWidgetResize>();
    }
};
