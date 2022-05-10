#include "game_pch.h"
#include "menu_layer.h"
#include "game.h"
#include "moth_ui/group.h"
#include "ui/ui_button.h"
#include "loading_layer.h"
#include "scores_layer.h"

MenuLayer::MenuLayer(Game& game)
    : UILayer("layouts/menu_screen.mothui")
    , m_game(game) {

    if (m_root) {
        if (auto startButton = m_root->FindChild<UIButton>("btn_play")) {
            startButton->SetClickAction([&]() {
                auto layerStack = m_layerStack;
                auto loadingLayer = std::make_unique<LoadingLayer>(m_game);
                layerStack->PopLayer(); // removes 'this'
                layerStack->PushLayer(std::move(loadingLayer));
            });
        }
        if (auto scoresButton = m_root->FindChild<UIButton>("btn_scores")) {
            scoresButton->SetClickAction([&]() {
                auto layerStack = m_layerStack;
                auto scoresLayer = std::make_unique<ScoresLayer>(m_game);
                layerStack->PopLayer(); // removes 'this'
                layerStack->PushLayer(std::move(scoresLayer));
            });
        }
        if (auto startButton = m_root->FindChild<UIButton>("btn_exit")) {
            startButton->SetClickAction([&]() {
                m_game.Stop();
            });
        }
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
    UILayer::OnAddedToStack(stack);

    if (m_root) {
        if (auto menu = m_root->FindChild("menu")) {
            menu->SetAnimation("in");
        }
    }
}