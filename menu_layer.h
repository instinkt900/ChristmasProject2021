#pragma once

#include "layer.h"

class MenuLayer : public Layer
{
public:
    MenuLayer();
    virtual ~MenuLayer();

    void Update(uint32_t ticks) override;
    void Draw(SDL_Renderer* renderer) override;

    void OnAddedToStack(LayerStack* layerStack) override;
    void OnRemovedFromStack() override;

private:
    LayerStack* m_layerStack = nullptr;
};
