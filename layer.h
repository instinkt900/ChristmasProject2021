#pragma once

#include <SDL.h>

class LayerStack;

class Layer
{
public:
    Layer() {}
    virtual ~Layer() {}

    virtual bool OnEvent(SDL_Event& event) { return false; }
    virtual void Update(uint32_t ticks) {}
    virtual void Draw(SDL_Renderer* renderer) {}

    virtual void OnAddedToStack(LayerStack* layerStack) {}
    virtual void OnRemovedFromStack() {}
};
