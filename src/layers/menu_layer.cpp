#include "game_pch.h"
#include "menu_layer.h"
#include "game.h"
#include "layers/game_layer.h"
#include "moth_ui/node.h"
#include "moth_ui/layout/layout.h"
#include "moth_ui/layout/layout_entity_group.h"
#include "moth_ui/group.h"
#include "moth_ui/node_factory.h"
#include "moth_ui/context.h"
#include "ui/ui_button.h"
#include "loading_layer.h"

MenuLayer::MenuLayer(Game& game)
    : m_game(game) {
    m_root = moth_ui::Context::GetCurrentContext()->GetNodeFactory().Create("layouts/menu_screen.mothui", 0, 0);

    if (auto startButton = m_root->FindChild<UIButton>("btn_play")) {
        startButton->SetClickAction([&]() {
            auto layerStack = m_layerStack;
            auto loadingLayer = std::make_unique<LoadingLayer>(m_game);
            layerStack->PopLayer(); // removes 'this'
            layerStack->PushLayer(std::move(loadingLayer));
        });
    }
    if (auto scoresButton = m_root->FindChild<UIButton>("btn_scores")) {
    }
    if (auto startButton = m_root->FindChild<UIButton>("btn_exit")) {
        startButton->SetClickAction([&]() {
            m_game.Stop();
        });
    }
}

MenuLayer::~MenuLayer() {
}

bool MenuLayer::OnEvent(moth_ui::Event const& event) {
    if (m_root && m_root->SendEvent(event, moth_ui::Node::EventDirection::Down)) {
        return true;
    }
    moth_ui::EventDispatch dispatch(event);
    dispatch.Dispatch(this, &MenuLayer::OnAnimEvent);
    return dispatch.GetHandled();
}

void MenuLayer::Update(uint32_t ticks) {
    if (m_root) {
        m_root->Update(ticks);
    }
}

void MenuLayer::Draw(SDL_Renderer& renderer) {
    if (m_root) {
        m_root->Draw();
    }
}

void MenuLayer::DebugDraw() {
    if (ImGui::CollapsingHeader("MenuLayer")) {
        if (ImGui::TreeNode("root")) {
            ImGui::TreePop();
        }
    }
}

void MenuLayer::OnAddedToStack(LayerStack* stack) {
    Layer::OnAddedToStack(stack);

    moth_ui::IntRect widgetRect;
    widgetRect.topLeft = { 0, 0 };
    widgetRect.bottomRight = { GetWidth(), GetHeight() };
    m_root->SetScreenRect(widgetRect);
}

void MenuLayer::OnRemovedFromStack() {
    m_root.reset();
}

bool MenuLayer::OnAnimEvent(moth_ui::EventAnimation const& event) {
    return true;
}
