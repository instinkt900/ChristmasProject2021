#pragma once

#include "layers/layer.h"
#include "ui/layouts/layout_entity_group.h"
#include "ui/group.h"
#include "imfilebrowser.h"
#include "bounds_widget.h"
#include "events/event_mouse.h"
#include "events/event_key.h"
#include "properties_editor.h"

namespace ui {
    class AnimationWidget;
    class IEditorAction;
    class ChangeBoundsAction;

    class EditorLayer : public Layer {
    public:
        EditorLayer();
        virtual ~EditorLayer();

        bool OnEvent(Event const& event) override;

        void Update(uint32_t ticks) override;
        void Draw(SDL_Renderer& renderer) override;
        void DebugDraw() override;

        void OnAddedToStack(LayerStack* layerStack) override;
        void OnRemovedFromStack() override;

        bool UseRenderSize() const override { return false; }

        void AddEditAction(std::unique_ptr<IEditorAction>&& editAction);

        void SetSelectedFrame(int frameNo);
        int GetSelectedFrame() const { return m_selectedFrame; }

        void SetSelection(std::shared_ptr<Node> selection);
        std::shared_ptr<Node> GetSelection() const { return m_selection; }
        bool IsSelected(std::shared_ptr<Node> node) const { return m_selection == node; }

        void Refresh();

        void BeginEditBounds();
        void EndEditBounds();

    private:
        enum class FileOpenMode {
            Unknown,
            Layout,
            SubLayout,
            Image,
            Save,
        };
        ImGui::FileBrowser m_fileDialog;
        FileOpenMode m_fileOpenMode = FileOpenMode::Unknown;
        std::shared_ptr<LayoutEntityGroup> m_rootLayout;
        std::shared_ptr<Group> m_root;
        BoundsWidget m_boundsWidget;
        std::shared_ptr<Node> m_selection;
        std::unique_ptr<AnimationWidget> m_animationWidget;
        int m_selectedFrame = 0;

        int m_displayWidth = 200;
        int m_displayHeight = 200;

        std::vector<std::unique_ptr<IEditorAction>> m_editActions;
        int m_actionIndex = -1;

        PropertiesEditor m_propertiesEditor;

        void UndoEditAction();
        void RedoEditAction();
        void ClearEditActions();

        void NewLayout();
        void LoadLayout(char const* path);
        void SaveLayout(char const* path);
        void AddSubLayout(char const* path);
        void AddImage(char const* path);
        void Rebuild();

        bool OnMouseDown(EventMouseDown const& event);
        bool OnMouseUp(EventMouseUp const& event);
        bool OnKey(EventKey const& event);

        struct EditBoundsContext {
            std::shared_ptr<LayoutEntity> entity;
            LayoutRect originalRect;
        };
        std::unique_ptr<EditBoundsContext> m_editBoundsContext;
    };
};
