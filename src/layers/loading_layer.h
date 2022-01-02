#pragma once

#include "layer.h"

class AudioFactory;
class Game;

class LoadingLayer : public Layer {
public:
    LoadingLayer(Game& game);
    virtual ~LoadingLayer();

    bool OnEvent(SDL_Event const& event) override;
    void Update(uint32_t ticks) override;
    void Draw(SDL_Renderer& renderer) override;

    void OnAddedToStack(LayerStack* layerStack) override;

private:
    Game& m_game;

    TextureRef m_loadingText;
    TextureRef m_loadingTextDrop;
    IntVec2 m_loadingTextDim;

    bool m_loadingFinished = false;
};
