#include "game_pch.h"
#include "utils.h"
#include "ecs/components/components.h"

IntVec2 ResolvePosition(entt::registry& registry, entt::entity entity) {
    IntVec2 position;
    if (registry.valid(entity)) {
        auto const& positionComponent = registry.get<PositionComponent>(entity);
        auto const parentPosition = ResolvePosition(registry, positionComponent.parent);
        position.x = static_cast<int>(positionComponent.x + parentPosition.x);
        position.y = static_cast<int>(positionComponent.y + parentPosition.y);
    }
    return position;
}

IntVec2 ResolvePosition(entt::registry& registry, PositionComponent const& positionComponent) {
    IntVec2 position;
    auto const parentPosition = ResolvePosition(registry, positionComponent.parent);
    position.x = static_cast<int>(positionComponent.x + parentPosition.x);
    position.y = static_cast<int>(positionComponent.y + parentPosition.y);
    return position;
}
