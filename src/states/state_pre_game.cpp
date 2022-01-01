#include "game_pch.h"
#include "state_pre_game.h"
#include "state_machine.h"
#include "state_game.h"
#include "layers/game_layer.h"
#include "ecs/systems/animation_system.h"

StatePreGame::StatePreGame(StateMachine* stateMachine, GameLayer& gameLayer)
    : State(stateMachine)
    , m_gameLayer(gameLayer) {
    FontRef font = CreateFontRef("pilotcommand.ttf", 80);
    SDL_Color textColor{ 255, 255, 255, 255 };
    SurfaceRef countText[3];
    countText[0] = CreateSurfaceRef(TTF_RenderText_Solid(font.get(), "1", textColor));
    countText[1] = CreateSurfaceRef(TTF_RenderText_Solid(font.get(), "2", textColor));
    countText[2] = CreateSurfaceRef(TTF_RenderText_Solid(font.get(), "3", textColor));
    for (int i = 0; i < 3; ++i) {
        m_countDownText[i] = CreateTextureRef(&m_gameLayer.GetRenderer(), countText[i]);
        m_countDownTextDim[i] = { countText[i]->w, countText[i]->h };
    }
}

StatePreGame::~StatePreGame() {
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
        Mix_PlayChannel(-1, m_gameLayer.GetCountSFX().get(), 0);
        m_sfxTimer += 1000;
    }
    m_sfxTimer -= ticks;

    AnimationSystem::Update(ticks, m_gameLayer);
}

void StatePreGame::Draw(SDL_Renderer& renderer) {
    int const index = std::min(2, static_cast<int>(std::floor(m_timer / 1000)));
    SDL_Rect destRect{
        (m_gameLayer.GetWidth() - m_countDownTextDim[index].x) / 2,
        (m_gameLayer.GetHeight() - m_countDownTextDim[index].y) / 2,
        m_countDownTextDim[index].x,
        m_countDownTextDim[index].y
    };
    SDL_RenderCopy(&renderer, m_countDownText[index].get(), nullptr, &destRect);
}
