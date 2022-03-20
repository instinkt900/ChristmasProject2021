#pragma once

#include "layer.h"
#include "moth_ui/ui_fwd.h"

class Game;

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
    std::unique_ptr<moth_ui::Node> m_rootWidget;

    //bool OnAnimEvent(EventAnimation const& event);
};
