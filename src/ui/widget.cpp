#include "game_pch.h"
#include "widget.h"

Widget::Widget() {
}

Widget::~Widget() {
}

bool Widget::OnEvent(SDL_Event const& event) {
    for (auto&& child : m_children) {
        if (child->OnEvent(event)) {
            return true;
        }
    }
    return false;
}

void Widget::Update(uint32_t ticks) {
    for (auto&& child : m_children) {
        child->Update(ticks);
    }
}

void Widget::Draw(SDL_Renderer& renderer) {
    for (auto&& child : m_children) {
        child->Draw(renderer);
    }
}

void Widget::SetRect(WidgetRect const& rect) {
    m_screenRect = rect;
    for (auto&& child : m_children) {
        child->RecalculateBounds();
    }
}

void Widget::RecalculateBounds() {
    if (nullptr != m_parent) {
        auto& parentBounds = m_parent->GetScreenRect();
        auto const parentWidth = parentBounds.bottomRight.x - parentBounds.topLeft.x;
        auto const parentHeight = parentBounds.bottomRight.y - parentBounds.topLeft.y;
        m_screenRect.topLeft.x = parentBounds.topLeft.x + m_layoutBounds.topLeft.offset.x + static_cast<int>(parentWidth * m_layoutBounds.topLeft.anchor.x);
        m_screenRect.topLeft.y = parentBounds.topLeft.y + m_layoutBounds.topLeft.offset.y + static_cast<int>(parentHeight * m_layoutBounds.topLeft.anchor.y);
        m_screenRect.bottomRight.x = parentBounds.topLeft.x + m_layoutBounds.bottomRight.offset.x + static_cast<int>(parentWidth * m_layoutBounds.bottomRight.anchor.x);
        m_screenRect.bottomRight.y = parentBounds.topLeft.y + m_layoutBounds.bottomRight.offset.y + static_cast<int>(parentHeight * m_layoutBounds.bottomRight.anchor.y);
    }

    for (auto&& child : m_children) {
        child->RecalculateBounds();
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
