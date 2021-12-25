#include "velocity_system.h"

#include "components.h"
#include "tile_map.h"

namespace VelocitySystem {
    void Update(uint32_t ticks, entt::registry& registry, TileMap& tileMap) {
        float const seconds = ticks / 1000.0f;
        registry.view<VelocityComponent, PositionComponent>().each([seconds, &registry, &tileMap](auto entity, auto const& velocityComponent, auto& positionComponent) {
            if (auto collisionComponent = registry.try_get<CollisionComponent>(entity)) {
                float const newPositionX = positionComponent.x + velocityComponent.x * seconds;
                float const newPositionY = positionComponent.y + velocityComponent.y * seconds;
                if (tileMap.Collides(static_cast<int>(newPositionX), static_cast<int>(newPositionY), static_cast<int>(collisionComponent->width), static_cast<int>(collisionComponent->height))) {
                    collisionComponent->tilemap_collision = true;
                }
                else {
                    positionComponent.x = newPositionX;
                    positionComponent.y = newPositionY;
                }
            }
            else {
                positionComponent.x += velocityComponent.x * seconds;
                positionComponent.y += velocityComponent.y * seconds;
            }
        });
    }
}
