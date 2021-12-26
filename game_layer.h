#pragma once

#include <entt/entt.hpp>
#include "layer.h"
#include "tile_map.h"
#include <tuple>
#include "state_machine.h"
#include "random.h"

struct WorldParameters
{
    uint32_t const m_seed = 0xF00DF00D;

    std::string const m_playerSpritePath = "ship003.png";
    int const m_playerSpriteWidth = 62;
    int const m_playerSpriteHeight = 32;
    int const m_playerCollisionWidth = 50;
    int const m_playerCollisionHeight = 19;
    std::string const m_playerBulletSpritePath = "laser.png";
    int const m_playerBulletSpriteWidth = 42;
    int const m_playerBulletSpriteHeight = 40;
    int const m_playerBulletCollisionWidth = 15;
    int const m_playerBulletCollisionHeight = 15;
    float const m_playerMoveSpeed = 150.0f;
    uint32_t const m_playerFireDelay = 300;
    float const m_playerBulletSpeed = 600.0f;

    std::string const m_enemySpritePath = "l0_SpaceShip0011.png";
    int const m_enemySpriteWidth = 32;
    int const m_enemySpriteHeight = 32;
    int const m_enemyCollisionWidth = 32;
    int const m_enemyCollisionHeight = 32;
    uint32_t m_enemySpawnDelayMin = 3000;
    uint32_t m_enemySpawnDelayMax = 6000;
    uint32_t m_enemySpawnDelayDecrease = 100;
    uint32_t m_enemySpawnTimer = 0;
    float m_enemyCurrentSpeed = 100.0f;
    float m_enemySpeedIncrease = 5.0f;
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

    SDL_Renderer& GetRenderer() const { return *m_renderer; }
    int GetLayerWidth() const;
    int GetLayerHeight() const;

    auto& GetRandom() { return m_random; }
    auto& GetWorldParameters() { return m_worldParameters; }
    auto& GetRegistry() { return m_registry; }
    auto GetPlayerEntity() const { return m_playerEntity; }
    auto GetCameraEntity() const { return m_cameraEntity; }
    auto& GetTileMap() const { return *m_tileMap; }

    void SetupLevel();

private:
    SDL_Renderer* m_renderer = nullptr;
    LayerStack* m_layerStack = nullptr;
    StateMachine m_stateMachine;

    WorldParameters m_worldParameters;
    Random m_random;
    entt::registry m_registry;
    entt::entity m_playerEntity = entt::null;
    entt::entity m_cameraEntity = entt::null;

    std::unique_ptr<TileMap> m_tileMap;
    SDL_Texture* m_backgroundTexture = nullptr;

    void DestroySprite(entt::registry& registry, entt::entity entity);
};