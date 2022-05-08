#include "game_pch.h"
#include "loading_layer.h"
#include "game_layer.h"
#include "layer_stack.h"
#include "audio_factory.h"
#include "game.h"

LoadingLayer::LoadingLayer(Game& game)
    : UILayer("layouts/loading.mothui")
    , m_game(game) {
}

void LoadingLayer::Update(uint32_t ticks) {
    UILayer::Update(ticks);
    if (m_loadingFinished) {
        auto layerStack = m_layerStack;
        auto gameLayer = std::make_unique<GameLayer>(m_game);
        layerStack->ClearLayers();
        layerStack->PushLayer(std::move(gameLayer));
    }
}

void LoadingLayer::OnAddedToStack(LayerStack* layerStack) {
    UILayer::OnAddedToStack(layerStack);
    auto& audioFactory = m_game.GetAudioFactory();
    audioFactory.LoadAll([this]() {
        m_loadingFinished = true;
    });
}
