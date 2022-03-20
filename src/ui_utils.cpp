#include "game_pch.h"
#include "ui_utils.h"

#include "events/event_device.h"
#include "events/event_window_size.h"
#include "events/event_quit.h"
#include "events/event_key.h"
#include "events/event_mouse.h"

#include "moth_ui/events/event_device.h"
#include "moth_ui/events/event_window_size.h"
#include "moth_ui/events/event_quit.h"
#include "moth_ui/events/event_key.h"
#include "moth_ui/events/event_mouse.h"

template <typename R>
class EventSwitch {
public:
    EventSwitch(Event const& event)
        : m_event(event) {}

    template <typename T>
    void Case(std::function<R(T const&)> const& func) {
        if (m_result.has_value()) {
            return;
        }
        if (auto specificEvent = event_cast<T>(m_event)) {
            m_result = func(*specificEvent);
        }
    }

    R GetResult() {
        assert(m_result.has_value() && "Unknown event type");
        return std::move(m_result.value());
    }

private:
    std::optional<R> m_result;
    Event const& m_event;
};

std::unique_ptr<moth_ui::Event> ToUIEvent(Event const& inEvent) {
    EventSwitch<std::unique_ptr<moth_ui::Event>> eventSwitch(inEvent);
    eventSwitch.Case<EventRenderDeviceReset>([](auto const& inEvent) {
        return std::make_unique<moth_ui::EventRenderDeviceReset>();
    });
    eventSwitch.Case<EventRenderTargetReset>([](auto const& inEvent) {
        return std::make_unique<moth_ui::EventRenderTargetReset>();
    });
    eventSwitch.Case<EventWindowSize>([](auto const& inEvent) {
        return std::make_unique<moth_ui::EventWindowSize>(inEvent.GetWidth(), inEvent.GetHeight());
    });
    eventSwitch.Case<EventQuit>([](auto const& inEvent) {
        return std::make_unique<moth_ui::EventQuit>();
    });
    eventSwitch.Case<EventKey>([](auto const& inEvent) {
        auto const action = static_cast<moth_ui::KeyAction>(inEvent.GetAction());
        auto const key = static_cast<moth_ui::Key>(inEvent.GetKey());
        return std::make_unique<moth_ui::EventKey>(action, key);
    });
    eventSwitch.Case<EventMouseDown>([](auto const& inEvent) {
        auto const button = static_cast<moth_ui::MouseButton>(inEvent.GetButton());
        auto const position = moth_ui::IntVec2{ inEvent.GetPosition().x, inEvent.GetPosition().y };
        return std::make_unique<moth_ui::EventMouseDown>(button, position);
    });
    eventSwitch.Case<EventMouseUp>([](auto const& inEvent) {
        auto const button = static_cast<moth_ui::MouseButton>(inEvent.GetButton());
        auto const position = moth_ui::IntVec2{ inEvent.GetPosition().x, inEvent.GetPosition().y };
        return std::make_unique<moth_ui::EventMouseUp>(button, position);
    });
    eventSwitch.Case<EventMouseMove>([](auto const& inEvent) {
        auto const position = moth_ui::IntVec2{ inEvent.GetPosition().x, inEvent.GetPosition().y };
        auto const delta = moth_ui::FloatVec2{ inEvent.GetDelta().x, inEvent.GetDelta().y };
        return std::make_unique<moth_ui::EventMouseMove>(position, delta);
    });
    eventSwitch.Case<EventMouseWheel>([](auto const& inEvent) {
        auto const delta = moth_ui::IntVec2{ inEvent.GetDelta().x, inEvent.GetDelta().y };
        return std::make_unique<moth_ui::EventMouseWheel>(delta);
    });
    return eventSwitch.GetResult();
}
