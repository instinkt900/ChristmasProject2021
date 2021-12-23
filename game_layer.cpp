#include "game_layer.h"

#include <SDL_image.h>

#include "components.h"
#include "layer_stack.h"

GameLayer::GameLayer(SDL_Renderer* renderer) {
    Setup(renderer);
}

GameLayer::~GameLayer() {

}

bool GameLayer::OnEvent(SDL_Event& event) {
    if (m_inputSystem.OnEvent(event)) {
        return true;
    }
    return false;
}

void GameLayer::Update(uint32_t ticks) {
    m_inputSystem.Update(ticks, m_registry);
}

void GameLayer::Draw(SDL_Renderer* renderer) {
    auto const& cameraPosition = m_registry.get<PositionComponent>(m_cameraEntity);
    int const viewOffsetX = static_cast<int>(cameraPosition.x) - GetLayerWidth() / 2;
    int const viewOffsetY = static_cast<int>(cameraPosition.y) - GetLayerHeight() / 2;

    m_registry.view<PositionComponent, SpriteComponent>().each([&renderer, viewOffsetX, viewOffsetY](auto entity, auto const& position, auto const& sprite) {
        SDL_Rect r {
            static_cast<int>(position.x) - viewOffsetX,
            static_cast<int>(position.y) - viewOffsetY,
            static_cast<int>(sprite.width),
            static_cast<int>(sprite.height)
        };
        SDL_RenderCopy(renderer, sprite.texture, nullptr, &r);
    });
}

void GameLayer::OnAddedToStack(LayerStack* layerStack) {
    m_layerStack = layerStack;
}

void GameLayer::OnRemovedFromStack() {
    m_layerStack = nullptr;
}

void GameLayer::Setup(SDL_Renderer* renderer) {
    // camera
    m_cameraEntity = m_registry.create();
    auto& cameraPosition = m_registry.emplace<PositionComponent>(m_cameraEntity);
    cameraPosition.x = 0;
    cameraPosition.y = 0;

    // player
    auto playerEntity = m_registry.create();
    auto& playerPosition = m_registry.emplace<PositionComponent>(playerEntity);
    playerPosition.x = 0;
    playerPosition.y = 0;

    m_registry.emplace<InputComponent>(playerEntity);

    auto& playerSprite = m_registry.emplace<SpriteComponent>(playerEntity);
    SDL_Surface* image = IMG_Load("dot.png");
    playerSprite.texture = SDL_CreateTextureFromSurface(renderer, image);
    playerSprite.width = 10;
    playerSprite.height = 10;
}

int GameLayer::GetLayerWidth() const {
    if (nullptr == m_layerStack) {
        return 0;
    }
    return m_layerStack->GetWidth();
}

int GameLayer::GetLayerHeight() const {
    if (nullptr == m_layerStack) {
        return 0;
    }
    return m_layerStack->GetHeight();
}