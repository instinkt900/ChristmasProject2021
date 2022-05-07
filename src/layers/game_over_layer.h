#pragma once

#include "layer.h"
#include "moth_ui/ui_fwd.h"

class Game;

class GameOverLayer : public Layer {
public:
    GameOverLayer(Game& game);
    virtual ~GameOverLayer() = default;

    bool OnEvent(moth_ui::Event const& event) override;
    void Update(uint32_t ticks) override;
    void Draw(SDL_Renderer& renderer) override;

    void OnAddedToStack(LayerStack* stack) override;
    void OnRemovedFromStack() override;

    void SetNewHighScore(int score);

private:
    Game& m_game;
    std::unique_ptr<moth_ui::Group> m_root;
};
