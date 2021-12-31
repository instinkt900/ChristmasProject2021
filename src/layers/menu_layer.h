#pragma once

#include "layer.h"

struct RenderContext;

class MenuLayer : public Layer {
public:
    MenuLayer(SDL_Renderer& renderer);
    virtual ~MenuLayer();

    bool OnEvent(SDL_Event const& event) override;
    void Update(uint32_t ticks) override;
    void Draw(SDL_Renderer& renderer) override;

private:
    SDL_Renderer& m_renderer;

    TextureRef m_splashTexture;
    TextureRef m_titleText;
    TextureRef m_titleTextDrop;
    TextureRef m_promptText;
    TextureRef m_promptTextDrop;

    std::tuple<int, int> m_titleTextDim;
    std::tuple<int, int> m_promptTextDim;
};
