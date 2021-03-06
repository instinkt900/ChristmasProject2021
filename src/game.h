#pragma once

#include "layers/layer_stack.h"
#include "audio_factory.h"

#include "moth_ui/iimage_factory.h"
#include "moth_ui/ifont_factory.h"
#include "moth_ui/irenderer.h"

class Game {
public:
    Game(int renderWidth, int renderHeight, std::string const& configPath = "config.json");
    virtual ~Game();

    int Run();
    void Stop() { m_running = false; }
    void SetPaused(bool paused) { m_paused = paused; }

    void SetEditorMode(bool editorMode);
    bool IsEditorMode() const { return m_editorMode; }

    auto GetRenderer() const { return m_renderer; }
    auto& GetAudioFactory() { return m_audioFactory; }

protected:
    bool Initialise();
    void OnEvent(moth_ui::Event const& event);
    void Update();
    void Draw();
    void Shutdown();

private:
    static int constexpr INIT_WINDOW_WIDTH = 1280;
    static int constexpr INIT_WINDOW_HEIGHT = 960;

    int m_renderWidth = 0;
    int m_renderHeight = 0;
    int m_windowWidth = 0;
    int m_windowHeight = 0;
    int m_editorWindowWidth = 0;
    int m_editorWindowHeight = 0;
    std::string m_configPath;

    bool m_running = false;
    bool m_paused = false;
    bool m_editorMode = false;
    uint32_t m_updateTicks = 0;
    uint32_t m_lastUpdateTicks = 0;
    SDL_Window* m_window = nullptr;
    SDL_Renderer* m_renderer = nullptr;
    AudioFactory m_audioFactory;
    TextureRef m_gameSurface;
    IntVec2 m_gameWindowPos;

    std::unique_ptr<moth_ui::IImageFactory> m_imageFactory;
    std::unique_ptr<moth_ui::IFontFactory> m_fontFactory;
    std::unique_ptr<moth_ui::IRenderer> m_uiRenderer;

    std::unique_ptr<LayerStack> m_layerStack;
};
