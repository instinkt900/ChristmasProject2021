#include "game_pch.h"
#include "layer_stack.h"
#include "layer.h"

LayerStack::LayerStack(int width, int height)
:m_width(width)
,m_height(height) {

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

void LayerStack::OnEvent(SDL_Event const& event) {
    if (event.type == SDL_WINDOWEVENT) {
        switch (event.window.event) {
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            m_width = event.window.data1;
            m_height = event.window.data2;
            break;
        }
    }
    for (auto it = m_layers.rbegin(); it != m_layers.rend(); ++it) {
        if ((*it)->OnEvent(event)) {
            break;
        }
    }
}

void LayerStack::Update(uint32_t ticks) {
    for (auto&& layer : m_layers) {
        layer->Update(ticks);
    }
}

void LayerStack::Draw(SDL_Renderer* renderer) {
    for (auto&& layer : m_layers) {
        layer->Draw(renderer);
    }
}
