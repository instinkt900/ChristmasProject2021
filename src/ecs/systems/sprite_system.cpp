#include "game_pch.h"
#include "sprite_system.h"
#include "utils.h"
#include "layers/game_layer.h"
#include "ecs/components/components.h"

namespace SpriteSystem {
    void Draw(SDL_Renderer& renderer, entt::registry& registry, ViewParameters const& view) {
        registry.view<PositionComponent, SpriteComponent>().each([&renderer, &registry, &view](auto const& positionComponent, auto const& spriteComponent) {
            if (spriteComponent.visible) {
                auto const position = ResolvePosition(registry, positionComponent);
                int const halfWidth = spriteComponent.width / 2;
                int const halfHeight = spriteComponent.height / 2;
                int const top = position.y - view.m_offsetY - halfHeight;
                int const left = position.x - view.m_offsetX - halfWidth;
                SDL_Rect destRect{ left, top, spriteComponent.width, spriteComponent.height };
                SDL_RenderCopyEx(&renderer, spriteComponent.texture.get(), spriteComponent.source_rect, &destRect, 0.0f, nullptr, spriteComponent.flip);
            }
        });
    }
}
