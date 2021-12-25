#include "enemy_system.h"
#include <random>
#include "components.h"
#include <SDL_image.h>
#include "tile_map.h"

namespace EnemySystem {
    void Update(uint32_t ticks, entt::registry& registry, SDL_Renderer* renderer, TileMap& tileMap) {
        std::vector<entt::entity> deadEntities;
        registry.view<EnemyComponent>().each([&deadEntities](auto entity, auto const& enemyComponent) {
            if (enemyComponent.dead) {
                deadEntities.push_back(entity);
            }
        });

        for (auto entity : deadEntities) {
            registry.destroy(entity);
        }

        auto spawnerBrainView = registry.view<EnemySpawnerComponent>();
        if (spawnerBrainView.empty()) {
            return;
        }
        auto& enemySpawner = spawnerBrainView.get<EnemySpawnerComponent>(*spawnerBrainView.begin());

        if (ticks > enemySpawner.spawn_delay) {
            int spawnX = 0;
            int spawnY = 0;
            int spawnYMin = 0;
            int spawnYMax = 0;

            // find the active camera
            auto camerasView = registry.view<CameraComponent, PositionComponent>();
            for (auto cameraEntity : camerasView) {
                auto const& cameraComponent = camerasView.get<CameraComponent>(cameraEntity);
                if (cameraComponent.active) {
                    auto const& positionComponent = camerasView.get<PositionComponent>(cameraEntity);
                    spawnX = static_cast<int>(positionComponent.x) + static_cast<int>(cameraComponent.width / 2);
                    spawnYMin = static_cast<int>(positionComponent.y) - static_cast<int>(cameraComponent.height / 2);
                    spawnYMax = static_cast<int>(positionComponent.y) + static_cast<int>(cameraComponent.height / 2);
                    break;
                }
            }

            int const collisionWidth = 32;
            int const collisionHeight = 32;

            if (tileMap.Collides(spawnX, spawnY, collisionWidth, collisionHeight)) {
                return;
            }

            auto enemy = registry.create();
            auto& positionComponent = registry.emplace<PositionComponent>(enemy);
            positionComponent.x = static_cast<float>(spawnX);
            positionComponent.y = static_cast<float>(spawnY);

            auto& sprite = registry.emplace<SpriteComponent>(enemy);
            SDL_Surface* image = IMG_Load("l0_SpaceShip0011.png");
            sprite.texture = SDL_CreateTextureFromSurface(renderer, image);
            sprite.width = collisionWidth;
            sprite.height = collisionHeight;
            SDL_FreeSurface(image);

            auto& collisionComponent = registry.emplace<CollisionComponent>(enemy);
            collisionComponent.width = 10;
            collisionComponent.height = 10;
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
                    spriteComponent.flip = spriteComponent.flip == SDL_FLIP_NONE ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE;
                }
            };

            registry.emplace<EnemyComponent>(enemy);

            auto& velocityComponent = registry.emplace<VelocityComponent>(enemy);
            velocityComponent.y = -enemySpawner.speed;

            enemySpawner.spawn_delay = 6000;
        }
        else {
            enemySpawner.spawn_delay -= ticks;
        }

        enemySpawner.speed += enemySpawner.speed_increase * ticks / 1000.0f;
    }
}

