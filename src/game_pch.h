#pragma once

#include <random>
#include <fstream>
#include <filesystem>
#include <memory>
#include <vector>
#include <cmath>
#include <variant>
#include <stack>

#include <imgui.h>
#include <backends/imgui_impl_sdl.h>
#include <backends/imgui_impl_sdlrenderer.h>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#include <entt/entt.hpp>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <glm/glm.hpp>

#include <magic_enum.hpp>

#include "smart_sdl.h"
#include "utils.h"

#include "moth_ui/events/event.h"
#include "moth_ui/events/event_key.h"
#include "moth_ui/events/event_mouse.h"
#include "moth_ui/events/event_animation.h"
#include "moth_ui/event_listener.h"
#include "moth_ui/event_dispatch.h"

#include "events/event.h"
