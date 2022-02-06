#include "game_pch.h"
#include "editor_layer.h"
#include "ui/layouts/layout.h"
#include "ui/node.h"
#include "ui/node_image.h"
#include "events/event_dispatch.h"
#include "im_animation_tracks.h"
#include "animation_widget.h"
#include "ui/layouts/animation_clip.h"
#include "ui/layouts/layout_entity_image.h"
#include "ui/editor/actions/add_action.h"
#include "ui/editor/actions/delete_action.h"

namespace ui {
    EditorLayer::EditorLayer()
        : m_boundsWidget(*this) {
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
        ImGui::DockSpaceOverViewport(nullptr, ImGuiDockNodeFlags_PassthruCentralNode);

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
            bool displayChanged = false;
            displayChanged |= ImGui::InputInt("Display Width", &m_displayWidth);
            displayChanged |= ImGui::InputInt("Display Height", &m_displayHeight);
            if (displayChanged) {
                IntRect displayRect;
                displayRect.topLeft = { (GetWidth() - m_displayWidth) / 2, (GetHeight() - m_displayHeight) / 2 };
                displayRect.bottomRight = { (GetWidth() + m_displayWidth) / 2, (GetHeight() + m_displayHeight) / 2 };
                m_root->SetScreenRect(displayRect);
            }

            if (ImGui::Button("Image")) {
                m_fileDialog.SetTitle("Open..");
                m_fileDialog.SetTypeFilters({ ".jpg", ".jpeg", ".png", ".bmp" });
                m_fileDialog.Open();
                m_fileOpenMode = FileOpenMode::Image;
            } else if (ImGui::Button("SubLayout")) {
                m_fileDialog.SetTitle("Open..");
                m_fileDialog.SetTypeFilters({ ".json" });
                m_fileDialog.Open();
                m_fileOpenMode = FileOpenMode::SubLayout;
            }
        }
        ImGui::End();

        if (ImGui::Begin("Change Stack")) {
            int i = 0;
            for (auto&& edit : m_editActions) {
                ImGui::PushID(edit.get());
                if (i == m_actionIndex) {
                    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                }
                edit->OnImGui();
                if (i == m_actionIndex) {
                    ImGui::PopStyleColor();
                }
                ImGui::PopID();
                ++i;
            }
        }
        ImGui::End();

        m_fileDialog.Display();

        if (m_fileDialog.HasSelected()) {
            if (m_fileOpenMode == FileOpenMode::Layout) {
                LoadLayout(m_fileDialog.GetSelected().string().c_str());
                m_fileDialog.ClearSelected();
            } else if (m_fileOpenMode == FileOpenMode::SubLayout) {
                AddSubLayout(m_fileDialog.GetSelected().string().c_str());
                m_fileDialog.ClearSelected();
            } else if (m_fileOpenMode == FileOpenMode::Image) {
                AddImage(m_fileDialog.GetSelected().string().c_str());
                m_fileDialog.ClearSelected();
            }
        }

        if (m_root) {
            m_root->Draw(renderer);
            m_boundsWidget.Draw(renderer);
            if (ImGui::Begin("Layout")) {
                m_root->DebugDraw();
            }
            ImGui::End();
        }

