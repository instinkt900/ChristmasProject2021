#include "game_pch.h"
#include "editor_layer.h"
#include "ui/layouts/layout.h"
#include "ui/node.h"
#include "ui/node_image.h"
#include "events/event_dispatch.h"
#include "animation_widget.h"
#include "ui/layouts/animation_clip.h"
#include "ui/layouts/layout_entity_image.h"
#include "ui/editor/actions/add_action.h"
#include "ui/editor/actions/delete_action.h"
#include "ui/editor/actions/composite_action.h"
#include "ui/editor/actions/modify_keyframe_action.h"
#include "ui/editor/actions/add_keyframe_action.h"
#include "bounds_widget.h"
#include "properties_editor.h"

namespace ui {
    EditorLayer::EditorLayer()
        : m_fileDialog(ImGuiFileBrowserFlags_EnterNewFilename)
        , m_boundsWidget(std::make_unique<BoundsWidget>(*this))
        , m_animationWidget(std::make_unique<AnimationWidget>(*this))
        , m_propertiesEditor(std::make_unique<PropertiesEditor>(*this)) {
    }

    EditorLayer::~EditorLayer() {
    }

    bool EditorLayer::OnEvent(Event const& event) {
        EventDispatch dispatch(event);
        dispatch.Dispatch(m_boundsWidget.get());
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
                if (ImGui::MenuItem("New", "Ctrl+N")) {
                    NewLayout();
                } else if (ImGui::MenuItem("Open..", "Ctrl+O")) {
                    m_fileDialog.SetTitle("Open..");
                    m_fileDialog.SetTypeFilters({ ".json" });
                    m_fileDialog.Open();
                    m_fileOpenMode = FileOpenMode::Layout;
                } else if (ImGui::MenuItem("Save..", "Ctrl+S")) {
                    m_fileDialog.SetTitle("Save..");
                    m_fileDialog.SetTypeFilters({ ".json" });
                    m_fileDialog.Open();
                    m_fileOpenMode = FileOpenMode::Save;
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
            } else if (m_fileOpenMode == FileOpenMode::Save) {
                SaveLayout(m_fileDialog.GetSelected().string().c_str());
                m_fileDialog.ClearSelected();
            }
        }

        if (m_root) {
            m_root->Draw(renderer);
        }

