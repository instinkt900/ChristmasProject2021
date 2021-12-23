#pragma once

#include <entt/entt.hpp>
#include "layer.h"
#include "input_system.h"

class GameLayer : public Layer
{
public:
    GameLayer(SDL_Renderer* renderer);
    ~GameLayer();

    bool OnEvent(SDL_Event& event) override;
    void Update(uint32_t ticks) override;
    void Draw(SDL_Renderer* renderer) override;

    void OnAddedToStack(LayerStack* layerStack) override;
    void OnRemovedFromStack() override;

private:
    LayerStack* m_layerStack;
    entt::registry m_registry;
    InputSystem m_inputSystem;

    entt::entity m_cameraEntity = entt::null;

    void Setup(SDL_Renderer* renderer);
    int GetLayerWidth() const;
    int GetLayerHeight() const;
};