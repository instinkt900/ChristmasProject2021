#pragma once

#include "layer.h"

class Game;
class Node;

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
    std::unique_ptr<Node> m_rootWidget;
};
