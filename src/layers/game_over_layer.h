#pragma once

#include "ui_layer.h"

class Game;

class GameOverLayer : public UILayer {
public:
    GameOverLayer(Game& game);
    virtual ~GameOverLayer() = default;

    void OnAddedToStack(LayerStack* stack) override;

    void SetNewHighScore(int score);

private:
    Game& m_game;
};
