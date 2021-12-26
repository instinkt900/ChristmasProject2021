#include "state_post_game.h"
#include "game_layer.h"
#include "components.h"
#include "state_pre_game.h"

#include <SDL_ttf.h>

StatePostGame::StatePostGame(StateMachine* stateMachine, GameLayer& gameLayer)
: State(stateMachine)
, m_gameLayer(gameLayer) {
    TTF_Font* font = TTF_OpenFont("pilotcommand.ttf", 80);
    SDL_Color textColor{ 255, 255, 255, 255 };
    SDL_Surface* gameOverText = nullptr;
    gameOverText = TTF_RenderText_Solid(font, "Game Over", textColor);
    m_gameOverText = SDL_CreateTextureFromSurface(&m_gameLayer.GetRenderer(), gameOverText);
    m_gameOverTextDim = std::make_tuple(gameOverText->w, gameOverText->h);
    SDL_FreeSurface(gameOverText);
    TTF_CloseFont(font);
}

StatePostGame::~StatePostGame() {
    SDL_DestroyTexture(m_gameOverText);
}

void StatePostGame::OnEnter() {
}

bool StatePostGame::OnEvent(SDL_Event const& event) {
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
        case SDLK_SPACE: m_exitPending = true; return true;
        }
    }
    if (event.type == SDL_KEYUP) {
        switch (event.key.keysym.sym) {
        case SDLK_SPACE:
            if (m_exitPending) {
                m_exitPending = false;
                m_stateMachine->StateTransition<StatePreGame>();
                return true;
            }
        }
    }
    return false;
}

void StatePostGame::Draw(SDL_Renderer* renderer) {
    SDL_Rect destRect{
        (m_gameLayer.GetLayerWidth() - std::get<0>(m_gameOverTextDim)) / 2,
        (m_gameLayer.GetLayerHeight() - std::get<1>(m_gameOverTextDim)) / 2,
        std::get<0>(m_gameOverTextDim),
        std::get<1>(m_gameOverTextDim)
    };
    SDL_RenderCopy(renderer, m_gameOverText, nullptr, &destRect);
}
