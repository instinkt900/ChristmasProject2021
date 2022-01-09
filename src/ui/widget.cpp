#include "game_pch.h"
#include "widget.h"
#include "events/event_dispatch.h"
#include "events/widget_events.h"
#include "debug/inspectors.h"
#include "widget_animation.h"
#include "widget_image.h"

Widget::Widget() {
}

Widget::Widget(WidgetDesc const& desc)
    : m_id(desc.id)
    , m_layoutBounds(desc.layoutBounds) {
    for (auto&& childDesc : desc.children) {

    }
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
    if (m_showRect) {
        SDL_Rect rect{ m_screenRect.topLeft.x, m_screenRect.topLeft.y, m_screenRect.bottomRight.x - m_screenRect.topLeft.x, m_screenRect.bottomRight.y - m_screenRect.topLeft.y };
        SDL_SetRenderDrawColor(&renderer, 0xFF, 0x00, 0x00, 0xFF);
        SDL_RenderDrawRect(&renderer, &rect);
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
        auto const& parentBounds = m_parent->GetScreenRect();
        auto const parentWidth = parentBounds.bottomRight.x - parentBounds.topLeft.x;
        auto const parentHeight = parentBounds.bottomRight.y - parentBounds.topLeft.y;
        m_screenRect.topLeft.x = parentBounds.topLeft.x + static_cast<int>(m_layoutBounds.topLeft.offset.x + parentWidth * m_layoutBounds.topLeft.anchor.x);
        m_screenRect.topLeft.y = parentBounds.topLeft.y + static_cast<int>(m_layoutBounds.topLeft.offset.y + parentHeight * m_layoutBounds.topLeft.anchor.y);
        m_screenRect.bottomRight.x = parentBounds.topLeft.x + static_cast<int>(m_layoutBounds.bottomRight.offset.x + parentWidth * m_layoutBounds.bottomRight.anchor.x);
        m_screenRect.bottomRight.y = parentBounds.topLeft.y + static_cast<int>(m_layoutBounds.bottomRight.offset.y + parentHeight * m_layoutBounds.bottomRight.anchor.y);
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

bool Widget::HasAnimation(std::string const& name) {
    if (m_tracks) {
        return m_tracks->HasAnimation(name);
    }
    return false;
}
bool Widget::SetAnimation(std::string const& name) {
    if (m_tracks) {
        return m_tracks->SetAnimation(name);
    }
    return false;
}
void Widget::StopAnimation() {
    if (m_tracks) {
        m_tracks->StopAnimation();
    }
}

void Widget::DebugDraw() {
    auto const label = fmt::format("Widget({})", m_id);
    if (ImGui::TreeNode(label.c_str())) {
        ImGui::LabelText("parent", "%d", (void*)m_parent);
        ImGuiInspectMember("visible", m_visible);
        ImGuiInspectMember("show_rect", m_showRect);
        ImGuiInspectMember("bounds", m_layoutBounds);
        ImGuiInspectMember("screen_rect", m_screenRect);
        if (ImGui::Button("Recalculate")) {
            RecalculateBounds(true);
        }
        for (auto&& child : m_children) {
            child->DebugDraw();
        }
        ImGui::TreePop();
    }
}

WidgetRef Widget::CreateWidget(WidgetDesc const& desc) {
    if (desc.type == WidgetType::Node) {
        return std::make_shared<WidgetImage>(desc);
    } else if (desc.type == WidgetType::Image) {
        return std::make_shared<Widget>(desc);
    }
    assert(false);
    return nullptr;
}
