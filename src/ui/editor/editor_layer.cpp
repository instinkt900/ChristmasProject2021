#include "game_pch.h"
#include "editor_layer.h"
#include "ui/layouts/layout.h"
#include "ui/node.h"

namespace ui {
    EditorLayer::EditorLayer() {
    }

    EditorLayer::~EditorLayer() {
    }

    bool EditorLayer::OnEvent(Event const& event) {
        return false;
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

        m_fileDialog.Display();

        if (m_fileDialog.HasSelected()) {
            m_currentLayout = ui::LoadLayout(m_fileDialog.GetSelected().string());
            m_fileDialog.ClearSelected();
        }

        if (ImGui::Begin("Layout")) {
            if (m_currentLayout) {
            m_currentLayout->OnEditDraw();
            }
            ImGui::End();
        }
    }

    void EditorLayer::DebugDraw() {
    }

    void EditorLayer::OnAddedToStack(LayerStack* layerStack) {
        Layer::OnAddedToStack(layerStack);
    }

    void EditorLayer::OnRemovedFromStack() {
        Layer::OnRemovedFromStack();
    }
}
