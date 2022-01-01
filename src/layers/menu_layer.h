#pragma once

#include "layer.h"

class AudioFactory;

class MenuLayer : public Layer {
public:
    MenuLayer(SDL_Renderer& renderer, AudioFactory& audioFactory);
    virtual ~MenuLayer();

    bool OnEvent(SDL_Event const& event) override;
    void Update(uint32_t ticks) override;
    void Draw(SDL_Renderer& renderer) override;

private:
    SDL_Renderer& m_renderer;
    AudioFactory& m_audioFactory;

    TextureRef m_splashTexture;
    TextureRef m_titleText;
    TextureRef m_titleTextDrop;
    TextureRef m_promptText;
    TextureRef m_promptTextDrop;

    IntVec2 m_titleTextDim;
    IntVec2 m_promptTextDim;
};
