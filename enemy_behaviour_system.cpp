#include "enemy_behaviour_system.h"

#include "components.h"

namespace EnemyBehaviourSystem {
    void Update(uint32_t ticks, entt::registry& registry) {
        registry.view<EnemyComponent, CollisionComponent, VelocityComponent>().each([](auto& collisionComponent, auto& velocityComponent) {
            if (collisionComponent.tilemap_collision) {
                velocityComponent.y = -velocityComponent.y;
                collisionComponent.tilemap_collision = false;
            }
        });
    }
}
