#pragma once

class LayerStack;
struct RenderContext;

class Layer {
public:
    Layer();
    virtual ~Layer();

    virtual bool OnEvent(SDL_Event const& event);
    virtual void Update(uint32_t ticks);
    virtual void Draw(SDL_Renderer& renderer);

    virtual void OnAddedToStack(LayerStack* layerStack);
    virtual void OnRemovedFromStack();

    int GetWidth() const;
    int GetHeight() const;

protected:
    LayerStack* m_layerStack = nullptr;
};
