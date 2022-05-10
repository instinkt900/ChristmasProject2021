#pragma once

#include "ui_layer.h"

class Game;

class ScoresLayer : public UILayer {
public:
    ScoresLayer(Game& game);
    virtual ~ScoresLayer() = default;

    void DebugDraw() override;

    void OnAddedToStack(LayerStack* stack) override;

private:
    Game& m_game;

    bool OnUIEvent(moth_ui::Event const& event);
    bool OnAnimationEnded(moth_ui::EventAnimationStopped const& event);
};
