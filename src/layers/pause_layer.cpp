#include "game_pch.h"
#include "pause_layer.h"
#include "ui/ui_button.h"
#include "menu_layer.h"
#include "layers/game_layer.h"
#include "states/state_game.h"

PauseLayer::PauseLayer(GameLayer& gameLayer)
    : UILayer("resources/layouts/pause.mothui")
    , m_gameLayer(gameLayer) {
    if (m_root) {
        m_root->SetEventHandler([&](moth_ui::Node* node, moth_ui::Event const& event) {
            moth_ui::EventDispatch dispatch(event);
            dispatch.Dispatch(this, &PauseLayer::OnAnimationEvent);
            return dispatch.GetHandled();
        });
        if (auto resumeButton = m_root->FindChild<UIButton>("btn_resume")) {
            resumeButton->SetClickAction([&]() {
                m_root->SetAnimation("out");
                m_leaveDest = LeaveDest::Resume;
            });
        }
        if (auto menuButton = m_root->FindChild<UIButton>("btn_menu")) {
            menuButton->SetClickAction([&]() {
                m_root->SetAnimation("out");
                m_leaveDest = LeaveDest::Menu;
            });
        }
    }
}

bool PauseLayer::OnEvent(moth_ui::Event const& event) {
    if (!UILayer::OnEvent(event)) {
        moth_ui::EventDispatch dispatch(event);
        dispatch.Dispatch(this, &PauseLayer::OnAnimationEvent);
        return dispatch.GetHandled();
    }
    return true;
}

void PauseLayer::OnAddedToStack(LayerStack* layerStack) {
    UILayer::OnAddedToStack(layerStack);

    if (m_root) {
        m_root->SetAnimation("in");
    }
}

bool PauseLayer::OnAnimationEvent(moth_ui::EventAnimation const& event) {
    if (event.GetName() == "out") {
        switch (m_leaveDest) {
        case LeaveDest::Menu: {
            // go to menu
            m_root->SetEventHandler(nullptr);
            auto layerStack = m_layerStack;
            auto menuLayer = std::make_unique<MenuLayer>(m_gameLayer.GetGame());
            layerStack->ClearLayers();
            layerStack->PushLayer(std::move(menuLayer));
        } break;
        case LeaveDest::Resume: {
            // go to game
            m_gameLayer.GetStateMachine().StateTransition<StateGame>();
        } break;
        }
        return true;
    }
    return false;
}
