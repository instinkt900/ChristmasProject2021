#include "input_system.h"

#include "components.h"

InputSystem::InputSystem() {

}

InputSystem::~InputSystem() {

}

bool InputSystem::OnEvent(SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
        case SDLK_UP: m_controlState[ControlKey::Up] = true; return true;
        case SDLK_DOWN: m_controlState[ControlKey::Down] = true; return true;
        case SDLK_LEFT: m_controlState[ControlKey::Left] = true; return true;
        case SDLK_RIGHT: m_controlState[ControlKey::Right] = true; return true;
        }
    }
    else if (event.type == SDL_KEYUP) {
        switch (event.key.keysym.sym) {
        case SDLK_UP: m_controlState[ControlKey::Up] = false; return true;
        case SDLK_DOWN: m_controlState[ControlKey::Down] = false; return true;
        case SDLK_LEFT: m_controlState[ControlKey::Left] = false; return true;
        case SDLK_RIGHT: m_controlState[ControlKey::Right] = false; return true;
        }
    }
    return false;
}

void InputSystem::Update(uint32_t ticks, entt::registry& registry) {
    registry.view<InputComponent, PositionComponent>().each([this, ticks](auto, auto& position) {
        if (m_controlState[ControlKey::Up]) position.y -= ticks * 0.1f;
        if (m_controlState[ControlKey::Down]) position.y += ticks * 0.1f;
        if (m_controlState[ControlKey::Left]) position.x -= ticks * 0.1f;
        if (m_controlState[ControlKey::Right]) position.x += ticks * 0.1f;
        });
}
