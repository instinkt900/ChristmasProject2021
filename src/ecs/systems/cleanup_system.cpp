#include "game_pch.h"
#include "cleanup_system.h"
#include "layers/game_layer.h"
#include "ecs/components/components.h"

namespace CleanupSystem {
    void Update(uint32_t ticks, GameLayer& gameLayer) {
        auto& registry = gameLayer.GetRegistry();

        std::vector<entt::entity> deadEntities;
        registry.view<HealthComponent>().each([&deadEntities](auto entity, auto const& healthComponent) {
            if (!healthComponent.alive) {
                deadEntities.push_back(entity);
            }
        });

        for (auto entity : deadEntities) {
            registry.destroy(entity);
        }
    }
}
