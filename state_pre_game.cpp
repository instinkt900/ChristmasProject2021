#include "game_pch.h"
#include "state_pre_game.h"
#include "state_machine.h"
#include "state_game.h"
#include "game_layer.h"

StatePreGame::StatePreGame(StateMachine* stateMachine, GameLayer& gameLayer)
: State(stateMachine)
, m_gameLayer(gameLayer) {
    TTF_Font* font = TTF_OpenFont("pilotcommand.ttf", 80);
    SDL_Color textColor{ 255, 255, 255, 255 };
    SDL_Surface* countText[3] = { nullptr, nullptr, nullptr };
    countText[0] = TTF_RenderText_Solid(font, "1", textColor);
    countText[1] = TTF_RenderText_Solid(font, "2", textColor);
    countText[2] = TTF_RenderText_Solid(font, "3", textColor);
    for (int i = 0; i < 3; ++i) {
        m_countDownText[i] = SDL_CreateTextureFromSurface(&m_gameLayer.GetRenderer(), countText[i]);
        m_countDownTextDim[i] = std::make_tuple(countText[i]->w, countText[i]->h);
        SDL_FreeSurface(countText[i]);
    }
    TTF_CloseFont(font);
}

StatePreGame::~StatePreGame() {
    for (int i = 0; i < 3; ++i) {
        SDL_DestroyTexture(m_countDownText[i]);
    }
}

void StatePreGame::OnEnter() {
    m_timer = 3000;
    m_sfxTimer = 0;
    m_gameLayer.SetupLevel();
}

void StatePreGame::Update(uint32_t ticks, entt::registry& registry) {
    if (ticks > m_timer) {
        m_stateMachine->StateTransition<StateGame>();
        return;
    }
    m_timer -= ticks;

    if (ticks > m_sfxTimer) {
        Mix_PlayChannel(-1, m_gameLayer.GetCountSFX(), 0);
        m_sfxTimer += 1000;
    }
    m_sfxTimer -= ticks;
}

void StatePreGame::Draw(SDL_Renderer* renderer) {
    int const index = std::min(2, static_cast<int>(std::floor(m_timer / 1000)));
    SDL_Rect destRect{
        (m_gameLayer.GetWidth() - std::get<0>(m_countDownTextDim[index])) / 2,
        (m_gameLayer.GetHeight() - std::get<1>(m_countDownTextDim[index])) / 2,
        std::get<0>(m_countDownTextDim[index]),
        std::get<1>(m_countDownTextDim[index])
    };
    SDL_RenderCopy(renderer, m_countDownText[index], nullptr, &destRect);
}
