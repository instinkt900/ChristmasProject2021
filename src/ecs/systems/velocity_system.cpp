#include "game_pch.h"
#include "velocity_system.h"
#include "tile_map.h"
#include "layers/game_layer.h"
#include "ecs/components/components.h"

namespace {
    bool TestOverlap(int collider1X, int collider1Y, int collider1Width, int collider1Height,
                     int collider2X, int collider2Y, int collider2Width, int collider2Height) {
        int const left1 = collider1X - collider1Width / 2;
        int const right1 = collider1X + collider1Width / 2;
        int const top1 = collider1Y - collider1Height / 2;
        int const bottom1 = collider1Y + collider1Height / 2;

        int const left2 = collider2X - collider2Width / 2;
        int const right2 = collider2X + collider2Width / 2;
        int const top2 = collider2Y - collider2Height / 2;
        int const bottom2 = collider2Y + collider2Height / 2;

        if (left1 > right2 || right1 < left2)
            return false;
        if (top1 > bottom2 || bottom1 < top2)
            return false;
        return true;
    }
}
namespace VelocitySystem {
    void Update(uint32_t ticks, GameLayer& gameLayer) {
        float const seconds = ticks / 1000.0f;

        auto& registry = gameLayer.GetRegistry();
        auto& tileMap = gameLayer.GetTileMap();

        // first update anything without collision
        registry.view<VelocityComponent, PositionComponent>(entt::exclude<CollisionComponent>).each([seconds](auto const& velocityComponent, auto& positionComponent) {
            positionComponent.x += velocityComponent.x * seconds;
            positionComponent.y += velocityComponent.y * seconds;
        });

        // then anything with collisions
        std::vector<entt::entity> previousEntities;
        registry.view<VelocityComponent, PositionComponent, CollisionComponent>().each([seconds, &registry, &tileMap, &previousEntities](auto entity, auto const& velocityComponent, auto& positionComponent, auto& collisionComponent) {
            if (auto healthComponent = registry.try_get<HealthComponent>(entity)) {
                if (!healthComponent->alive) {
                    return;
                }
            }

            float const newPositionX = positionComponent.x + velocityComponent.x * seconds;
            float const newPositionY = positionComponent.y + velocityComponent.y * seconds;
            bool move = true;

            PositionComponent newPositionComponent;
            newPositionComponent.x = newPositionX;
            newPositionComponent.y = newPositionY;
            newPositionComponent.parent = positionComponent.parent;
            auto const hostPosition = ResolvePosition(registry, newPositionComponent);

            int const colliderX = static_cast<int>(hostPosition.x);
            int const colliderY = static_cast<int>(hostPosition.y);
            int const colliderWidth = static_cast<int>(collisionComponent.width);
            int const colliderHeight = static_cast<int>(collisionComponent.height);

            collisionComponent.collision_bits = 0;

            if ((collisionComponent.flag_mask & COLLISION_FLAG_MAP) != 0) {
                if (tileMap.Collides(colliderX, colliderY, colliderWidth, colliderHeight)) {
                    collisionComponent.tilemap_collision = true;
                    move = false;
                    if (collisionComponent.on_collision) {
                        collisionComponent.on_collision(entt::null);
                    }
                }
            }

            if ((collisionComponent.flag_mask & (COLLISION_FLAG_PLAYER | COLLISION_FLAG_ENEMY | COLLISION_FLAG_BULLET)) != 0) {
                for (auto otherEntity : previousEntities) {
                    if (!registry.valid(otherEntity)) {
                        continue;
                    }
                    auto& otherCollisionComponent = registry.get<CollisionComponent>(otherEntity);

                    if ((otherCollisionComponent.flags & collisionComponent.flag_mask) != 0) {
                        auto const otherPosition = ResolvePosition(registry, otherEntity);
                        int const otherColliderX = static_cast<int>(otherPosition.x);
                        int const otherColliderY = static_cast<int>(otherPosition.y);
                        int const otherColliderWidth = static_cast<int>(otherCollisionComponent.width);
                        int const otherColliderHeight = static_cast<int>(otherCollisionComponent.height);

                        if (TestOverlap(colliderX, colliderY, colliderWidth, colliderHeight,
                                        otherColliderX, otherColliderY, otherColliderWidth, otherColliderHeight)) {
                            collisionComponent.collision_bits |= otherCollisionComponent.flags;
                            otherCollisionComponent.collision_bits |= collisionComponent.flags;
                            if (collisionComponent.on_collision) {
                                collisionComponent.on_collision(otherEntity);
                            }
                            if (otherCollisionComponent.on_collision) {
                                otherCollisionComponent.on_collision(entity);
                            }
                        }
                    }
                }
            }

            if (move) {
                positionComponent.x = newPositionX;
                positionComponent.y = newPositionY;
            }

            previousEntities.push_back(entity);
        });
    }
}
