#include "game_pch.h"
#include "layer_stack.h"
#include "layer.h"

LayerStack::LayerStack(int renderWidth, int renderHeight, int windowWidth, int windowHeight)
    : m_renderWidth(renderWidth)
    , m_renderHeight(renderHeight)
    , m_windowWidth(windowWidth)
    , m_windowHeight(windowHeight) {
}

LayerStack::~LayerStack() {
}

void LayerStack::PushLayer(std::shared_ptr<Layer>&& layer) {
    m_layers.push_back(std::move(layer));
    m_layers.back()->OnAddedToStack(this);
}

std::shared_ptr<Layer> LayerStack::PopLayer() {
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

void LayerStack::ClearLayers() {
    while (!m_layers.empty()) {
        PopLayer();
    }
}

bool LayerStack::OnEvent(moth_ui::Event const& event) {
    float const scaleX = m_renderWidth / static_cast<float>(m_windowWidth);
    float const scaleY = m_renderHeight / static_cast<float>(m_windowHeight);
    std::shared_ptr<moth_ui::Event> translatedEvent;
    if (auto mouseEvent = moth_ui::event_cast<moth_ui::EventMouseDown>(event)) {
        moth_ui::IntVec2 translatedPosition = mouseEvent->GetPosition();
        translatedPosition.x = static_cast<int>(translatedPosition.x * scaleX);
        translatedPosition.y = static_cast<int>(translatedPosition.y * scaleY);
        translatedEvent = std::make_shared<moth_ui::EventMouseDown>(mouseEvent->GetButton(), translatedPosition);
    } else if (auto mouseEvent = moth_ui::event_cast<moth_ui::EventMouseUp>(event)) {
        moth_ui::IntVec2 translatedPosition = mouseEvent->GetPosition();
        translatedPosition.x = static_cast<int>(translatedPosition.x * scaleX);
        translatedPosition.y = static_cast<int>(translatedPosition.y * scaleY);
        translatedEvent = std::make_shared<moth_ui::EventMouseUp>(mouseEvent->GetButton(), translatedPosition);
    } else if (auto mouseEvent = moth_ui::event_cast<moth_ui::EventMouseMove>(event)) {
        moth_ui::IntVec2 translatedPosition = mouseEvent->GetPosition();
        moth_ui::FloatVec2 translatedDelta = mouseEvent->GetDelta();
        translatedPosition.x = static_cast<int>(translatedPosition.x * scaleX);
        translatedPosition.y = static_cast<int>(translatedPosition.y * scaleY);
        translatedDelta.x *= scaleX;
        translatedDelta.y *= scaleY;
        translatedEvent = std::make_shared<moth_ui::EventMouseMove>(translatedPosition, translatedDelta);
    }

    for (auto it = m_layers.rbegin(); it != m_layers.rend(); ++it) {
        auto& layer = *it;
        if (layer->UseRenderSize() && translatedEvent) {
            if ((*it)->OnEvent(*translatedEvent)) {
                return true;
            }
        } else {
            if ((*it)->OnEvent(event)) {
                return true;
            }
        }
    }
    return false;
}

void LayerStack::Update(uint32_t ticks) {
    auto layersCopy = m_layers;
    for (auto&& layer : layersCopy) {
        layer->Update(ticks);
    }
}

void LayerStack::Draw(SDL_Renderer& renderer) {
    for (auto&& layer : m_layers) {
        if (layer->UseRenderSize()) {
            SDL_RenderSetLogicalSize(&renderer, m_renderWidth, m_renderHeight);
        } else {
            SDL_RenderSetLogicalSize(&renderer, m_windowWidth, m_windowHeight);
        }
        layer->Draw(renderer);
    }
}

void LayerStack::DebugDraw() {
    if (ImGui::Begin("Layer Stack")) {
        for (auto&& layer : m_layers) {
            layer->DebugDraw();
        }
    }
    ImGui::End();
}