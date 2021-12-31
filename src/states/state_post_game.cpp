#include "game_pch.h"
#include "state_post_game.h"
#include "state_pre_game.h"
#include "layers/game_layer.h"
#include "ecs/components/components.h"

StatePostGame::StatePostGame(StateMachine* stateMachine, GameLayer& gameLayer)
    : State(stateMachine)
    , m_gameLayer(gameLayer) {
    FontRef font = CreateFontRef(gameLayer.GetWorldParameters().m_gameFontPath.c_str(), 80);
    SDL_Color textColor{ 255, 255, 255, 255 };
    SurfaceRef gameOverText = CreateSurfaceRef(TTF_RenderText_Solid(font.get(), "Game Over", textColor));
    m_gameOverText = CreateTextureRef(&m_gameLayer.GetRenderer(), gameOverText);
    m_gameOverTextDim = { gameOverText->w, gameOverText->h };

    font = CreateFontRef(gameLayer.GetWorldParameters().m_gameFontPath.c_str(), 30);
    SurfaceRef highScoreText = CreateSurfaceRef(TTF_RenderText_Solid(font.get(), "New high score!", textColor));
    m_highScoreText = CreateTextureRef(&m_gameLayer.GetRenderer(), highScoreText);
    m_highScoreTextDim = { highScoreText->w, highScoreText->h };
}

StatePostGame::~StatePostGame() {
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
        case SDLK_SPACE:
            m_exitPending = true;
            return true;
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

void StatePostGame::Draw(SDL_Renderer& renderer) {
    int const displayWidth = m_gameLayer.GetWidth();
    int const displayHeight = m_gameLayer.GetHeight();

    int const text1Width = m_gameOverTextDim.x;
    int const text1Height = m_gameOverTextDim.y;
    int const text1X = (displayWidth - text1Width) / 2;
    int const text1Y = (displayHeight - text1Height) / 2;

    int const text2Width = m_highScoreTextDim.x;
    int const text2Height = m_highScoreTextDim.y;
    int const text2X = (displayWidth - text2Width) / 2;
    int const text2Y = text1Y + text1Height;

    SDL_Rect destRect1{ text1X, text1Y, text1Width, text1Height };
    SDL_RenderCopy(&renderer, m_gameOverText.get(), nullptr, &destRect1);

    if (m_newHighScore) {
        SDL_Rect destRect2{ text2X, text2Y, text2Width, text2Height };
        SDL_RenderCopy(&renderer, m_highScoreText.get(), nullptr, &destRect2);
    }
}
