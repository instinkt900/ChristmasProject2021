#pragma once

#include <SDL.h>
#include <SDL_image.h>

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

    void LoadTexture();

private:
    static int constexpr WINDOW_WIDTH = 640;
    static int constexpr WINDOW_HEIGHT = 480;

    bool m_running = false;
    SDL_Window* m_window = nullptr;
    SDL_Renderer* m_renderer = nullptr;
    SDL_Texture* m_testTexture = nullptr;
};