        if (m_animationWidget) {
            m_animationWidget->Draw();
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

    void EditorLayer::AddEditAction(std::unique_ptr<IEditorAction>&& editAction) {
        // discard anything past the current action
        while ((static_cast<int>(m_editActions.size()) - 1) > m_actionIndex) {
            m_editActions.pop_back();
        }
        m_editActions.push_back(std::move(editAction));
        ++m_actionIndex;
    }

    void EditorLayer::SetSelectedFrame(int frameNo) {
        if (m_selectedFrame != frameNo) {
            m_selectedFrame = frameNo;
            Refresh();
        }
    }

    void EditorLayer::Refresh() {
        for (auto&& child : m_root->GetChildren()) {
            child->RefreshBounds(m_selectedFrame);
        }
    }

    void EditorLayer::UndoEditAction() {
        if (m_actionIndex >= 0) {
            m_editActions[m_actionIndex]->Undo();
            --m_actionIndex;
            Refresh();
        }
    }

    void EditorLayer::RedoEditAction() {
        if (m_actionIndex < (static_cast<int>(m_editActions.size()) - 1)) {
            ++m_actionIndex;
            m_editActions[m_actionIndex]->Do();
            Refresh();
        }
    }

    void EditorLayer::ClearEditActions() {
        m_editActions.clear();
        m_actionIndex = -1;
    }

    void EditorLayer::NewLayout() {
        m_rootLayout = std::make_shared<LayoutEntityGroup>(LayoutRect{});
        Rebuild();
    }

    void EditorLayer::LoadLayout(char const* path) {
        m_rootLayout = ui::LoadLayout(path);
        Rebuild();
    }

    void EditorLayer::AddSubLayout(char const* path) {
        auto newSubLayout = ui::LoadLayout(path);
        LayoutRect bounds;
        bounds.anchor.topLeft = { 0.5f, 0.5f };
        bounds.anchor.bottomRight = { 0.5f, 0.5f };
        bounds.offset.topLeft = { -50, -50 };
        bounds.offset.bottomRight = { 50, 50 };
        newSubLayout->SetBounds(bounds);

        auto instance = newSubLayout->Instantiate();
        instance->SetShowRect(true);

        auto addAction = std::make_unique<AddAction>(std::move(instance), m_root);
        addAction->Do();
        AddEditAction(std::move(addAction));

        m_root->RecalculateBounds();
        //m_animationWidget->Update();
    }

    void EditorLayer::AddImage(char const* path) {
        LayoutRect bounds;
        bounds.anchor.topLeft = { 0.5f, 0.5f };
        bounds.anchor.bottomRight = { 0.5f, 0.5f };
        bounds.offset.topLeft = { -50, -50 };
        bounds.offset.bottomRight = { 50, 50 };

        auto newImageLayout = std::make_shared<LayoutEntityImage>(bounds);
        newImageLayout->m_texturePath = path;

        auto instance = newImageLayout->Instantiate();
        instance->SetShowRect(true);

        auto addAction = std::make_unique<AddAction>(std::move(instance), m_root);
        addAction->Do();
        AddEditAction(std::move(addAction));

        m_root->RecalculateBounds();
        //m_animationWidget->Update();
    }

    void EditorLayer::Rebuild() {
        m_root = std::make_unique<Group>(m_rootLayout);
        IntRect displayRect;
        displayRect.topLeft = { (GetWidth() - m_displayWidth) / 2, (GetHeight() - m_displayHeight) / 2 };
        displayRect.bottomRight = { (GetWidth() + m_displayWidth) / 2, (GetHeight() + m_displayHeight) / 2 };
        m_root->SetScreenRect(displayRect);
        m_root->SetShowRect(true);
        for (auto&& child : m_root->GetChildren()) {
            child->SetShowRect(true);
        }

        m_animationWidget = std::make_unique<AnimationWidget>(*this, m_root.get());
    }

    bool EditorLayer::OnMouseDown(EventMouseDown const& event) {
        for (auto&& child : m_root->GetChildren()) {
            if (child->IsInBounds(event.GetPosition())) {
                m_selection = child;
                m_boundsWidget.SetSelection(m_selection);
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
                    auto delAction = std::make_unique<DeleteAction>(m_selection, m_root);
                    delAction->Do();
                    AddEditAction(std::move(delAction));
                    m_selection = nullptr;
                    m_boundsWidget.SetSelection(nullptr);
                    //m_animationWidget->Update();
                }
                return true;
            case Key::Z:
                UndoEditAction();
                //m_animationWidget->Update();
                return true;
            case Key::Y:
                RedoEditAction();
                //m_animationWidget->Update();
                return true;
            }
        }
        return false;
    }
}
