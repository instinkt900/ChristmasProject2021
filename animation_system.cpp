#include "game_pch.h"
#include "animation_system.h"
#include "components.h"
#include "game_layer.h"

namespace AnimationSystem {
    void Update(uint32_t ticks, GameLayer& gameLayer) {
        auto& registry = gameLayer.GetRegistry();
        registry.view<AnimationComponent, SpriteComponent>().each([ticks](auto& animationComponent, auto& spriteComponent) {
            int const frameCount = static_cast<int>(animationComponent.frames.size());
            if (animationComponent.playing) {
                auto const animDuration = frameCount * animationComponent.ticks_per_frame;
                animationComponent.current_time += ticks;
                if (animationComponent.current_time > animDuration) {
                    switch (animationComponent.anim_type) {
                    case AnimType::Stop:
                        animationComponent.current_time = animDuration;
                        animationComponent.playing = false;
                        break;
                    case AnimType::Loop:
                        animationComponent.current_time = animationComponent.current_time % animDuration;
                        break;
                    case AnimType::Reset:
                        animationComponent.current_time = 0;
                        animationComponent.playing = false;
                        break;
                    }
                }
            }
            int const frameIndex = std::min(frameCount - 1, animationComponent.current_time / animationComponent.ticks_per_frame);
            spriteComponent.source_rect = &animationComponent.frames[frameIndex];
            });
    }
}
