#pragma once

#include "layers/layer_stack.h"
#include "audio_factory.h"

class Game {
public:
    Game(int renderWidth, int renderHeight, std::string const& configPath = "config.json");
    virtual ~Game();

    int Run();
    void Stop() { m_running = false; }

protected:
    bool Initialise();
    void OnEvent(SDL_Event const& event);
    void Update();
    void Draw();
    void Shutdown();

private:
    static int constexpr INIT_WINDOW_WIDTH = 1280;
    static int constexpr INIT_WINDOW_HEIGHT = 960;

    int m_renderWidth = 0;
    int m_renderHeight = 0;
    std::string m_configPath;

    bool m_running = false;
    uint32_t m_updateTicks = 0;
    uint32_t m_lastUpdateTicks = 0;
    SDL_Window* m_window = nullptr;
    SDL_Renderer* m_renderer = nullptr;
    AudioFactory m_audioFactory;

    std::unique_ptr<LayerStack> m_layerStack;
};
