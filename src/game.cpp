#include "game_pch.h"
#include "game.h"
#include "layers/menu_layer.h"

Game::Game(int renderWidth, int renderHeight, std::string const& configPath)
    : m_renderWidth(renderWidth)
    , m_renderHeight(renderHeight)
    , m_configPath(configPath) {
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
    nlohmann::json j;
    std::ifstream configFile(m_configPath.c_str());
    if (configFile.is_open()) {
        configFile >> j;
    }

    int windowWidth = INIT_WINDOW_WIDTH;
    int windowHeight = INIT_WINDOW_HEIGHT;

    if (j.contains("window_width")) {
        windowWidth = j["window_width"];
    }
    if (j.contains("window_height")) {
        windowHeight = j["window_height"];
    }

    if (0 > SDL_Init(SDL_INIT_EVERYTHING)) {
        return false;
    }

    if (nullptr == (m_window = SDL_CreateWindow("Xmas Game 2021", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_SHOWN))) {
        return false;
    }

    if (nullptr == (m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED))) {
        return false;
    }

    SDL_RenderSetLogicalSize(m_renderer, m_renderWidth, m_renderHeight);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

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

    if (0 > Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048)) {
        return false;
    }

    m_layerStack = std::make_unique<LayerStack>(m_renderWidth, m_renderHeight);

    auto menuLayer = std::make_unique<MenuLayer>(*m_renderer);
    m_layerStack->PushLayer(std::move(menuLayer));

    return true;
}

void Game::OnEvent(SDL_Event const& event) {
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
    m_layerStack->Draw(*m_renderer);

    ImGui::Render();
    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(m_renderer);
}

void Game::Shutdown() {
    int windowWidth = 0;
    int windowHeight = 0;
    int renderWidth = 0;
    int renderHeight = 0;

    SDL_GetWindowSize(m_window, &windowWidth, &windowHeight);
    SDL_RenderGetLogicalSize(m_renderer, &renderWidth, &renderHeight);
    nlohmann::json j;
    j["window_width"] = windowWidth;
    j["window_height"] = windowHeight;
    std::ofstream configFile(m_configPath.c_str());
    configFile << std::setw(4) << j << std::endl;

    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    IMG_Quit();
    SDL_Quit();
}
