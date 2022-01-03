#pragma once

enum class EventType {
    RenderDeviceReset,
    RenderTargetReset,
    WindowSize,
    Quit,
    Key,
};

class Event {
public:
    Event(EventType type)
        : m_type(type) {}
    virtual ~Event() {}

    EventType GetType() const { return m_type; }

    static std::unique_ptr<Event> FromSDL(SDL_Event const& event);

protected:
    EventType m_type;
};

template<typename T>
T const* event_cast(Event const& event) {
    if (event.GetType() == T::GetStaticType()) {
        return static_cast<T const*>(&event);
    }
    return nullptr;
}
