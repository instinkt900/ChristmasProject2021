#pragma once

#include "ui_layer.h"

class AudioFactory;
class Game;

class LoadingLayer : public UILayer {
public:
    LoadingLayer(Game& game);
    virtual ~LoadingLayer() = default;

    void Update(uint32_t ticks) override;

    void OnAddedToStack(LayerStack* layerStack) override;

private:
    Game& m_game;

    bool m_loadingFinished = false;
};
