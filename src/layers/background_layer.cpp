#include "game_pch.h"
#include "background_layer.h"

BackgroundLayer::BackgroundLayer(SDL_Renderer& renderer) {
    m_backgroundTexture = CreateTextureRef(&renderer, "title_raw.jpg");
}

BackgroundLayer::~BackgroundLayer() {
}

void BackgroundLayer::Draw(SDL_Renderer& renderer) {
    SDL_RenderCopy(&renderer, m_backgroundTexture.get(), nullptr, nullptr);
}