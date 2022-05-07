#include "game_pch.h"
#include "splash_layer.h"
#include "loading_layer.h"
#include "layer_stack.h"
#include "audio_factory.h"
#include "game.h"
#include "menu_layer.h"
#include "moth_ui/layout/layout.h"
#include "moth_ui/group.h"

SplashLayer::SplashLayer(Game& game)
    : m_game(game) {

    std::shared_ptr<moth_ui::Layout> layout;
    if (moth_ui::Layout::LoadResult::Success == moth_ui::Layout::Load("layouts/title.mothui", &layout)) {
        m_root = layout->Instantiate();
    }
}

SplashLayer::~SplashLayer() {
}

bool SplashLayer::OnEvent(moth_ui::Event const& event) {
    if (m_root->SendEvent(event, moth_ui::Node::EventDirection::Down)) {
        return true;
    }
    moth_ui::EventDispatch dispatch(event);
    dispatch.Dispatch(this, &SplashLayer::OnKeyEvent);
    return dispatch.GetHandled();
}

void SplashLayer::Update(uint32_t ticks) {
    m_root->Update(ticks);
}

void SplashLayer::Draw(SDL_Renderer& renderer) {
    m_root->Draw();
}

void SplashLayer::OnAddedToStack(LayerStack* stack) {
    Layer::OnAddedToStack(stack);

    moth_ui::IntRect widgetRect;
    widgetRect.topLeft = { 0, 0 };
    widgetRect.bottomRight = { GetWidth(), GetHeight() };
    m_root->SetScreenRect(widgetRect);
    m_root->SetAnimation("intro");
}

bool SplashLayer::OnKeyEvent(moth_ui::EventKey const& event) {
    if (event.GetAction() == moth_ui::KeyAction::Up) {
        if (event.GetKey() == moth_ui::Key::Space) {
            // we will delete 'this' here so we need to keep everything local
            auto layerStack = m_layerStack;
            //auto loadingLayer = std::make_unique<LoadingLayer>(m_game);
            //layerStack->PopLayer(); // removes 'this'
            //layerStack->PushLayer(std::move(loadingLayer));
            auto menuLayer = std::make_unique<MenuLayer>(m_game);
            layerStack->PopLayer(); // removes 'this'
            layerStack->PushLayer(std::move(menuLayer));
            return true;
        }
    }
    return false;
}
