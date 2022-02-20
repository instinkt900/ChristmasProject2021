#include "game_pch.h"
#include "menu_layer.h"
#include "game.h"
#include "layers/game_layer.h"
#include "ui/node.h"
#include "ui/layouts/layout.h"
#include "ui/layouts/layout_entity_group.h"
#include "events/event_animation.h"
#include "events/event_dispatch.h"

MenuLayer::MenuLayer(Game& game)
    : m_game(game) {

    auto entity = ui::LoadLayout("basic.json");
    m_rootWidget = entity->Instantiate();
    

    //auto imageWidget = std::make_shared<WidgetImage>();
    //imageWidget->SetImage(CreateTextureRef(game.GetRenderer(), "ship003.png"));

    //auto buttonWidget = std::make_shared<WidgetButton>();
    //auto& buttonBounds = buttonWidget->GetLayoutBounds();
    //buttonBounds.topLeft.anchor.x = 0.5f;
    //buttonBounds.topLeft.offset.x = -50;
    //buttonBounds.topLeft.anchor.y = 0.5f;
    //buttonBounds.topLeft.offset.y = -50;
    //buttonBounds.bottomRight.anchor.x = 0.5f;
    //buttonBounds.bottomRight.offset.x = 50;
    //buttonBounds.bottomRight.anchor.y = 0.5f;
    //buttonBounds.bottomRight.offset.y = 50;
    //buttonWidget->SetOnClickedCallback([this]() {
    //    auto layerStack = m_layerStack;
    //    auto& game = m_game;
    //    layerStack->PopLayer();
    //    layerStack->PopLayer();
    //    layerStack->PushLayer(std::make_unique<GameLayer>(game));
    //});

    //buttonWidget->AddChild(imageWidget);
    //m_rootWidget->AddChild(buttonWidget);
}

MenuLayer::~MenuLayer() {
}

bool MenuLayer::OnEvent(Event const& event) {
    EventDispatch dispatch(event);
    dispatch.Dispatch(m_rootWidget.get());
    dispatch.Dispatch(this, &MenuLayer::OnAnimEvent);
    return dispatch.GetHandled();
}

void MenuLayer::Update(uint32_t ticks) {
    m_rootWidget->Update(ticks);
}

void MenuLayer::Draw(SDL_Renderer& renderer) {
    m_rootWidget->Draw(renderer);
}

void MenuLayer::DebugDraw() {
    if (ImGui::CollapsingHeader("MenuLayer")) {
        if (ImGui::TreeNode("root")) {
            m_rootWidget->DebugDraw();
            ImGui::TreePop();
        }
    }
}

void MenuLayer::OnAddedToStack(LayerStack* stack) {
    Layer::OnAddedToStack(stack);

    //WidgetRect widgetRect;
    //widgetRect.topLeft = { 0, 0 };
    //widgetRect.bottomRight = { GetWidth(), GetHeight() };
    //m_rootWidget->SetScreenRect(widgetRect);
    m_rootWidget->SetShowRect(true);
    m_rootWidget->SetScreenRect({ 0, 0, GetWidth(), GetHeight() });
    m_rootWidget->SetEventHandler([&](ui::Node* fromNode, Event const& event) {
        return OnEvent(event);
    });
    m_rootWidget->SetAnimation("Simple");
}

bool MenuLayer::OnAnimEvent(EventAnimation const& event) {
    return true;
}
