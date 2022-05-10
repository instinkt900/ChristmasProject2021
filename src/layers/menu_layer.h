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

    enum class ExitDestination {
        Unknown,
        Play,
        Scores,
        Exit
    };
    ExitDestination m_exitDestination = ExitDestination::Unknown;

    bool OnUIEvent(moth_ui::Event const& event);
    bool OnAnimationEnd(moth_ui::EventAnimationStopped const& event);
};
