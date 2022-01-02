#pragma once

#include "layer.h"

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
};
