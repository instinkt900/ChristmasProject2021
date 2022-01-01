#pragma once

#include "layer.h"
#include "states/state_machine.h"
#include "random.h"

class TileMap;

struct WorldParameters {
    uint32_t m_seed = 0xF00DF00D;

    std::string m_dataFile = "data.bin";
    std::string m_gameFontPath = "pilotcommand.ttf";

    float m_levelSpeedInit = 100.0f;
    float m_levelSpeedIncrease = 1.0f;

    std::string m_playerSpritePath = "ship003.png";
    int m_playerSpriteWidth = 62;
    int m_playerSpriteHeight = 32;
    int m_playerCollisionWidth = 50;
    int m_playerCollisionHeight = 19;
    std::string m_playerBulletSpritePath = "laser.png";
    int m_playerBulletSpriteWidth = 42;
    int m_playerBulletSpriteHeight = 40;
    int m_playerBulletCollisionWidth = 15;
    int m_playerBulletCollisionHeight = 15;
    float m_playerMoveSpeed = 110.0f;
    uint32_t m_playerFireDelay = 300;
    float m_playerBulletSpeed = 600.0f;

    std::string m_enemySpritePath = "l0_SpaceShip0011.png";
    int m_enemySpriteWidth = 32;
    int m_enemySpriteHeight = 32;
    int m_enemyCollisionWidth = 32;
    int m_enemyCollisionHeight = 32;

    int m_enemySpawnDelayMinInit = 6000;
    int m_enemySpawnDelayMaxInit = 8000;
    float m_enemySpawnDelayDecrease = 50.0f;
    float m_enemySpeedInit = 100.0f;
    float m_enemySpeedIncrease = 3.0f;

    int m_scorePerSecond = 100;
    int m_scorePerKill = 5000;
    int m_scoreShootPenalty = 10;
};

struct WorldState {
    float m_levelSpeed = 0.0f;

    float m_enemySpawnDelayMin = 0.0f;
    float m_enemySpawnDelayMax = 0.0f;

    uint32_t m_enemySpawnTimer = 0;
    float m_enemyCurrentSpeed = 0.0f;

    int m_score = 0;
    int m_highScore = 0;
};

class AudioFactory;

class GameLayer : public Layer {
public:
    GameLayer(SDL_Renderer& renderer, AudioFactory& audioFactory);
    virtual ~GameLayer();

    bool OnEvent(SDL_Event const& event) override;
    void Update(uint32_t ticks) override;
    void Draw(SDL_Renderer& renderer) override;

    void OnAddedToStack(LayerStack* layerStack) override;
    void OnRemovedFromStack() override;

    SDL_Renderer& GetRenderer() const { return m_renderer; }
    AudioFactory& GetAudioFactory() const { return m_audioFactory; }

    auto& GetRandom() { return m_random; }
    auto& GetWorldParameters() const { return m_worldParameters; }
    auto& GetWorldState() { return m_worldState; }
    auto& GetRegistry() { return m_registry; }
    auto GetPlayerEntity() const { return m_playerEntity; }
    auto GetCameraEntity() const { return m_cameraEntity; }
    auto& GetTileMap() const { return *m_tileMap; }

    entt::entity SpawnCamera(int x, int y);
    entt::entity SpawnPlayer(int x, int y);
    entt::entity SpawnExplosion(int x, int y, bool playSound);

    void SetupLevel();
    void SaveScore();

private:
    void LoadScore();

    SDL_Renderer& m_renderer;
    AudioFactory& m_audioFactory;
    StateMachine m_stateMachine;

    WorldParameters const m_worldParameters;
    WorldState m_worldState;
    Random m_random;

    entt::registry m_registry;
    entt::entity m_playerEntity = entt::null;
    entt::entity m_cameraEntity = entt::null;

    std::unique_ptr<TileMap> m_tileMap;

    TextureRef m_backgroundTexture;
    TextureRef m_explosionTexture;

    CachedFontRef m_scoreFont;
};