#pragma once

#include <entt/entt.hpp>
#include "layer.h"
#include "tile_map.h"
#include <tuple>
#include "state_machine.h"

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

    SDL_Renderer& GetRenderer() const { return *m_renderer; }
    int GetLayerWidth() const;
    int GetLayerHeight() const;

    auto& GetRegistry() { return m_registry; }
    auto GetPlayerEntity() const { return m_playerEntity; }
    auto GetCameraEntity() const { return m_cameraEntity; }

    void SetupLevel();

private:
    SDL_Renderer* m_renderer = nullptr;
    LayerStack* m_layerStack = nullptr;
    StateMachine m_stateMachine;

    entt::registry m_registry;
    entt::entity m_playerEntity = entt::null;
    entt::entity m_cameraEntity = entt::null;

    std::unique_ptr<TileMap> m_tileMap;
    SDL_Texture* m_backgroundTexture = nullptr;

    void DestroySprite(entt::registry& registry, entt::entity entity);
};