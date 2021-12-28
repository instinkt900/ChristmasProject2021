#include "state_post_game.h"
#include "game_layer.h"
#include "components.h"
#include "state_pre_game.h"

#include <SDL_ttf.h>

StatePostGame::StatePostGame(StateMachine* stateMachine, GameLayer& gameLayer)
: State(stateMachine)
, m_gameLayer(gameLayer) {
    TTF_Font* font = TTF_OpenFont(gameLayer.GetWorldParameters().m_gameFontPath.c_str(), 80);
    SDL_Color textColor{ 255, 255, 255, 255 };
    SDL_Surface* gameOverText = nullptr;
    gameOverText = TTF_RenderText_Solid(font, "Game Over", textColor);
    m_gameOverText = SDL_CreateTextureFromSurface(&m_gameLayer.GetRenderer(), gameOverText);
    m_gameOverTextDim = std::make_tuple(gameOverText->w, gameOverText->h);
    SDL_FreeSurface(gameOverText);
    TTF_CloseFont(font);

    font = TTF_OpenFont(gameLayer.GetWorldParameters().m_gameFontPath.c_str(), 30);
    SDL_Surface* highScoreText = nullptr;
    highScoreText = TTF_RenderText_Solid(font, "New high score!", textColor);
    m_highScoreText = SDL_CreateTextureFromSurface(&m_gameLayer.GetRenderer(), highScoreText);
    m_highScoreTextDim = std::make_tuple(highScoreText->w, highScoreText->h);
    SDL_FreeSurface(highScoreText);
    TTF_CloseFont(font);
}

StatePostGame::~StatePostGame() {
    SDL_DestroyTexture(m_gameOverText);
    SDL_DestroyTexture(m_highScoreText);
}

void StatePostGame::OnEnter() {
    m_newHighScore = false;
    auto& worldState = m_gameLayer.GetWorldState();
    if (worldState.m_score > worldState.m_highScore) {
        worldState.m_highScore = worldState.m_score;
        m_newHighScore = true;
        m_gameLayer.SaveScore();
    }
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
    int const displayWidth = m_gameLayer.GetLayerWidth();
    int const displayHeight = m_gameLayer.GetLayerHeight();

    int const text1Width = std::get<0>(m_gameOverTextDim);
    int const text1Height = std::get<1>(m_gameOverTextDim);
    int const text1X = (displayWidth - text1Width) / 2;
    int const text1Y = (displayHeight - text1Height) / 2;

    int const text2Width = std::get<0>(m_highScoreTextDim);
    int const text2Height = std::get<1>(m_highScoreTextDim);
    int const text2X = (displayWidth - text2Width) / 2;
    int const text2Y = text1Y + text1Height;

    SDL_Rect destRect1{ text1X, text1Y, text1Width, text1Height };
    SDL_RenderCopy(renderer, m_gameOverText, nullptr, &destRect1);

    if (m_newHighScore) {
        SDL_Rect destRect2{ text2X, text2Y, text2Width, text2Height };
        SDL_RenderCopy(renderer, m_highScoreText, nullptr, &destRect2);
    }
}
