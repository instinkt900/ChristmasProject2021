#pragma once

#include "ui_layer.h"

class Game;

class SplashLayer : public UILayer {
public:
    SplashLayer(Game& game);
    virtual ~SplashLayer() = default;

    bool OnEvent(moth_ui::Event const& event) override;

    void OnAddedToStack(LayerStack* stack) override;

private:
    Game& m_game;

    bool OnKeyEvent(moth_ui::EventKey const& event);
};
