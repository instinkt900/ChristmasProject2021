#pragma once

#include "layers/layer_stack.h"

class Game {
public:
    Game();
    ~Game();

    int Run();

protected:
    bool Initialise();
    void OnEvent(SDL_Event const& event);
    void Update();
    void Draw();
    void Shutdown();

private:
    static int constexpr INIT_WINDOW_WIDTH = 1280;
    static int constexpr INIT_WINDOW_HEIGHT = 960;

    static int constexpr INIT_RENDER_WIDTH = 640;
    static int constexpr INIT_RENDER_HEIGHT = 480;

    bool m_running = false;
    uint32_t m_updateTicks = 0;
    uint32_t m_lastUpdateTicks = 0;
    SDL_Window* m_window = nullptr;
    SDL_Renderer* m_renderer = nullptr;

    std::unique_ptr<LayerStack> m_layerStack;
};
