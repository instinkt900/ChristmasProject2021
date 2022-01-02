#include "game_pch.h"
#include "menu_layer.h"

MenuLayer::MenuLayer(Game& game)
    : m_game(game) {
}

MenuLayer::~MenuLayer() {
}

bool MenuLayer::OnEvent(SDL_Event const& event) {
    return false;
}

void MenuLayer::Update(uint32_t ticks) {
}

void MenuLayer::Draw(SDL_Renderer& renderer) {
}
