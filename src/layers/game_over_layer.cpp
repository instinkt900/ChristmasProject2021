#include "game_pch.h"
#include "game_over_layer.h"
#include "game.h"
#include "moth_ui/context.h"
#include "moth_ui/group.h"
#include "moth_ui/node_text.h"
#include "../ui/ui_button.h"
#include "menu_layer.h"
#include "loading_layer.h"

GameOverLayer::GameOverLayer(Game& game)
    :UILayer("resources/layouts/game_over.mothui")
    , m_game(game) {

    if (m_root) {
        if (auto replayButton = m_root->FindChild<UIButton>("btn_replay")) {
            replayButton->SetClickAction([&]() {
                auto layerStack = m_layerStack;
                auto loadingLayer = std::make_unique<LoadingLayer>(m_game);
                layerStack->ClearLayers();
                layerStack->PushLayer(std::move(loadingLayer));
            });
        }
        if (auto menuButton = m_root->FindChild<UIButton>("btn_menu")) {
            menuButton->SetClickAction([&]() {
                auto layerStack = m_layerStack;
                auto menuLayer = std::make_unique<MenuLayer>(m_game);
                layerStack->ClearLayers();
                layerStack->PushLayer(std::move(menuLayer));
            });
        }

        if (auto highScoreLabel = m_root->FindChild("high_score")) {
            highScoreLabel->SetVisible(false);
        }

        if (auto highScoreText = m_root->FindChild("txt_score")) {
            highScoreText->SetVisible(false);
        }
    }
}

void GameOverLayer::OnAddedToStack(LayerStack* stack) {
    UILayer::OnAddedToStack(stack);

    if (m_root) {
        m_root->SetAnimation("in");
    }
}

void GameOverLayer::SetNewHighScore(int score) {
    if (m_root) {
        if (auto highScoreLabel = m_root->FindChild("high_score")) {
            highScoreLabel->SetVisible(true);
        }

        if (auto highScoreText = m_root->FindChild<moth_ui::NodeText>("txt_score")) {
            highScoreText->SetVisible(true);
            highScoreText->SetText(fmt::format("{}", score).c_str());
        }
    }
}
