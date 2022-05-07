#pragma once

class LayerStack;

class Layer : public moth_ui::EventListener {
public:
    Layer();
    virtual ~Layer();

    bool OnEvent(moth_ui::Event const& event) override;

    virtual void Update(uint32_t ticks);
    virtual void Draw(SDL_Renderer& renderer);
    virtual void DebugDraw();

    virtual void OnAddedToStack(LayerStack* layerStack);
    virtual void OnRemovedFromStack();

    int GetWidth() const;
    int GetHeight() const;

    virtual bool UseRenderSize() const { return true; }

    LayerStack* GetLayerStack() const { return m_layerStack; }

protected:
    LayerStack* m_layerStack = nullptr;
};
