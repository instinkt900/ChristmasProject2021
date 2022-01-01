#pragma once

#include "layer.h"

class AudioFactory;
class Game;

class MenuLayer : public Layer {
public:
    MenuLayer(Game& game);
    virtual ~MenuLayer();

    bool OnEvent(SDL_Event const& event) override;
    void Update(uint32_t ticks) override;
    void Draw(SDL_Renderer& renderer) override;

private:
    Game& m_game;

    TextureRef m_splashTexture;
    TextureRef m_titleText;
    TextureRef m_titleTextDrop;
    TextureRef m_promptText;
    TextureRef m_promptTextDrop;

    IntVec2 m_titleTextDim;
    IntVec2 m_promptTextDim;
};
