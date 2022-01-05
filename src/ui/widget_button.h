#pragma once

#include "widget.h"

class EventMouseDown;
class EventMouseUp;

enum class ButtonState {
    Unknown,    // indicates something is wrong
    Pressed,    // when the mouse is actively pressing down on the button
    Released,   // default state where nothing is interacting
};

class WidgetButton : public Widget {
public:
    WidgetButton();
    virtual ~WidgetButton();

    bool OnEvent(Event const& event) override;
    void Update(uint32_t ticks) override;
    void Draw(SDL_Renderer& renderer) override;

    void SetOnClickedCallback(std::function<void()> const& callback) { m_callback = callback; }

protected:
    ButtonState m_state = ButtonState::Released;
    std::function<void()> m_callback;

    bool OnButtonDown(EventMouseDown const& event);
    bool OnButtonUp(EventMouseUp const& event);
};
