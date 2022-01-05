#include "game_pch.h"
#include "widget_button.h"
#include "events/event_mouse.h"
#include "events/event_dispatch.h"

WidgetButton::WidgetButton() {
}

WidgetButton::~WidgetButton() {
}

bool WidgetButton::OnEvent(Event const& event) {
    EventDispatch dispatch(event);
    dispatch.Dispatch(this, &WidgetButton::OnButtonDown);
    dispatch.Dispatch(this, &WidgetButton::OnButtonUp);
    return dispatch.GetHandled();
}

void WidgetButton::Update(uint32_t ticks) {
    Widget::Update(ticks);
}

void WidgetButton::Draw(SDL_Renderer& renderer) {
    Widget::Draw(renderer);
}

bool WidgetButton::OnButtonDown(EventMouseDown const& event) {
    if (event.GetButton() == MouseButton::Left && IsInBounds(event.GetPosition())) {
        m_state = ButtonState::Pressed;
        return true;
    }
    return false;
}

bool WidgetButton::OnButtonUp(EventMouseUp const& event) {
    if (event.GetButton() == MouseButton::Left && m_state == ButtonState::Pressed) {
        m_state = ButtonState::Released;
        if (m_callback) {
            m_callback();
        }
        return true;
    }
    return false;
}
