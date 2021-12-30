#pragma once

#include "layer.h"

class MenuLayer : public Layer
{
public:
    MenuLayer(SDL_Renderer* renderer);
    virtual ~MenuLayer();

    bool OnEvent(SDL_Event const& event) override;
    void Update(uint32_t ticks) override;
    void Draw(SDL_Renderer* renderer) override;

private:
    SDL_Renderer* m_renderer = nullptr;
    SDL_Texture* m_splashTexture = nullptr;

    SDL_Texture* m_titleText = nullptr;
    SDL_Texture* m_titleTextDrop = nullptr;
    SDL_Texture* m_promptText = nullptr;
    SDL_Texture* m_promptTextDrop = nullptr;
    std::tuple<int, int> m_titleTextDim;
    std::tuple<int, int> m_promptTextDim;
};
