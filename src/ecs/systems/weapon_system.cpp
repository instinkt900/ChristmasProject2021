#include "game_pch.h"
#include "weapon_system.h"
#include "ecs/components/components.h"
#include "layers/game_layer.h"

namespace WeaponSystem {
    void Update(uint32_t ticks, GameLayer& gameLayer) {
        auto& renderer = gameLayer.GetRenderer();
        auto& registry = gameLayer.GetRegistry();
        auto const& worldParameters = gameLayer.GetWorldParameters();
        auto& worldState = gameLayer.GetWorldState();
        registry.view<WeaponComponent, PositionComponent>().each([ticks, &registry, &renderer, &worldParameters, &worldState, &gameLayer](auto& weaponComponent, auto const& positionComponent) {
            if (weaponComponent.timer > 0) {
                weaponComponent.timer -= ticks;
            }

            if (weaponComponent.timer <= 0 && weaponComponent.firing) {
                // spawn projectile
                auto projectile = registry.create();
                registry.emplace<HealthComponent>(projectile);
                auto& projectilePositionComponent = registry.emplace<PositionComponent>(projectile);
                auto& projectileVelocityComponent = registry.emplace<VelocityComponent>(projectile);
                auto& projectileSpriteComponent = registry.emplace<SpriteComponent>(projectile);
                auto& collisionComponent = registry.emplace<CollisionComponent>(projectile);
                projectilePositionComponent.x = positionComponent.x;
                projectilePositionComponent.y = positionComponent.y;
                projectileVelocityComponent.x = weaponComponent.velocity * (weaponComponent.facing_left ? -1.0f : 1.0f);
                projectileVelocityComponent.y = 0;
                projectileSpriteComponent.texture = CreateTextureRef(&renderer, worldParameters.m_playerBulletSpritePath.c_str());
                projectileSpriteComponent.width = worldParameters.m_playerBulletSpriteWidth;
                projectileSpriteComponent.height = worldParameters.m_playerBulletSpriteHeight;
                collisionComponent.width = worldParameters.m_playerBulletCollisionWidth;
                collisionComponent.height = worldParameters.m_playerBulletCollisionHeight;
                collisionComponent.flags = COLLISION_FLAG_BULLET;
                collisionComponent.flag_mask = COLLISION_FLAG_MAP | COLLISION_FLAG_ENEMY;
                collisionComponent.on_collision = [&registry, projectile](entt::entity otherEntity) {
                    auto& healthComponent = registry.get<HealthComponent>(projectile);
                    healthComponent.alive = false;
                };
                weaponComponent.timer = weaponComponent.fire_delay;
                Mix_PlayChannel(-1, gameLayer.GetWeaponSFX().get(), 0);

                worldState.m_score -= worldParameters.m_scoreShootPenalty;
            }
        });
    }
}
