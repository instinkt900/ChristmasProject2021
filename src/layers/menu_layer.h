#pragma once

#include "ui_layer.h"

class Game;

class MenuLayer : public UILayer {
public:
    MenuLayer(Game& game);
    virtual ~MenuLayer() = default;

    void DebugDraw() override;

    void OnAddedToStack(LayerStack* stack) override;

private:
    Game& m_game;
};
