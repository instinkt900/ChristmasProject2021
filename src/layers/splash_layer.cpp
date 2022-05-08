#include "game_pch.h"
#include "splash_layer.h"
#include "moth_ui/group.h"
#include "menu_layer.h"
#include "layer_stack.h"

SplashLayer::SplashLayer(Game& game)
    : UILayer("layouts/title.mothui")
    , m_game(game) {
}

bool SplashLayer::OnEvent(moth_ui::Event const& event) {
    if (!UILayer::OnEvent(event)) {
        moth_ui::EventDispatch dispatch(event);
        dispatch.Dispatch(this, &SplashLayer::OnKeyEvent);
        return dispatch.GetHandled();
    }
    return true;
}

void SplashLayer::OnAddedToStack(LayerStack* stack) {
    UILayer::OnAddedToStack(stack);
    if (m_root) {
        m_root->SetAnimation("intro");
    }
}

bool SplashLayer::OnKeyEvent(moth_ui::EventKey const& event) {
    if (event.GetAction() == moth_ui::KeyAction::Up) {
        if (event.GetKey() == moth_ui::Key::Space) {
            auto layerStack = m_layerStack;
            auto menuLayer = std::make_unique<MenuLayer>(m_game);
            layerStack->ClearLayers();
            layerStack->PushLayer(std::move(menuLayer));
            return true;
        }
    }
    return false;
}
