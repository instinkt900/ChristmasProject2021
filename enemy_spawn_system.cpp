#include "enemy_spawn_system.h"
#include <random>
#include "components.h"
#include <SDL_image.h>

namespace EnemySpawnSystem {
    uint32_t SpawnDelay = 0;

    void Update(uint32_t ticks, entt::registry& registry, SDL_Renderer* renderer) {
        if (ticks > SpawnDelay) {
            int spawnX = 0;
            int spawnYMin = 0;
            int spawnYMax = 0;
            bool cameraFound = false;
            registry.view<CameraComponent, PositionComponent>().each([&spawnX, &spawnYMin, &spawnYMax, &cameraFound](auto const& cameraComponent, auto const& positionComponent) {
                if (!cameraFound && cameraComponent.active) {
                    spawnX = static_cast<int>(positionComponent.x) + static_cast<int>(cameraComponent.width / 2);
                    spawnYMin = static_cast<int>(positionComponent.y) - static_cast<int>(cameraComponent.height / 2);
                    spawnYMax = static_cast<int>(positionComponent.y) + static_cast<int>(cameraComponent.height / 2);
                    cameraFound = true;
                }
                });

            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> distr(spawnYMin, spawnYMax);
            int spawnY = distr(gen);

            auto enemy = registry.create();
            auto& positionComponent = registry.emplace<PositionComponent>(enemy);
            positionComponent.x = static_cast<float>(spawnX);
            positionComponent.y = static_cast<float>(spawnY);

            auto& sprite = registry.emplace<SpriteComponent>(enemy);
            SDL_Surface* image = IMG_Load("dot.png");
            sprite.texture = SDL_CreateTextureFromSurface(renderer, image);
            sprite.width = 10;
            sprite.height = 10;
            SDL_FreeSurface(image);

            auto& velocityComponent = registry.emplace<VelocityComponent>(enemy);
            velocityComponent.x = -200.0f;

            auto& lifetimeComponent = registry.emplace<LifetimeComponent>(enemy);
            lifetimeComponent.lifetime = 3500;

            SpawnDelay = 3000;
        }
        else {
            SpawnDelay -= ticks;
        }
    }
}

