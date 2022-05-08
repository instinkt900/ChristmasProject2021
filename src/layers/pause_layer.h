#pragma once

#include "ui_layer.h"

class GameLayer;

class PauseLayer : public UILayer {
public:
    PauseLayer(GameLayer& gameLayer);
    virtual ~PauseLayer() = default;

    bool OnEvent(moth_ui::Event const& event) override;
    void OnAddedToStack(LayerStack* layerStack) override;

private:
    GameLayer& m_gameLayer;

    enum class LeaveDest {
        Resume,
        Menu,
    };
    LeaveDest m_leaveDest = LeaveDest::Resume;

    bool OnAnimationEvent(moth_ui::EventAnimation const& event);
};
