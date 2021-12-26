#include "menu_layer.h"

#include <imgui.h>
#include <SDL_image.h>

#include "game_layer.h"
#include "layer_stack.h"

MenuLayer::MenuLayer(SDL_Renderer* renderer)
:m_renderer(renderer) {
    SDL_Surface* bg = IMG_Load("title_raw.jpg");
    m_splashTexture = SDL_CreateTextureFromSurface(m_renderer, bg);
    SDL_FreeSurface(bg);

    TTF_Font* font = TTF_OpenFont("pilotcommand.ttf", 60);
    SDL_Color textColor{ 255, 255, 255, 255 };
    SDL_Color dropColor{ 0, 0, 0, 255 };
    SDL_Surface* titleText = nullptr;
    titleText = TTF_RenderText_Solid(font, "Pew Pew 2021", textColor);
    m_titleText = SDL_CreateTextureFromSurface(m_renderer, titleText);
    m_titleTextDim = std::make_tuple(titleText->w, titleText->h);
    SDL_FreeSurface(titleText);
    titleText = TTF_RenderText_Solid(font, "Pew Pew 2021", dropColor);
    m_titleTextDrop = SDL_CreateTextureFromSurface(m_renderer, titleText);
    SDL_FreeSurface(titleText);
    TTF_CloseFont(font);

    font = TTF_OpenFont("pilotcommand.ttf", 20);
    SDL_Surface* promptText = nullptr;
    promptText = TTF_RenderText_Solid(font, "Press space to start", textColor);
    m_promptText = SDL_CreateTextureFromSurface(m_renderer, promptText);
    m_promptTextDim = std::make_tuple(promptText->w, promptText->h);
    SDL_FreeSurface(promptText);
    promptText = TTF_RenderText_Solid(font, "Press space to start", dropColor);
    m_promptTextDrop = SDL_CreateTextureFromSurface(m_renderer, promptText);
    SDL_FreeSurface(promptText);
    TTF_CloseFont(font);
}

MenuLayer::~MenuLayer() {
    SDL_DestroyTexture(m_splashTexture);
    SDL_DestroyTexture(m_titleText);
    SDL_DestroyTexture(m_titleTextDrop);
    SDL_DestroyTexture(m_promptText);
    SDL_DestroyTexture(m_promptTextDrop);
}

bool MenuLayer::OnEvent(SDL_Event& event) {
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

void MenuLayer::Draw(SDL_Renderer* renderer) {
    SDL_RenderCopy(renderer, m_splashTexture, nullptr, nullptr);

    int const displayWidth = 640; // TODO
    int const displayHeight = 480;

    int const text1Width = std::get<0>(m_titleTextDim);
    int const text1Height = std::get<1>(m_titleTextDim);
    int const text1X = (displayWidth - text1Width) / 2;
    int const text1Y = (displayHeight - text1Height) / 2;

    int const text2Width = std::get<0>(m_promptTextDim);
    int const text2Height = std::get<1>(m_promptTextDim);
    int const text2X = (displayWidth - text2Width) / 2;
    int const text2Y = text1Y + text1Height;

    SDL_Rect destRect1Drop{ text1X + 2, text1Y + 2, text1Width, text1Height };
    SDL_RenderCopy(renderer, m_titleTextDrop, nullptr, &destRect1Drop);

    SDL_Rect destRect1{ text1X, text1Y, text1Width, text1Height };
    SDL_RenderCopy(renderer, m_titleText, nullptr, &destRect1);

    SDL_Rect destRect2Drop{ text2X + 2, text2Y + 2, text2Width, text2Height };
    SDL_RenderCopy(renderer, m_promptTextDrop, nullptr, &destRect2Drop);

    SDL_Rect destRect2{ text2X, text2Y, text2Width, text2Height };
    SDL_RenderCopy(renderer, m_promptText, nullptr, &destRect2);
}

void MenuLayer::OnAddedToStack(LayerStack* layerStack) {
    m_layerStack = layerStack;
}
void MenuLayer::OnRemovedFromStack() {
    m_layerStack = nullptr;
}
