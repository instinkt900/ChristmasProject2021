#include "game_pch.h"
#include "editor_layer.h"
#include "ui/layouts/layout.h"
#include "ui/node.h"
#include "ui/node_image.h"
#include "events/event_dispatch.h"

namespace ui {
    EditorLayer::EditorLayer() {
    }

    EditorLayer::~EditorLayer() {
    }

    bool EditorLayer::OnEvent(Event const& event) {
        EventDispatch dispatch(event);
        dispatch.Dispatch(&m_boundsWidget);
        dispatch.Dispatch(this, &EditorLayer::OnMouseDown);
        dispatch.Dispatch(this, &EditorLayer::OnMouseUp);
        return dispatch.GetHandled();
    }

    void EditorLayer::Update(uint32_t ticks) {
    }

    void EditorLayer::Draw(SDL_Renderer& renderer) {
        SDL_SetRenderDrawColor(&renderer, 0xAA, 0xAA, 0xAA, 0xFF);
        SDL_RenderClear(&renderer);

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Open..", "Ctrl+O")) {
                    m_fileDialog.SetTitle("Open..");
                    m_fileDialog.SetTypeFilters({ ".json" });
                    m_fileDialog.Open();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        if (ImGui::Begin("Elements")) {
            if (ImGui::Button("Image")) {
                auto newImageNode = std::make_unique<ui::NodeImage>();
                auto& layoutRect = newImageNode->GetLayoutRect();
                layoutRect.topLeft.anchor = { 0.5f, 0.5f };
                layoutRect.bottomRight.anchor = { 0.5f, 0.5f };
                layoutRect.topLeft.offset = { -50, -50 };
                layoutRect.bottomRight.offset = { 50, 50 };
                newImageNode->SetShowRect(true);
                m_root->AddChild(std::move(newImageNode));
                m_root->RecalculateBounds();
            }
            ImGui::End();
        }

        m_fileDialog.Display();

        if (m_fileDialog.HasSelected()) {
            //m_currentLayout = ui::LoadLayout(m_fileDialog.GetSelected().string());
            m_fileDialog.ClearSelected();
        }

        if (m_root) {
            m_root->Draw(renderer);
            m_boundsWidget.Draw(renderer);
            if (ImGui::Begin("Layout")) {
                m_root->DebugDraw();
                ImGui::End();
            }
        }
    }

    void EditorLayer::DebugDraw() {
    }

    void EditorLayer::OnAddedToStack(LayerStack* layerStack) {
        Layer::OnAddedToStack(layerStack);
        NewLayout();
    }

    void EditorLayer::OnRemovedFromStack() {
        Layer::OnRemovedFromStack();
    }

    void EditorLayer::NewLayout() {
        m_root = std::make_unique<ui::Group>();
        IntRect displayRect;
        displayRect.topLeft = { (GetWidth() - m_displayWidth) / 2, (GetHeight() - m_displayHeight) / 2 };
        displayRect.bottomRight = { (GetWidth() + m_displayWidth) / 2, (GetHeight() + m_displayHeight) / 2 };
        m_root->SetScreenRect(displayRect);
        m_root->SetShowRect(true);
    }

    bool EditorLayer::OnMouseDown(EventMouseDown const& event) {
        for (auto&& child : m_root->GetChildren()) {
            if (child->IsInBounds(event.GetPosition())) {
                m_boundsWidget.SetSelection(child.get());
                return true;
            }
        }

        m_boundsWidget.SetSelection(nullptr);
        return false;
    }

    bool EditorLayer::OnMouseUp(EventMouseUp const& event) {
        return false;
    }
}
