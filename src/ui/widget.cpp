#include "game_pch.h"
#include "widget.h"
#include "events/event_dispatch.h"
#include "events/widget_events.h"

Widget::Widget() {
}

Widget::~Widget() {
}

bool Widget::OnEvent(Event const& event) {
    EventDispatch dispatch(event);
    dispatch.Dispatch<EventWidgetResize>([this](auto event) { RecalculateBounds(false); return false; });
    for (auto&& child : m_children) {
        dispatch.Dispatch(child.get());
    }
    return dispatch.GetHandled();
}

void Widget::Update(uint32_t ticks) {
    for (auto&& child : m_children) {
        child->Update(ticks);
    }
}

void Widget::Draw(SDL_Renderer& renderer) {
    if (!IsVisible()) {
        return;
    }
    for (auto&& child : m_children) {
        child->Draw(renderer);
    }
}

void Widget::AddChild(WidgetRef widget) {
    m_children.push_back(widget);
    widget->SetParent(this);
}

void Widget::RemoveChild(WidgetRef widget) {
    auto it = std::find(m_children.begin(), m_children.end(), widget);
    if (std::end(m_children) != it) {
        (*it)->SetParent(nullptr);
        m_children.erase(it);
    }
}

void Widget::SetScreenRect(WidgetRect const& rect) {
    m_screenRect = rect;
    for (auto&& child : m_children) {
        child->RecalculateBounds(true);
    }
}

void Widget::RecalculateBounds(bool propagate) {
    if (nullptr != m_parent) {
        auto& parentBounds = m_parent->GetScreenRect();
        auto const parentWidth = parentBounds.bottomRight.x - parentBounds.topLeft.x;
        auto const parentHeight = parentBounds.bottomRight.y - parentBounds.topLeft.y;
        m_screenRect.topLeft.x = parentBounds.topLeft.x + m_layoutBounds.topLeft.offset.x + static_cast<int>(parentWidth * m_layoutBounds.topLeft.anchor.x);
        m_screenRect.topLeft.y = parentBounds.topLeft.y + m_layoutBounds.topLeft.offset.y + static_cast<int>(parentHeight * m_layoutBounds.topLeft.anchor.y);
        m_screenRect.bottomRight.x = parentBounds.topLeft.x + m_layoutBounds.bottomRight.offset.x + static_cast<int>(parentWidth * m_layoutBounds.bottomRight.anchor.x);
        m_screenRect.bottomRight.y = parentBounds.topLeft.y + m_layoutBounds.bottomRight.offset.y + static_cast<int>(parentHeight * m_layoutBounds.bottomRight.anchor.y);
    }

    if (propagate) {
        for (auto&& child : m_children) {
            child->RecalculateBounds(true);
        }
    }
}

bool Widget::IsInBounds(IntVec2 const& point) const {
    if (point.x > m_screenRect.bottomRight.x ||
        point.x < m_screenRect.topLeft.x ||
        point.y > m_screenRect.bottomRight.y ||
        point.y < m_screenRect.topLeft.y) {
        return false;
    }
    return true;
}

IntVec2 Widget::TranslatePosition(IntVec2 const& point) const {
    IntVec2 translated = point;
    translated.x -= m_screenRect.topLeft.x;
    translated.y -= m_screenRect.topLeft.y;
    return translated;
}
