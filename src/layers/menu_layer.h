#pragma once

#include "layer.h"
#include "ui/ui_fwd.h"

class Game;
class EventAnimation;

class MenuLayer : public Layer {
public:
    MenuLayer(Game& game);
    virtual ~MenuLayer();

    bool OnEvent(Event const& event) override;
    void Update(uint32_t ticks) override;
    void Draw(SDL_Renderer& renderer) override;
    void DebugDraw() override;

    void OnAddedToStack(LayerStack* stack) override;

private:
    Game& m_game;
    std::unique_ptr<ui::Node> m_rootWidget;

    bool OnAnimEvent(EventAnimation const& event);
};
