#include "game_pch.h"
#include "event.h"
#include "event_window_size.h"
#include "event_quit.h"
#include "event_key.h"
#include "event_device.h"

std::unique_ptr<Event> Event::FromSDL(SDL_Event const& event) {
    switch (event.type) {
    case SDL_WINDOWEVENT: {
        switch (event.window.type) {
        case SDL_WINDOWEVENT_SIZE_CHANGED: {
            return std::make_unique<EventWindowSize>(event.window.data1, event.window.data2);
        default:
            return nullptr;
        }
        }
    }
    case SDL_QUIT: {
        return std::make_unique<EventQuit>();
    }
    case SDL_KEYUP: {
        return std::make_unique<EventKey>(KeyAction::Up, FromSDLKey(event.key.keysym.sym));
    }
    case SDL_KEYDOWN: {
        return std::make_unique<EventKey>(KeyAction::Down, FromSDLKey(event.key.keysym.sym));
    }
    case SDL_RENDER_DEVICE_RESET: {
        return std::make_unique<EventRenderDeviceReset>();
    }
    case SDL_RENDER_TARGETS_RESET: {
        return std::make_unique<EventRenderTargetReset>();
    }
    }
    return nullptr;
}
