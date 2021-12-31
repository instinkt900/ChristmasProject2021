#include "game_pch.h"
#include "menu_layer.h"
#include "game_layer.h"
#include "layer_stack.h"

MenuLayer::MenuLayer(SDL_Renderer& renderer)
    : m_renderer(renderer) {
    m_splashTexture = CreateTextureRef(&m_renderer, "title_raw.jpg");

    FontRef font = CreateFontRef("pilotcommand.ttf", 60);
    SDL_Color textColor{ 255, 255, 255, 255 };
    SDL_Color dropColor{ 0, 0, 0, 255 };
    SurfaceRef titleText = CreateSurfaceRef(TTF_RenderText_Solid(font.get(), "Pew Pew 2021", textColor));
    m_titleText = CreateTextureRef(&m_renderer, titleText);
    m_titleTextDim = std::make_tuple(titleText->w, titleText->h);

    titleText = CreateSurfaceRef(TTF_RenderText_Solid(font.get(), "Pew Pew 2021", dropColor));
    m_titleTextDrop = CreateTextureRef(&m_renderer, titleText);

    font = CreateFontRef("pilotcommand.ttf", 20);
    SurfaceRef promptText = CreateSurfaceRef(TTF_RenderText_Solid(font.get(), "Press space to start", textColor));
    m_promptText = CreateTextureRef(&m_renderer, promptText);
    m_promptTextDim = std::make_tuple(promptText->w, promptText->h);

    promptText = CreateSurfaceRef(TTF_RenderText_Solid(font.get(), "Press space to start", dropColor));
    m_promptTextDrop = CreateTextureRef(&m_renderer, promptText);
}

MenuLayer::~MenuLayer() {
}

bool MenuLayer::OnEvent(SDL_Event const& event) {
    if (event.type == SDL_KEYUP) {
        switch (event.key.keysym.sym) {
        case SDLK_SPACE:
            auto gameLayer = std::make_unique<GameLayer>(m_renderer);
            m_layerStack->PushLayer(std::move(gameLayer));
            m_layerStack->RemoveLayer(this);
            return true;
        }
    }
    return false;
}

void MenuLayer::Update(uint32_t ticks) {
}

void MenuLayer::Draw(SDL_Renderer& renderer) {
    SDL_RenderCopy(&m_renderer, m_splashTexture.get(), nullptr, nullptr);

    int const text1Width = std::get<0>(m_titleTextDim);
    int const text1Height = std::get<1>(m_titleTextDim);
    int const text1X = (GetWidth() - text1Width) / 2;
    int const text1Y = (GetHeight() - text1Height) / 2;

    int const text2Width = std::get<0>(m_promptTextDim);
    int const text2Height = std::get<1>(m_promptTextDim);
    int const text2X = (GetWidth() - text2Width) / 2;
    int const text2Y = text1Y + text1Height;

    SDL_Rect destRect1Drop{ text1X + 2, text1Y + 2, text1Width, text1Height };
    SDL_RenderCopy(&m_renderer, m_titleTextDrop.get(), nullptr, &destRect1Drop);

    SDL_Rect destRect1{ text1X, text1Y, text1Width, text1Height };
    SDL_RenderCopy(&m_renderer, m_titleText.get(), nullptr, &destRect1);

    SDL_Rect destRect2Drop{ text2X + 2, text2Y + 2, text2Width, text2Height };
    SDL_RenderCopy(&m_renderer, m_promptTextDrop.get(), nullptr, &destRect2Drop);

    SDL_Rect destRect2{ text2X, text2Y, text2Width, text2Height };
    SDL_RenderCopy(&m_renderer, m_promptText.get(), nullptr, &destRect2);
}
