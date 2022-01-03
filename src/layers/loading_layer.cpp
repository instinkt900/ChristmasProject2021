#include "game_pch.h"
#include "loading_layer.h"
#include "game_layer.h"
#include "layer_stack.h"
#include "audio_factory.h"
#include "game.h"

LoadingLayer::LoadingLayer(Game& game)
    : m_game(game) {
    auto renderer = m_game.GetRenderer();

    FontRef font = CreateFontRef("pilotcommand.ttf", 60);
    SDL_Color textColor{ 255, 255, 255, 255 };
    SDL_Color dropColor{ 0, 0, 0, 255 };
    SurfaceRef loadingText = CreateSurfaceRef(TTF_RenderText_Solid(font.get(), "Loading...", textColor));
    m_loadingText = CreateTextureRef(renderer, loadingText);
    m_loadingTextDim = { loadingText->w, loadingText->h };

    loadingText = CreateSurfaceRef(TTF_RenderText_Solid(font.get(), "Loading...", dropColor));
    m_loadingTextDrop = CreateTextureRef(renderer, loadingText);
}

LoadingLayer::~LoadingLayer() {
}

bool LoadingLayer::OnEvent(Event const& event) {
    return false;
}

void LoadingLayer::Update(uint32_t ticks) {
    if (m_loadingFinished) {
        auto layerStack = m_layerStack;
        auto gameLayer = std::make_unique<GameLayer>(m_game);
        layerStack->PopLayer(); // removes 'this'
        layerStack->PopLayer(); // removes background layer
        layerStack->PushLayer(std::move(gameLayer));
    }
}

void LoadingLayer::Draw(SDL_Renderer& renderer) {
    int const text1Width = m_loadingTextDim.x;
    int const text1Height = m_loadingTextDim.y;
    int const text1X = (GetWidth() - text1Width) / 2;
    int const text1Y = (GetHeight() - text1Height) / 2;

    SDL_Rect destRect1Drop{ text1X + 2, text1Y + 2, text1Width, text1Height };
    SDL_RenderCopy(&renderer, m_loadingTextDrop.get(), nullptr, &destRect1Drop);

    SDL_Rect destRect1{ text1X, text1Y, text1Width, text1Height };
    SDL_RenderCopy(&renderer, m_loadingText.get(), nullptr, &destRect1);
}

void LoadingLayer::OnAddedToStack(LayerStack* layerStack) {
    Layer::OnAddedToStack(layerStack);
    auto& audioFactory = m_game.GetAudioFactory();
    audioFactory.LoadAll([this]() {
        m_loadingFinished = true;
    });
}
