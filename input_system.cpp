#include "input_system.h"
#include "components.h"
#include <map>

namespace InputSystem {

    enum class ControlKey {
        Up,
        Down,
        Left,
        Right,
        Fire
    };

    std::map<ControlKey, bool> ControlState;

    bool OnEvent(SDL_Event& event) {
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
            case SDLK_UP: ControlState[ControlKey::Up] = true; return true;
            case SDLK_DOWN: ControlState[ControlKey::Down] = true; return true;
            case SDLK_LEFT: ControlState[ControlKey::Left] = true; return true;
            case SDLK_RIGHT: ControlState[ControlKey::Right] = true; return true;
            case SDLK_SPACE: ControlState[ControlKey::Fire] = true; return true;
            }
        }
        else if (event.type == SDL_KEYUP) {
            switch (event.key.keysym.sym) {
            case SDLK_UP: ControlState[ControlKey::Up] = false; return true;
            case SDLK_DOWN: ControlState[ControlKey::Down] = false; return true;
            case SDLK_LEFT: ControlState[ControlKey::Left] = false; return true;
            case SDLK_RIGHT: ControlState[ControlKey::Right] = false; return true;
            case SDLK_SPACE: ControlState[ControlKey::Fire] = false; return true;
            }
        }
        return false;
    }

    void Update(uint32_t ticks, entt::registry& registry) {
        registry.view<InputComponent, PositionComponent>().each([ticks, &registry](auto entity, auto& position) {
            if (ControlState[ControlKey::Up]) position.y -= ticks * 0.1f;
            if (ControlState[ControlKey::Down]) position.y += ticks * 0.1f;
            if (ControlState[ControlKey::Left]) position.x -= ticks * 0.1f;
            if (ControlState[ControlKey::Right]) position.x += ticks * 0.1f;
            if (auto weaponComponent = registry.try_get<WeaponComponent>(entity)) {
                weaponComponent->firing = ControlState[ControlKey::Fire];
            }
            });
    }
}
