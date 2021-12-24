#pragma once

#include <entt/entt.hpp>
#include "layer.h"
#include "tile_map.h"

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
    SDL_Renderer* m_renderer = nullptr;
    LayerStack* m_layerStack = nullptr;
    entt::registry m_registry;

    SDL_Texture* m_backgroundTexture = nullptr;

    std::unique_ptr<TileMap> m_tileMap;

    void Setup();
    int GetLayerWidth() const;
    int GetLayerHeight() const;

    void DestroySprite(entt::registry& registry, entt::entity entity);
};