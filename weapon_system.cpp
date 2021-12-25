#include "weapon_system.h"

#include "components.h"
#include <SDL_image.h>

namespace WeaponSystem {
    void Update(uint32_t ticks, entt::registry& registry, SDL_Renderer* renderer) {
        registry.view<WeaponComponent, PositionComponent>().each([ticks, &registry, renderer](auto& weaponComponent, auto const& positionComponent) {
            if (weaponComponent.timer > 0) {
                weaponComponent.timer -= ticks;
            }

            if (weaponComponent.timer <= 0 && weaponComponent.firing) {
                // spawn projectile
                auto projectile = registry.create();
                auto& projectilePositionComponent = registry.emplace<PositionComponent>(projectile);
                auto& projectileVelocityComponent = registry.emplace<VelocityComponent>(projectile);
                auto& projectileSpriteComponent = registry.emplace<SpriteComponent>(projectile);
                auto& collisionComponent = registry.emplace<CollisionComponent>(projectile);
                projectilePositionComponent.x = positionComponent.x;
                projectilePositionComponent.y = positionComponent.y;
                projectileVelocityComponent.x = weaponComponent.velocity * (weaponComponent.facing_left ? -1.0f : 1.0f);
                projectileVelocityComponent.y = 0;
                SDL_Surface* image = IMG_Load("laser.png");
                projectileSpriteComponent.texture = SDL_CreateTextureFromSurface(renderer, image);
                SDL_FreeSurface(image);
                projectileSpriteComponent.width = 42;
                projectileSpriteComponent.height = 40;
                collisionComponent.width = 15;
                collisionComponent.height = 15;
                collisionComponent.flags = COLLISION_FLAG_BULLET;
                collisionComponent.flag_mask = COLLISION_FLAG_MAP | COLLISION_FLAG_ENEMY;
                collisionComponent.on_collision = [&registry, projectile](entt::entity otherEntity) {
                    registry.destroy(projectile);
                };
                weaponComponent.timer = weaponComponent.fire_delay;
            }
        });
    }
}
