#pragma once

#include <random>
#include <fstream>
#include <filesystem>
#include <memory>
#include <vector>
#include <cmath>
#include <variant>

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
#include "events/event.h"
#include "ui/serialize_utils.h"
#include "ui/imgui_ext.h"

#include "range/v3/algorithm/find.hpp"
#include "range/v3/algorithm/find_if.hpp"
#include "range/v3/algorithm/sort.hpp"
