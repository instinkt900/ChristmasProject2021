#include "velocity_system.h"

#include "components.h"

namespace VelocitySystem {
    void Update(uint32_t ticks, entt::registry& registry) {
        float const seconds = ticks / 1000.0f;
        registry.view<VelocityComponent, PositionComponent>().each([seconds](auto const& velocityComponent, auto& positionComponent) {
            positionComponent.x += velocityComponent.x * seconds;
            positionComponent.y += velocityComponent.y * seconds;
            });
    }
}
