#pragma once

class Layer;

class LayerStack
{
public:
    LayerStack(int width, int height);
    ~LayerStack();

    void PushLayer(std::unique_ptr<Layer>&& layer);
    std::unique_ptr<Layer> PopLayer();
    void RemoveLayer(Layer* layer);

    void OnEvent(SDL_Event const& event);
    void Update(uint32_t ticks);
    void Draw(SDL_Renderer* renderer);

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

private:
    std::vector<std::unique_ptr<Layer>> m_layers;

    int m_width = 0;
    int m_height = 0;
};