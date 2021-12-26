#include "enemy_system.h"
#include <random>
#include "components.h"
#include <SDL_image.h>
#include "tile_map.h"
#include "game_layer.h"

namespace EnemySystem {
    void Update(uint32_t ticks, GameLayer& gameLayer) {
        auto& registry = gameLayer.GetRegistry();
        std::vector<entt::entity> deadEntities;
        registry.view<EnemyComponent>().each([&deadEntities](auto entity, auto const& enemyComponent) {
            if (enemyComponent.dead) {
                deadEntities.push_back(entity);
            }
        });

        for (auto entity : deadEntities) {
            registry.destroy(entity);
        }

        auto& worldParameters = gameLayer.GetWorldParameters();
        auto& worldState = gameLayer.GetWorldState();
        if (ticks > worldState.m_enemySpawnTimer) {
            auto cameraEntity = gameLayer.GetCameraEntity();
            auto const& cameraPositionComponent = registry.get<PositionComponent>(cameraEntity);
            auto const& cameraCameraComponent = registry.get<CameraComponent>(cameraEntity);
            int const spawnY = 0;
            int const spawnX = static_cast<int>(cameraPositionComponent.x) + static_cast<int>(cameraCameraComponent.width / 2);
            int const spawnYMin = static_cast<int>(cameraPositionComponent.y) - static_cast<int>(cameraCameraComponent.height / 2);
            int const spawnYMax = static_cast<int>(cameraPositionComponent.y) + static_cast<int>(cameraCameraComponent.height / 2);

            int const collisionWidth = 32;
            int const collisionHeight = 32;

            auto& tileMap = gameLayer.GetTileMap();
            if (tileMap.Collides(spawnX, spawnY, collisionWidth, collisionHeight)) {
                return;
            }

            auto enemy = registry.create();
            auto& positionComponent = registry.emplace<PositionComponent>(enemy);
            positionComponent.x = static_cast<float>(spawnX);
            positionComponent.y = static_cast<float>(spawnY);

            auto& renderer = gameLayer.GetRenderer();
            auto& sprite = registry.emplace<SpriteComponent>(enemy);
            SDL_Surface* image = IMG_Load(worldParameters.m_enemySpritePath.c_str());
            sprite.texture = SDL_CreateTextureFromSurface(&renderer, image);
            sprite.width = worldParameters.m_enemySpriteWidth;
            sprite.height = worldParameters.m_enemySpriteHeight;
            SDL_FreeSurface(image);

            auto& collisionComponent = registry.emplace<CollisionComponent>(enemy);
            collisionComponent.width = worldParameters.m_enemyCollisionWidth;
            collisionComponent.height = worldParameters.m_enemyCollisionHeight;
            collisionComponent.flags = COLLISION_FLAG_ENEMY;
            collisionComponent.flag_mask = COLLISION_FLAG_MAP | COLLISION_FLAG_PLAYER | COLLISION_FLAG_BULLET;
            collisionComponent.on_collision = [enemy, &registry](entt::entity otherEntity) {
                if (otherEntity != entt::null) {
                    auto& enemyComponent = registry.get<EnemyComponent>(enemy);
                    enemyComponent.dead = true;
                }
                else {
                    auto& velocityComponent = registry.get<VelocityComponent>(enemy);
                    velocityComponent.y = -velocityComponent.y;
                    auto& spriteComponent = registry.get<SpriteComponent>(enemy);
                    spriteComponent.flip = (spriteComponent.flip == SDL_FLIP_NONE) ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE;
                }
            };

            registry.emplace<EnemyComponent>(enemy);

            auto& velocityComponent = registry.emplace<VelocityComponent>(enemy);
            velocityComponent.y = -worldState.m_enemyCurrentSpeed;

            auto& random = gameLayer.GetRandom();
            worldState.m_enemySpawnTimer = static_cast<uint32_t>(random.Range(worldState.m_enemySpawnDelayMin, worldState.m_enemySpawnDelayMax));
        }
        else {
            worldState.m_enemySpawnTimer -= ticks;
        }

        float const seconds = ticks / 1000.0f;
        worldState.m_enemyCurrentSpeed += worldParameters.m_enemySpeedIncrease * seconds;
        worldState.m_enemySpawnDelayMin = std::max(0.0f, worldState.m_enemySpawnDelayMin - worldParameters.m_enemySpawnDelayDecrease * seconds);
        worldState.m_enemySpawnDelayMax = std::max(0.0f, worldState.m_enemySpawnDelayMax - worldParameters.m_enemySpawnDelayDecrease * seconds);
    }
}