        m_boundsWidget->Draw(renderer);
        m_animationWidget->Draw();
        m_propertiesEditor->Draw();
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
            m_animationWidget->OnUndo();
        }
    }

    void EditorLayer::RedoEditAction() {
        if (m_actionIndex < (static_cast<int>(m_editActions.size()) - 1)) {
            ++m_actionIndex;
            m_editActions[m_actionIndex]->Do();
            Refresh();
            m_animationWidget->OnRedo();
        }
    }

    void EditorLayer::ClearEditActions() {
        m_editActions.clear();
        m_actionIndex = -1;
    }

    void EditorLayer::NewLayout() {
        m_rootLayout = std::make_shared<LayoutEntityGroup>(LayoutRect{});
        m_selectedFrame = 0;
        m_editActions.clear();
        m_selection = nullptr;
        Rebuild();
    }

    void EditorLayer::LoadLayout(char const* path) {
        m_rootLayout = ui::LoadLayout(path);
        m_selectedFrame = 0;
        m_editActions.clear();
        m_selection = nullptr;
        Rebuild();
    }

    void EditorLayer::SaveLayout(char const* path) {
        std::ofstream ofile(path);
        if (!ofile.is_open()) {
            return;
        }

        nlohmann::json json = m_rootLayout->Serialize();
        ofile << json;
    }

    void EditorLayer::AddSubLayout(char const* path) {
        auto newSubLayout = ui::LoadLayout(path);
        LayoutRect bounds;
        bounds.anchor.topLeft = { 0.5f, 0.5f };
        bounds.anchor.bottomRight = { 0.5f, 0.5f };
        bounds.offset.topLeft = { -50, -50 };
        bounds.offset.bottomRight = { 50, 50 };
        newSubLayout->SetBounds(bounds, 0);

        auto instance = newSubLayout->Instantiate();
        instance->SetShowRect(true);

        auto addAction = std::make_unique<AddAction>(std::move(instance), m_root);
        addAction->Do();
        AddEditAction(std::move(addAction));

        m_root->RecalculateBounds();
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
    }

    bool EditorLayer::OnMouseDown(EventMouseDown const& event) {

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
                    SetSelection(nullptr);
                }
                return true;
            case Key::Z:
                UndoEditAction();
                return true;
            case Key::Y:
                RedoEditAction();
                return true;
            }
        }
        return false;
    }

    void EditorLayer::SetSelection(std::shared_ptr<Node> selection) {
        if (selection != m_selection) {
            if (m_editBoundsContext) {
                EndEditBounds();
            }
        }
        m_selection = selection;
    }

    void EditorLayer::BeginEditBounds() {
        if (!m_editBoundsContext && m_selection) {
            m_editBoundsContext = std::make_unique<EditBoundsContext>();
            m_editBoundsContext->entity = m_selection->GetLayoutEntity();
            m_editBoundsContext->originalRect = m_selection->GetLayoutRect();
        } else {
            assert(m_editBoundsContext->entity == m_selection->GetLayoutEntity());
        }
    }

    void EditorLayer::EndEditBounds() {
        if (!m_editBoundsContext) {
            return;
        }
        auto entity = m_editBoundsContext->entity;
        auto& tracks = entity->GetAnimationTracks();
        int const frameNo = m_selectedFrame;
        std::unique_ptr<CompositeAction> editAction = std::make_unique<CompositeAction>();

        auto const SetTrackValue = [&](AnimationTrack::Target target, float value) {
            auto track = tracks.at(target);
            if (auto keyframePtr = track->GetKeyframe(frameNo)) {
                // keyframe exists
                auto const oldValue = keyframePtr->m_value;
                keyframePtr->m_value = value;
                editAction->GetActions().push_back(std::make_unique<ModifyKeyframeAction>(entity, target, frameNo, oldValue, value));
            } else {
                // no keyframe
                auto& keyframe = track->GetOrCreateKeyframe(frameNo);
                keyframe.m_value = value;
                editAction->GetActions().push_back(std::make_unique<AddKeyframeAction>(entity, target, frameNo, value));
            }
        };

        auto const& newRect = m_selection->GetLayoutRect();
        auto const rectDelta = newRect - m_editBoundsContext->originalRect;

        if (rectDelta.anchor.topLeft.x != 0) {
            SetTrackValue(AnimationTrack::Target::LeftAnchor, newRect.anchor.topLeft.x);
        }
        if (rectDelta.anchor.topLeft.y != 0) {
            SetTrackValue(AnimationTrack::Target::TopAnchor, newRect.anchor.topLeft.y);
        }
        if (rectDelta.anchor.bottomRight.x != 0) {
            SetTrackValue(AnimationTrack::Target::RightAnchor, newRect.anchor.bottomRight.x);
        }
        if (rectDelta.anchor.bottomRight.y != 0) {
            SetTrackValue(AnimationTrack::Target::BottomAnchor, newRect.anchor.bottomRight.y);
        }
        if (rectDelta.offset.topLeft.x != 0) {
            SetTrackValue(AnimationTrack::Target::LeftOffset, newRect.offset.topLeft.x);
        }
        if (rectDelta.offset.topLeft.y != 0) {
            SetTrackValue(AnimationTrack::Target::TopOffset, newRect.offset.topLeft.y);
        }
        if (rectDelta.offset.bottomRight.x != 0) {
            SetTrackValue(AnimationTrack::Target::RightOffset, newRect.offset.bottomRight.x);
        }
        if (rectDelta.offset.bottomRight.y != 0) {
            SetTrackValue(AnimationTrack::Target::BottomOffset, newRect.offset.bottomRight.y);
        }

        if (!editAction->GetActions().empty()) {
            AddEditAction(std::move(editAction));
        }

        m_editBoundsContext.reset();
    }
}
