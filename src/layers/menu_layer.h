#pragma once

#include "layer.h"
#include "ui/widget.h"

class Game;

class MenuLayer : public Layer {
public:
    MenuLayer(Game& game);
    virtual ~MenuLayer();

    bool OnEvent(SDL_Event const& event) override;
    void Update(uint32_t ticks) override;
    void Draw(SDL_Renderer& renderer) override;

    void OnAddedToStack(LayerStack* stack) override;

private:
    Game& m_game;
    WidgetRef m_rootWidget;
};
