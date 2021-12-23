#include "menu_layer.h"

#include <imgui.h>

#include "game_layer.h"
#include "layer_stack.h"

MenuLayer::MenuLayer() {

}

MenuLayer::~MenuLayer() {

}

void MenuLayer::Update(uint32_t ticks) {

}

void MenuLayer::Draw(SDL_Renderer* renderer) {
    if (ImGui::Begin("Main Menu")) {
        if (ImGui::Button("Play")) {
            auto gameLayer = std::make_unique<GameLayer>(renderer);
            m_layerStack->PushLayer(std::move(gameLayer));
            m_layerStack->RemoveLayer(this);
        }
    }
    ImGui::End();
}

void MenuLayer::OnAddedToStack(LayerStack* layerStack) {
    m_layerStack = layerStack;
}
void MenuLayer::OnRemovedFromStack() {
    m_layerStack = nullptr;
}
