#include "game_pch.h"
#include "game.h"
#include "layers/menu_layer.h"

Game::Game(int renderWidth, int renderHeight, std::string const& configPath)
    : m_renderWidth(renderWidth)
    , m_renderHeight(renderHeight)
    , m_windowWidth(INIT_WINDOW_WIDTH)
    , m_windowHeight(INIT_WINDOW_HEIGHT)
    , m_editorWindowWidth(INIT_WINDOW_WIDTH)
    , m_editorWindowHeight(INIT_WINDOW_HEIGHT)
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

void Game::SetEditorMode(bool editorMode) {
    m_editorMode = editorMode;
    if (m_editorMode) {
        if (m_gameSurface == nullptr) {
            m_gameSurface = CreateTextureRef(SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, m_renderWidth, m_renderHeight));
        }
        SDL_SetWindowSize(m_window, m_editorWindowWidth, m_editorWindowHeight);
        SDL_SetWindowResizable(m_window, SDL_TRUE);
    } else {
        SDL_SetWindowSize(m_window, m_windowWidth, m_windowHeight);
        SDL_SetWindowResizable(m_window, SDL_FALSE);
    }
}

bool Game::Initialise() {
    nlohmann::json j;
    std::ifstream configFile(m_configPath.c_str());
    if (configFile.is_open()) {
        configFile >> j;
    }

    if (j.contains("window_width")) {
        m_windowWidth = j["window_width"];
    }
    if (j.contains("window_height")) {
        m_windowHeight = j["window_height"];
    }
    if (j.contains("editor_window_width")) {
        m_editorWindowWidth = j["editor_window_width"];
    }
    if (j.contains("editor_window_height")) {
        m_editorWindowHeight = j["editor_window_height"];
    }

    if (0 > SDL_Init(SDL_INIT_EVERYTHING)) {
        return false;
    }

    if (nullptr == (m_window = SDL_CreateWindow("Xmas Game 2021", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_windowWidth, m_windowHeight, SDL_WINDOW_SHOWN))) {
        return false;
    }

    if (nullptr == (m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED))) {
        return false;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForSDLRenderer(m_window);
    ImGui_ImplSDLRenderer_Init(m_renderer);

    ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame(m_window);
    ImGui::NewFrame();

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

    auto menuLayer = std::make_unique<MenuLayer>(*this);
    m_layerStack->PushLayer(std::move(menuLayer));

    return true;
}

void Game::OnEvent(SDL_Event const& event) {
    ImGui_ImplSDL2_ProcessEvent(&event);
    if (ImGui::GetIO().WantCaptureKeyboard && (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)) {
        return;
    }
    if (event.type == SDL_WINDOWEVENT) {
        if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
            if (m_editorMode) {
                m_editorWindowWidth = event.window.data1;
                m_editorWindowHeight = event.window.data2;
            } else {
                m_windowWidth = event.window.data1;
                m_windowHeight = event.window.data2;
            }
        }
    } else if (event.type == SDL_QUIT) {
        m_running = false;
    } else if (event.type == SDL_KEYUP) {
        switch (event.key.keysym.sym) {
        case SDLK_ESCAPE:
            m_running = false;
            break;
        case SDLK_g:
            SetEditorMode(!m_editorMode);
            break;
        }
    }
    m_layerStack->OnEvent(event);
}

void Game::Update() {
    uint32_t const nowTicks = SDL_GetTicks();
    uint32_t deltaTicks = nowTicks - m_lastUpdateTicks;
    while (deltaTicks > m_updateTicks) {
        if (!m_paused) {
            m_layerStack->Update(m_updateTicks);
        }
        m_lastUpdateTicks += m_updateTicks;
        deltaTicks -= m_updateTicks;
    }
}

void Game::Draw() {
    SDL_RenderSetLogicalSize(m_renderer, m_renderWidth, m_renderHeight);

    if (m_editorMode) {
        SDL_SetRenderDrawColor(m_renderer, 0x11, 0x11, 0x11, 0xFF);
        SDL_RenderClear(m_renderer);
        SDL_SetRenderTarget(m_renderer, m_gameSurface.get());
    }

    SDL_SetRenderDrawColor(m_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(m_renderer);
    m_layerStack->Draw(*m_renderer);

    if (m_editorMode) {
        SDL_SetRenderTarget(m_renderer, nullptr);
        SDL_RenderSetLogicalSize(m_renderer, m_editorWindowWidth, m_editorWindowHeight);

        if (ImGui::Begin("Game")) {
            ImGui::Image(m_gameSurface.get(), ImVec2(static_cast<float>(m_renderWidth), static_cast<float>(m_renderHeight)));
        }
        ImGui::End();

        if (ImGui::Begin("Control")) {
            if (ImGui::Button("Play")) {
                SetPaused(false);
            }
            ImGui::SameLine();
            if (ImGui::Button("Pause")) {
                SetPaused(true);
            }
        }
        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(m_renderer);

    ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame(m_window);
    ImGui::NewFrame();
}

void Game::Shutdown() {
    nlohmann::json j;
    j["window_width"] = m_windowWidth;
    j["window_height"] = m_windowHeight;
    j["editor_window_width"] = m_editorWindowWidth;
    j["editor_window_height"] = m_editorWindowHeight;
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
