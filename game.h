#pragma once

#include <SDL.h>

#include "layer_stack.h"

class Game
{
public:
    Game();
    ~Game();

    int Run();

protected:
    bool Initialise();
    void OnEvent(SDL_Event& event);
    void Update();
    void Draw();
    void Shutdown();

private:
    static int constexpr WINDOW_WIDTH = 640;
    static int constexpr WINDOW_HEIGHT = 480;

    bool m_running = false;
    uint32_t m_updateTicks = 0;
    uint32_t m_lastUpdateTicks = 0;
    SDL_Window* m_window = nullptr;
    SDL_Renderer* m_renderer = nullptr;

    std::unique_ptr<LayerStack> m_layerStack;
};
