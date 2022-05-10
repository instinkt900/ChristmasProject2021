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
        m_root->SetEventHandler([this](moth_ui::Node*, moth_ui::Event const& event) { return OnUIEvent(event); });
        if (auto menu = m_root->FindChild("menu")) {
            if (auto startButton = m_root->FindChild<UIButton>("btn_play")) {
                startButton->SetClickAction([&, menu]() {
                    m_exitDestination = ExitDestination::Play;
                    menu->SetAnimation("out");
                });
            }
            if (auto scoresButton = m_root->FindChild<UIButton>("btn_scores")) {
                scoresButton->SetClickAction([&, menu]() {
                    m_exitDestination = ExitDestination::Scores;
                    menu->SetAnimation("out");
                });
            }
            if (auto startButton = m_root->FindChild<UIButton>("btn_exit")) {
                startButton->SetClickAction([&, menu]() {
                    m_exitDestination = ExitDestination::Exit;
                    menu->SetAnimation("out");
                });
            }
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

bool MenuLayer::OnUIEvent(moth_ui::Event const& event) {
    moth_ui::EventDispatch dispatch(event);
    dispatch.Dispatch(this, &MenuLayer::OnAnimationEnd);
    return dispatch.GetHandled();
}

bool MenuLayer::OnAnimationEnd(moth_ui::EventAnimationStopped const& event) {
    if (event.GetClipName() == "out") {
        auto layerStack = m_layerStack;
        switch (m_exitDestination) {
        case ExitDestination::Play: {
            auto loadingLayer = std::make_unique<LoadingLayer>(m_game);
            layerStack->PopLayer(); // removes 'this'
            layerStack->PushLayer(std::move(loadingLayer));
        } break;
        case ExitDestination::Scores: {
            auto scoresLayer = std::make_unique<ScoresLayer>(m_game);
            layerStack->PopLayer(); // removes 'this'
            layerStack->PushLayer(std::move(scoresLayer));
        } break;
        case ExitDestination::Exit: {
            m_game.Stop();
        } break;
        }
        return true;
    }
    return false;
}
