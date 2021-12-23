#include "weapon_system.h"

#include "components.h"
#include <SDL_image.h>

namespace WeaponSystem {
    void Update(uint32_t ticks, entt::registry& registry, SDL_Renderer* renderer) {
        registry.view<WeaponComponent, PositionComponent>().each([ticks, &registry, renderer](auto& weaponComponent, auto const& positionComponent) {
            if (weaponComponent.firing) {
                weaponComponent.timer -= ticks;
                if (weaponComponent.timer <= 0) {
                    // spawn projectile
                    auto projectile = registry.create();
                    auto& projectilePositionComponent = registry.emplace<PositionComponent>(projectile);
                    auto& projectileVelocityComponent = registry.emplace<VelocityComponent>(projectile);
                    auto& projectileSpriteComponent = registry.emplace<SpriteComponent>(projectile);
                    projectilePositionComponent.x = positionComponent.x;
                    projectilePositionComponent.y = positionComponent.y;
                    projectileVelocityComponent.x = weaponComponent.velocity * (weaponComponent.facing_left ? -1.0f : 1.0f);
                    projectileVelocityComponent.y = 0;
                    SDL_Surface* image = IMG_Load("dot.png");
                    projectileSpriteComponent.texture = SDL_CreateTextureFromSurface(renderer, image);
                    SDL_FreeSurface(image);
                    projectileSpriteComponent.width = 10;
                    projectileSpriteComponent.height = 10;
                    weaponComponent.timer = weaponComponent.fire_delay;
                }
            }
            else {
                weaponComponent.timer = weaponComponent.fire_delay;
            }
            });
    }
}
