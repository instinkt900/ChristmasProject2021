#include "game_pch.h"
#include "menu_layer.h"
#include "game.h"
#include "ui/widget_image.h"

MenuLayer::MenuLayer(Game& game)
    : m_game(game) {

    m_rootWidget = std::make_shared<Widget>();
    auto imageWidget = std::make_shared<WidgetImage>();
    auto& imageBounds = imageWidget->GetLayoutBounds();
    imageBounds.topLeft.anchor.x = 0.5f;
    imageBounds.topLeft.offset.x = -50;
    imageBounds.topLeft.anchor.y = 0.5f;
    imageBounds.topLeft.offset.y = -50;
    imageBounds.bottomRight.anchor.x = 0.5f;
    imageBounds.bottomRight.offset.x = 50;
    imageBounds.bottomRight.anchor.y = 0.5f;
    imageBounds.bottomRight.offset.y = 50;
    imageWidget->SetImage(CreateTextureRef(game.GetRenderer(), "ship003.png"));
    m_rootWidget->AddChild(imageWidget);
}

MenuLayer::~MenuLayer() {
}

bool MenuLayer::OnEvent(Event const& event) {
    if (m_rootWidget->OnEvent(event)) {
        return true;
    }
    return false;
}

void MenuLayer::Update(uint32_t ticks) {
    m_rootWidget->Update(ticks);
}

void MenuLayer::Draw(SDL_Renderer& renderer) {
    m_rootWidget->Draw(renderer);
}

void MenuLayer::OnAddedToStack(LayerStack* stack) {
    Layer::OnAddedToStack(stack);

    WidgetRect widgetRect;
    widgetRect.topLeft = { 0, 0 };
    widgetRect.bottomRight = { GetWidth(), GetHeight() };
    m_rootWidget->SetScreenRect(widgetRect);
}
