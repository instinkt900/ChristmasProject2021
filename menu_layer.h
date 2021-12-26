#pragma once

#include "layer.h"
#include <tuple>

class MenuLayer : public Layer
{
public:
    MenuLayer(SDL_Renderer* renderer);
    virtual ~MenuLayer();

    bool OnEvent(SDL_Event& event) override;
    void Update(uint32_t ticks) override;
    void Draw(SDL_Renderer* renderer) override;

    void OnAddedToStack(LayerStack* layerStack) override;
    void OnRemovedFromStack() override;

private:
    LayerStack* m_layerStack = nullptr;
    SDL_Renderer* m_renderer = nullptr;
    SDL_Texture* m_splashTexture = nullptr;

    SDL_Texture* m_titleText = nullptr;
    SDL_Texture* m_titleTextDrop = nullptr;
    SDL_Texture* m_promptText = nullptr;
    SDL_Texture* m_promptTextDrop = nullptr;
    std::tuple<int, int> m_titleTextDim;
    std::tuple<int, int> m_promptTextDim;
};
