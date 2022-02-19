#pragma once

enum EventType : int {
    EVENTTYPE_RENDERDEVICERESET,
    EVENTTYPE_RENDERTARGETRESET,
    EVENTTYPE_WINDOWSIZE,
    EVENTTYPE_QUIT,
    EVENTTYPE_KEY,
    EVENTTYPE_WIDGET_RESIZE,
    EVENTTYPE_MOUSE_DOWN,
    EVENTTYPE_MOUSE_UP,
    EVENTTYPE_MOUSE_MOVE,
    EVENTTYPE_ANIMATION,
};

class Event {
public:
    Event(int type)
        : m_type(type) {}
    virtual ~Event() {}

    int GetType() const { return m_type; }

    static std::unique_ptr<Event> FromSDL(SDL_Event const& event);

protected:
    int m_type;
};

template<typename T>
T const* event_cast(Event const& event) {
    if (event.GetType() == T::GetStaticType()) {
        return static_cast<T const*>(&event);
    }
    return nullptr;
}
