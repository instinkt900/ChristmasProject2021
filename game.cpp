#include "game.h"

Game::Game() {

}

Game::~Game() {

}

int Game::Run() {
    if (!Initialise()) {
        return 1;
    }

    LoadTexture();

    m_running = true;

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

    SDL_SetRenderDrawColor(m_renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    int const imgFlags = IMG_INIT_JPG | IMG_INIT_PNG;
    if (imgFlags != IMG_Init(imgFlags)) {
        return false;
    }

    return true;
}

void Game::OnEvent(SDL_Event& event) {
    if (event.type == SDL_QUIT) {
        m_running = false;
    }
}

void Game::Update() {

}

void Game::Draw() {
    SDL_RenderClear(m_renderer);
    SDL_RenderCopy(m_renderer, m_testTexture, NULL, NULL);
    SDL_RenderPresent(m_renderer);
}

void Game::Shutdown() {
    SDL_DestroyTexture(m_testTexture);
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    IMG_Quit();
    SDL_Quit();
}

void Game::LoadTexture() {
    SDL_Surface* imageSurface = IMG_Load("test.jpg");
    if (nullptr != imageSurface) {
        m_testTexture = SDL_CreateTextureFromSurface(m_renderer, imageSurface);
        SDL_FreeSurface(imageSurface);
    }
}
