#include "lifetime_system.h"
#include "components.h"
#include "game_layer.h"

namespace LifetimeSystem {
    void Update(uint32_t ticks, GameLayer& gameLayer) {
        auto& registry = gameLayer.GetRegistry();

        std::vector<entt::entity> deadEntities;
        registry.view<LifetimeComponent>().each([ticks, &deadEntities](auto entity, auto& lifetimeComponent) {
            if (ticks > lifetimeComponent.lifetime) {
                deadEntities.push_back(entity);
            }
            else {
                lifetimeComponent.lifetime -= ticks;
            }
        });

        for (auto&& entity : deadEntities) {
            registry.destroy(entity);
        }
    }
}
