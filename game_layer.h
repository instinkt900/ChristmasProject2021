#pragma once

#include <entt/entt.hpp>
#include "layer.h"
#include "tile_map.h"
#include <tuple>

enum class GameState {
    None,
    PreGame,
    Game,
    PostGame
};

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

    void StateTransition(GameState newState);

private:
    SDL_Renderer* m_renderer = nullptr;
    LayerStack* m_layerStack = nullptr;

    GameState m_currentState = GameState::None;

    uint32_t m_pregameTimer = 0;

    entt::registry m_registry;
    entt::entity m_playerEntity = entt::null;
    entt::entity m_cameraEntity = entt::null;

    std::unique_ptr<TileMap> m_tileMap;
    SDL_Texture* m_backgroundTexture = nullptr;

    SDL_Texture* m_countDownText[3] = { nullptr, nullptr, nullptr };
    std::tuple<int, int> m_countDownTextDim[3];
    SDL_Texture* m_gameOverText = nullptr;
    std::tuple<int, int> m_gameOverTextDim;

    enum class ControlKey {
        Up,
        Down,
        Left,
        Right,
        Fire
    };

    std::map<ControlKey, bool> m_controlState;

    void Setup();
    int GetLayerWidth() const;
    int GetLayerHeight() const;

    void DestroySprite(entt::registry& registry, entt::entity entity);
};