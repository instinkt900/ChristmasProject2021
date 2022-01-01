#include "game_pch.h"
#include "loading_layer.h"
#include "game_layer.h"
#include "layer_stack.h"
#include "audio_factory.h"

LoadingLayer::LoadingLayer(SDL_Renderer& renderer, AudioFactory& audioFactory)
    : m_renderer(renderer)
    , m_audioFactory(audioFactory) {
    m_loadingTexture = CreateTextureRef(&m_renderer, "title_raw.jpg");

    FontRef font = CreateFontRef("pilotcommand.ttf", 60);
    SDL_Color textColor{ 255, 255, 255, 255 };
    SDL_Color dropColor{ 0, 0, 0, 255 };
    SurfaceRef loadingText = CreateSurfaceRef(TTF_RenderText_Solid(font.get(), "Loading...", textColor));
    m_loadingText = CreateTextureRef(&m_renderer, loadingText);
    m_loadingTextDim = { loadingText->w, loadingText->h };

    loadingText = CreateSurfaceRef(TTF_RenderText_Solid(font.get(), "Loading...", dropColor));
    m_loadingTextDrop = CreateTextureRef(&m_renderer, loadingText);
}

LoadingLayer::~LoadingLayer() {
}

bool LoadingLayer::OnEvent(SDL_Event const& event) {
    return false;
}

void LoadingLayer::Update(uint32_t ticks) {
    if (m_loadingFinished) {
        auto gameLayer = std::make_unique<GameLayer>(m_renderer, m_audioFactory);
        m_layerStack->PushLayer(std::move(gameLayer));
        m_layerStack->RemoveLayer(this);
    }
}

void LoadingLayer::Draw(SDL_Renderer& renderer) {
    SDL_RenderCopy(&m_renderer, m_loadingTexture.get(), nullptr, nullptr);

    int const text1Width = m_loadingTextDim.x;
    int const text1Height = m_loadingTextDim.y;
    int const text1X = (GetWidth() - text1Width) / 2;
    int const text1Y = (GetHeight() - text1Height) / 2;

    SDL_Rect destRect1Drop{ text1X + 2, text1Y + 2, text1Width, text1Height };
    SDL_RenderCopy(&m_renderer, m_loadingTextDrop.get(), nullptr, &destRect1Drop);

    SDL_Rect destRect1{ text1X, text1Y, text1Width, text1Height };
    SDL_RenderCopy(&m_renderer, m_loadingText.get(), nullptr, &destRect1);
}

void LoadingLayer::OnAddedToStack(LayerStack* layerStack) {
    Layer::OnAddedToStack(layerStack);
    m_audioFactory.LoadAll([this]() {
        m_loadingFinished = true;
    });
}
