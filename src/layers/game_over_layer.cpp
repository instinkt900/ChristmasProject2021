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
    : m_game(game) {
    m_root = moth_ui::Context::GetCurrentContext()->GetNodeFactory().Create("layouts/game_over.mothui", 0, 0);

    if (auto replayButton = m_root->FindChild<UIButton>("btn_replay")) {
        replayButton->SetClickAction([&]() {
            auto layerStack = m_layerStack;
            auto loadingLayer = std::make_unique<LoadingLayer>(m_game);
            layerStack->PopLayer(); // removes 'this'
            layerStack->PushLayer(std::move(loadingLayer));
        });
    }
    if (auto menuButton = m_root->FindChild<UIButton>("btn_menu")) {
        menuButton->SetClickAction([&]() {
            auto layerStack = m_layerStack;
            auto menuLayer = std::make_unique<MenuLayer>(m_game);
            layerStack->PopLayer(); // removes 'this'
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

bool GameOverLayer::OnEvent(moth_ui::Event const& event) {
    if (m_root && m_root->SendEvent(event, moth_ui::Node::EventDirection::Down)) {
        return true;
    }
    moth_ui::EventDispatch dispatch(event);
    return dispatch.GetHandled();
}

void GameOverLayer::Update(uint32_t ticks) {
    if (m_root) {
        m_root->Update(ticks);
    }
}

void GameOverLayer::Draw(SDL_Renderer& renderer) {
    if (m_root) {
        m_root->Draw();
    }
}

void GameOverLayer::OnAddedToStack(LayerStack* stack) {
    Layer::OnAddedToStack(stack);

    moth_ui::IntRect widgetRect;
    widgetRect.topLeft = { 0, 0 };
    widgetRect.bottomRight = { GetWidth(), GetHeight() };
    m_root->SetScreenRect(widgetRect);
    m_root->SetAnimation("in");
}

void GameOverLayer::OnRemovedFromStack() {
    m_root.reset();
}

void GameOverLayer::SetNewHighScore(int score) {
    if (auto highScoreLabel = m_root->FindChild("high_score")) {
        highScoreLabel->SetVisible(true);
    }

    if (auto highScoreText = m_root->FindChild<moth_ui::NodeText>("txt_score")) {
        highScoreText->SetVisible(true);
        highScoreText->SetText(fmt::format("{}", score).c_str());
    }
}
