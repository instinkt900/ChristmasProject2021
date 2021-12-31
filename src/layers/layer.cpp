#include "game_pch.h"
#include "layer.h"
#include "layer_stack.h"

Layer::Layer() {
}

Layer::~Layer() {
}

bool Layer::OnEvent(SDL_Event const& event) {
    return false;
}

void Layer::Update(uint32_t ticks) {
}

void Layer::Draw(SDL_Renderer& renderer) {
}

void Layer::OnAddedToStack(LayerStack* layerStack) {
    m_layerStack = layerStack;
}

void Layer::OnRemovedFromStack() {
    m_layerStack = nullptr;
}

int Layer::GetWidth() const {
    if (nullptr == m_layerStack) {
        return 0;
    }
    return m_layerStack->GetWidth();
}

int Layer::GetHeight() const {
    if (nullptr == m_layerStack) {
        return 0;
    }
    return m_layerStack->GetHeight();
}
