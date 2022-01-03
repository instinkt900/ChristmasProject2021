#include "game_pch.h"
#include "layer_stack.h"
#include "layer.h"

LayerStack::LayerStack(int width, int height)
    : m_width(width)
    , m_height(height) {
}

LayerStack::~LayerStack() {
}

void LayerStack::PushLayer(std::unique_ptr<Layer>&& layer) {
    m_layers.push_back(std::move(layer));
    m_layers.back()->OnAddedToStack(this);
}

std::unique_ptr<Layer> LayerStack::PopLayer() {
    auto oldLayer = std::move(m_layers.back());
    m_layers.pop_back();
    oldLayer->OnRemovedFromStack();
    return oldLayer;
}

void LayerStack::RemoveLayer(Layer* layer) {
    auto it = std::find_if(std::begin(m_layers), std::end(m_layers), [layer](auto& candidate) {
        return candidate.get() == layer;
    });
    if (std::end(m_layers) != it) {
        (*it)->OnRemovedFromStack();
        m_layers.erase(it);
    }
}

bool LayerStack::OnEvent(Event const& event) {
    for (auto it = m_layers.rbegin(); it != m_layers.rend(); ++it) {
        if ((*it)->OnEvent(event)) {
            return true;
        }
    }
    return false;
}

void LayerStack::Update(uint32_t ticks) {
    for (auto&& layer : m_layers) {
        layer->Update(ticks);
    }
}

void LayerStack::Draw(SDL_Renderer& renderer) {
    for (auto&& layer : m_layers) {
        layer->Draw(renderer);
    }
}
