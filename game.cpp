#include "game.h"

#include <SDL_image.h>
#include <SDL_ttf.h>

#include "imgui.h"
#include "backends/imgui_impl_sdl.h"
#include "backends/imgui_impl_sdlrenderer.h"

#include "menu_layer.h"

Game::Game() {
    m_updateTicks = 1000 / 60;
}

Game::~Game() {

}

int Game::Run() {
    if (!Initialise()) {
        return 1;
    }

    m_running = true;
    m_lastUpdateTicks = SDL_GetTicks();

    while (m_running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            OnEvent(event);
        }

        Update();
        Draw();
    }

    Shutdown();

    return 0;
}

bool Game::Initialise() {
    if (0 > SDL_Init(SDL_INIT_EVERYTHING)) {
        return false;
    }

    if (nullptr == (m_window = SDL_CreateWindow("Xmas Game 2021", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN))) {
        return false;
    }

    if (nullptr == (m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED))) {
        return false;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForSDLRenderer(m_window);
    ImGui_ImplSDLRenderer_Init(m_renderer);

    SDL_SetRenderDrawColor(m_renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    int const imgFlags = IMG_INIT_JPG | IMG_INIT_PNG;
    if (imgFlags != IMG_Init(imgFlags)) {
        return false;
    }

    if (0 > TTF_Init()) {
        return false;
    }

    m_layerStack = std::make_unique<LayerStack>(WINDOW_WIDTH, WINDOW_HEIGHT);

    auto menuLayer = std::make_unique<MenuLayer>();
    m_layerStack->PushLayer(std::move(menuLayer));

    return true;
}

void Game::OnEvent(SDL_Event& event) {
    if (event.type == SDL_QUIT) {
        m_running = false;
    }
    m_layerStack->OnEvent(event);
}

void Game::Update() {
    uint32_t const nowTicks = SDL_GetTicks();
    uint32_t deltaTicks = nowTicks - m_lastUpdateTicks;
    while (deltaTicks > m_updateTicks) {
        m_layerStack->Update(m_updateTicks);
        m_lastUpdateTicks += m_updateTicks;
        deltaTicks -= m_updateTicks;
    }
}

void Game::Draw() {
    ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame(m_window);
    ImGui::NewFrame();

    SDL_RenderClear(m_renderer);
    m_layerStack->Draw(m_renderer);

    ImGui::Render();
    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(m_renderer);
}

void Game::Shutdown() {
    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    IMG_Quit();
    SDL_Quit();
}
