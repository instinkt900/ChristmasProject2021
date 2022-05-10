#include "game_pch.h"
#include "scores_layer.h"
#include "game.h"
#include "moth_ui/group.h"
#include "ui/ui_button.h"
#include "menu_layer.h"

ScoresLayer::ScoresLayer(Game& game)
    : UILayer("layouts/scores.mothui")
    , m_game(game) {

    if (m_root) {
        m_root->SetEventHandler([this](moth_ui::Node*, moth_ui::Event const& event) { return OnUIEvent(event); });
        if (auto backButton = m_root->FindChild<UIButton>("btn_back")) {
            backButton->SetClickAction([&]() {
                m_root->SetAnimation("out");
            });
        }
    }
}

void ScoresLayer::DebugDraw() {
    if (ImGui::CollapsingHeader("ScoresLayer")) {
        if (ImGui::TreeNode("root")) {
            ImGui::TreePop();
        }
    }
}

void ScoresLayer::OnAddedToStack(LayerStack* stack) {
    UILayer::OnAddedToStack(stack);

    if (m_root) {
        m_root->SetAnimation("in");
    }
}

bool ScoresLayer::OnUIEvent(moth_ui::Event const& event) {
    moth_ui::EventDispatch dispatch(event);
    dispatch.Dispatch(this, &ScoresLayer::OnAnimationEnded);
    return dispatch.GetHandled();
}

bool ScoresLayer::OnAnimationEnded(moth_ui::EventAnimationStopped const& event) {
    if (event.GetClipName() == "out") {
        auto layerStack = m_layerStack;
        auto menuLayer = std::make_unique<MenuLayer>(m_game);
        layerStack->ClearLayers();
        layerStack->PushLayer(std::move(menuLayer));
    }
    return false;
}
