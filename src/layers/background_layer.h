#pragma once

#include "layer.h"

class BackgroundLayer : public Layer {
public:
    BackgroundLayer(SDL_Renderer& renderer);
    virtual ~BackgroundLayer();

    void Draw(SDL_Renderer& renderer) override;
    void DebugDraw() override;

private:
    TextureRef m_backgroundTexture;
};
