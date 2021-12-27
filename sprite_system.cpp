#include "sprite_system.h"
#include "game_layer.h"
#include "components.h"

namespace SpriteSystem {
    void Update(uint32_t ticks, GameLayer& gameLayer) {
        auto& registry = gameLayer.GetRegistry();
        registry.view<SpriteComponent>().each([ticks](auto& spriteComponent) {
            if (spriteComponent.playing) {
                auto const animDuration = spriteComponent.frames * spriteComponent.ticks_per_frame;
                spriteComponent.anim_time += ticks;
                if (spriteComponent.anim_time > animDuration) {
                    switch (spriteComponent.anim_type) {
                    case AnimType::Stop:
                        spriteComponent.anim_time = animDuration;
                        spriteComponent.playing = false;
                        break;
                    case AnimType::Loop:
                        spriteComponent.anim_time = spriteComponent.anim_time % animDuration;
                        break;
                    case AnimType::Reset:
                        spriteComponent.anim_time = 0;
                        spriteComponent.playing = false;
                        break;
                    }
                }
            }
        });
    }

    void Draw(SDL_Renderer* renderer, entt::registry& registry, int viewOffsetX, int viewOffsetY, int viewWidth, int viewHeight) {
        registry.view<PositionComponent, SpriteComponent>().each([renderer, viewOffsetX, viewOffsetY](auto const& positionComponent, auto const& spriteComponent) {
            if (spriteComponent.visible) {
                SDL_Rect const* sourceRect = nullptr;
                if (spriteComponent.frames > 0) {
                    int const frameIndex = std::min(spriteComponent.frames - 1, spriteComponent.anim_time / spriteComponent.ticks_per_frame);
                    sourceRect = &(spriteComponent.source_rects[frameIndex]);
                }

                int const halfWidth = spriteComponent.width / 2;
                int const halfHeight = spriteComponent.height / 2;
                int const top = static_cast<int>(positionComponent.y - viewOffsetY - halfHeight);
                int const left = static_cast<int>(positionComponent.x - viewOffsetX - halfWidth);

                SDL_Rect destRect{ left, top, spriteComponent.width, spriteComponent.height };
                SDL_RenderCopyEx(renderer, spriteComponent.texture, sourceRect, &destRect, 0.0f, nullptr, spriteComponent.flip);
            }
        });
    }
}