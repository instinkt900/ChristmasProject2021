#include "game_pch.h"
#include "properties_editor.h"
#include "editor_layer.h"
#include "ui/layouts/layout_entity_image.h"

namespace ui {
    PropertiesEditor::PropertiesEditor(EditorLayer& editorLayer)
        : m_editorLayer(editorLayer) {
    }

    PropertiesEditor::~PropertiesEditor() {
    }

    void PropertiesEditor::Draw() {
        if (ImGui::Begin("Properties")) {
            if (auto selection = m_editorLayer.GetSelection()) {
                auto entity = selection->GetLayoutEntity();
                imgui_ext::FocusGroupBegin(&m_focusContext);
                DrawEntityProperties();
                switch (entity->GetType()) {
                case LayoutEntityType::Entity:
                    break;
                case LayoutEntityType::Group:
                    DrawGroupProperties();
                    break;
                case LayoutEntityType::Image:
                    DrawImageProperties();
                    break;
                }
                imgui_ext::FocusGroupEnd();
            }
        }
        ImGui::End();
    }

    void PropertiesEditor::DrawEntityProperties() {
        int const frame = m_editorLayer.GetSelectedFrame();
        auto selection = m_editorLayer.GetSelection();
        auto entity = selection->GetLayoutEntity();

        imgui_ext::FocusGroupInputText(
            "ID", entity->GetId(),
            [&](std::string const& value) { entity->SetId(value); },
            [&]() {});

        imgui_ext::FocusGroupInputLayoutRect(
            "Bounds", selection->GetLayoutRect(),
            [&](LayoutRect const& value) { m_editorLayer.BeginEditBounds(); selection->GetLayoutRect() = value; selection->RecalculateBounds(); },
            [&]() { m_editorLayer.EndEditBounds(); });
    }

    void PropertiesEditor::DrawGroupProperties() {
    }

    void PropertiesEditor::DrawImageProperties() {
    }
}
