#include "game_pch.h"
#include "game.h"
#include "layers/splash_layer.h"
#include "image_factory.h"
#include "ui_renderer.h"
#include "moth_ui/context.h"
#include "event_factory.h"
#include "font_factory.h"

// TODO needed a few places but we dont want to pass this around
// not strictly needed as we can remove its use by being smarter
// about resource creation.
SDL_Renderer* g_renderer = nullptr;

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
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (ImGui::GetIO().WantCaptureKeyboard && (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)) {
                continue;
            }
            if (ImGui::GetIO().WantCaptureMouse && (event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP || event.type == SDL_MOUSEWHEEL)) {
                continue;
            }
            if (auto const translatedEvent = EventFactory::FromSDL(event)) {
                OnEvent(*translatedEvent);
            }
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

    g_renderer = m_renderer;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    //ImGuiIO& io = ImGui::GetIO();
    //(void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForSDLRenderer(m_window);
    ImGui_ImplSDLRenderer_Init(m_renderer);

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

    m_imageFactory = std::make_unique<ImageFactory>(*m_renderer);
    m_fontFactory = std::make_unique<FontFactory>(*m_renderer);
    m_uiRenderer = std::make_unique<UIRenderer>(*m_renderer);
    auto uiContext = std::make_shared<moth_ui::Context>(m_imageFactory.get(), m_fontFactory.get(), m_uiRenderer.get());
    moth_ui::Context::SetCurrentContext(uiContext);

    m_layerStack = std::make_unique<LayerStack>(m_renderWidth, m_renderHeight, m_windowWidth, m_windowHeight);

    auto menuLayer = std::make_unique<SplashLayer>(*this);
    m_layerStack->PushLayer(std::move(menuLayer));

    return true;
}

void Game::OnEvent(moth_ui::Event const& event) {
    if (auto windowEvent = moth_ui::event_cast<EventWindowSize>(event)) {
        if (m_editorMode) {
            m_editorWindowWidth = windowEvent->GetWidth();
            m_editorWindowHeight = windowEvent->GetHeight();
        } else {
            m_windowWidth = windowEvent->GetWidth();
            m_windowHeight = windowEvent->GetHeight();
        }
    } else if (auto quitEvent = moth_ui::event_cast<EventRequestQuit>(event)) {
        m_running = false;
    } else if (auto keyEvent = moth_ui::event_cast<moth_ui::EventKey>(event)) {
        if (keyEvent->GetAction() == moth_ui::KeyAction::Up) {
            switch (keyEvent->GetKey()) {
            case moth_ui::Key::Escape:
                //m_running = false;
                break;
            case moth_ui::Key::G:
                SetEditorMode(!m_editorMode);
                break;
            }
        }
    }

    if (m_editorMode) {
        std::shared_ptr<moth_ui::Event> translatedEvent;
        if (auto mouseEvent = moth_ui::event_cast<moth_ui::EventMouseDown>(event)) {
            moth_ui::IntVec2 translatedPosition = mouseEvent->GetPosition();
            translatedPosition.x -= m_gameWindowPos.x;
            translatedPosition.y -= m_gameWindowPos.y;
            translatedEvent = std::make_shared<moth_ui::EventMouseDown>(mouseEvent->GetButton(), translatedPosition);
        } else if (auto mouseEvent = moth_ui::event_cast<moth_ui::EventMouseUp>(event)) {
            moth_ui::IntVec2 translatedPosition = mouseEvent->GetPosition();
            translatedPosition.x -= m_gameWindowPos.x;
            translatedPosition.y -= m_gameWindowPos.y;
            translatedEvent = std::make_shared<moth_ui::EventMouseUp>(mouseEvent->GetButton(), translatedPosition);
        }
        if (translatedEvent) {
            m_layerStack->OnEvent(*translatedEvent);
            return;
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
    ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame(m_window);
    ImGui::NewFrame();

    if (m_editorMode) {
        SDL_SetRenderDrawColor(m_renderer, 0x11, 0x11, 0x11, 0xFF);
        SDL_RenderClear(m_renderer);
        SDL_SetRenderTarget(m_renderer, m_gameSurface.get());
    }

    SDL_SetRenderDrawColor(m_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(m_renderer);

    int oldLogicalWidth;
    int oldLogicalHeight;
    SDL_RenderGetLogicalSize(m_renderer, &oldLogicalWidth, &oldLogicalHeight);
    m_layerStack->Draw(*m_renderer);
    SDL_RenderSetLogicalSize(m_renderer, oldLogicalWidth, oldLogicalHeight);

    if (m_editorMode) {
        SDL_SetRenderTarget(m_renderer, nullptr);

        if (ImGui::Begin("Game")) {
            auto windowPos = ImGui::GetWindowPos();
            auto windowRegionStart = ImGui::GetWindowContentRegionMin();
            m_gameWindowPos.x = static_cast<int>(windowPos.x + windowRegionStart.x);
            m_gameWindowPos.y = static_cast<int>(windowPos.y + windowRegionStart.y);
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

        m_layerStack->DebugDraw();
    }

    ImGui::Render();
    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(m_renderer);
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
