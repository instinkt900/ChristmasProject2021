#include "game_pch.h"
#include "ui_layer.h"
#include "moth_ui/group.h"
#include "moth_ui/context.h"

UILayer::UILayer(std::filesystem::path const& layoutPath) {
    LoadLayout(layoutPath);
}

bool UILayer::OnEvent(moth_ui::Event const& event) {
    bool handled = false;
    if (m_root) {
        handled = m_root->SendEvent(event, moth_ui::Node::EventDirection::Down);
    }
    return handled;
}

void UILayer::Update(uint32_t ticks) {
    if (m_root) {
        m_root->Update(ticks);
    }
}

void UILayer::Draw(SDL_Renderer& renderer) {
    if (m_root) {
        m_root->Draw();
    }
}

void UILayer::OnAddedToStack(LayerStack* stack) {
    Layer::OnAddedToStack(stack);

    if (m_root) {
        moth_ui::IntRect rect;
        rect.topLeft = { 0, 0 };
        rect.bottomRight = { GetWidth(), GetHeight() };
        m_root->SetScreenRect(rect);
    }
}

void UILayer::OnRemovedFromStack() {
    Layer::OnRemovedFromStack();
}

void UILayer::LoadLayout(std::filesystem::path const& path) {
    m_root = moth_ui::Context::GetCurrentContext()->GetNodeFactory().Create(path, GetWidth(), GetHeight());
}
