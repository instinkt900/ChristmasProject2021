#include "game_pch.h"
#include "editor_layer.h"
#include "ui/layouts/layout.h"
#include "ui/node.h"
#include "ui/node_image.h"
#include "events/event_dispatch.h"
#include "im_animation_tracks.h"
#include "animation_editor.h"
#include "ui/layouts/animation_clip.h"
#include "ui/layouts/layout_entity_image.h"

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
        dispatch.Dispatch(this, &EditorLayer::OnKey);
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
                    m_fileOpenMode = FileOpenMode::Layout;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        if (ImGui::Begin("Elements")) {
            if (ImGui::Button("Image")) {
                auto newImageLayout = std::make_unique<LayoutEntityImage>();
                auto& layoutRect = newImageLayout->GetBounds();
                layoutRect.topLeft.anchor = { 0.5f, 0.5f };
                layoutRect.bottomRight.anchor = { 0.5f, 0.5f };
                layoutRect.topLeft.offset = { -50, -50 };
                layoutRect.bottomRight.offset = { 50, 50 };
                m_rootLayout->GetChildren().push_back(std::move(newImageLayout));
                Refresh();
            } else if (ImGui::Button("SubLayout")) {
                m_fileDialog.SetTitle("Open..");
                m_fileDialog.SetTypeFilters({ ".json" });
                m_fileDialog.Open();
                m_fileOpenMode = FileOpenMode::SubLayout;
            }
            ImGui::End();
        }

        m_fileDialog.Display();

        if (m_fileDialog.HasSelected()) {
            if (m_fileOpenMode == FileOpenMode::Layout) {
                LoadLayout(m_fileDialog.GetSelected().string().c_str());
                m_fileDialog.ClearSelected();
            } else if (m_fileOpenMode == FileOpenMode::SubLayout) {
                auto newSubLayout = ui::LoadLayout(m_fileDialog.GetSelected().string());
                auto& layoutRect = newSubLayout->GetBounds();
                layoutRect.topLeft.anchor = { 0.5f, 0.5f };
                layoutRect.bottomRight.anchor = { 0.5f, 0.5f };
                layoutRect.topLeft.offset = { -50, -50 };
                layoutRect.bottomRight.offset = { 50, 50 };
                m_rootLayout->GetChildren().push_back(std::move(newSubLayout));
                Refresh();
                m_fileDialog.ClearSelected();
            }
        }

        if (m_root) {
            m_root->Draw(renderer);
            m_boundsWidget.Draw(renderer);
            if (ImGui::Begin("Layout")) {
                m_root->DebugDraw();
                ImGui::End();
            }
        }

        if (m_animationEditorContext) {
            m_animationEditorContext->Draw();
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
        m_rootLayout = std::make_shared<LayoutEntityGroup>();
        Refresh();
    }

    void EditorLayer::LoadLayout(char const* path) {
        m_rootLayout = ui::LoadLayout(path);
        Refresh();
    }

    void EditorLayer::Refresh() {
        m_root = std::make_unique<Group>(m_rootLayout);
        IntRect displayRect;
        displayRect.topLeft = { (GetWidth() - m_displayWidth) / 2, (GetHeight() - m_displayHeight) / 2 };
        displayRect.bottomRight = { (GetWidth() + m_displayWidth) / 2, (GetHeight() + m_displayHeight) / 2 };
        m_root->SetScreenRect(displayRect);
        m_root->SetShowRect(true);
        for (auto&& child : m_root->GetChildren()) {
            child->SetShowRect(true);
        }

        m_animationEditorContext = std::make_unique<AnimationEditContext>(m_root.get());
    }

    bool EditorLayer::OnMouseDown(EventMouseDown const& event) {
        for (auto&& child : m_root->GetChildren()) {
            if (child->IsInBounds(event.GetPosition())) {
                m_selection = child;
                m_boundsWidget.SetSelection(m_selection.get());
                return true;
            }
        }
        m_selection = nullptr;
        m_boundsWidget.SetSelection(nullptr);
        return false;
    }

    bool EditorLayer::OnMouseUp(EventMouseUp const& event) {
        return false;
    }

    bool EditorLayer::OnKey(EventKey const& event) {
        if (event.GetAction() == KeyAction::Up) {
            switch (event.GetKey()) {
            case Key::Delete:
                if (m_selection) {
                    auto selectedLayout = m_selection->GetLayoutEntity();
                    auto& children = m_rootLayout->GetChildren();
                    auto it = std::find_if(std::begin(children), std::end(children), [&selectedLayout](auto& child) { return child.get() == selectedLayout; });
                    if (std::end(children) != it) {
                        children.erase(it);
                    }
                    m_selection = nullptr;
                    m_boundsWidget.SetSelection(nullptr);
                    Refresh();
                }
                return true;
            }
        }
        return false;
    }
}
