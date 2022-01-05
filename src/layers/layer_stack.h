#pragma once

#include "events/event_listener.h"

class Layer;

class LayerStack : public EventListener {
public:
    LayerStack(int width, int height);
    ~LayerStack();

    void PushLayer(std::unique_ptr<Layer>&& layer);
    std::unique_ptr<Layer> PopLayer();
    void RemoveLayer(Layer* layer);

    bool OnEvent(Event const& event) override;
    void Update(uint32_t ticks);
    void Draw(SDL_Renderer& renderer);
    void DebugDraw();

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

private:
    std::vector<std::unique_ptr<Layer>> m_layers;

    int m_width = 0;
    int m_height = 0;
};