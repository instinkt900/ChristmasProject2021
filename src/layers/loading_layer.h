#pragma once

#include "layer.h"

class AudioFactory;

class LoadingLayer : public Layer {
public:
    LoadingLayer(SDL_Renderer& renderer, AudioFactory& audioFactory);
    virtual ~LoadingLayer();

    bool OnEvent(SDL_Event const& event) override;
    void Update(uint32_t ticks) override;
    void Draw(SDL_Renderer& renderer) override;

    void OnAddedToStack(LayerStack* layerStack) override;

private:
    SDL_Renderer& m_renderer;
    AudioFactory& m_audioFactory;

    TextureRef m_loadingTexture;
    TextureRef m_loadingText;
    TextureRef m_loadingTextDrop;
    IntVec2 m_loadingTextDim;

    bool m_loadingFinished = false;
};
