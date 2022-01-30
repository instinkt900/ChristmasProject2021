#pragma once

#include "layers/layer.h"
#include "ui/layouts/layout_entity_group.h"
#include "ui/group.h"
#include "imfilebrowser.h"
#include "bounds_widget.h"
#include "events/event_mouse.h"
#include "events/event_key.h"

namespace ui {
    class AnimationEditContext;

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

    private:
        enum class FileOpenMode {
            Layout,
            SubLayout,
            Image,
        };
        ImGui::FileBrowser m_fileDialog;
        FileOpenMode m_fileOpenMode;
        std::shared_ptr<LayoutEntityGroup> m_rootLayout;
        std::unique_ptr<Group> m_root;
        BoundsWidget m_boundsWidget;
        std::shared_ptr<Node> m_selection;
        std::unique_ptr<AnimationEditContext> m_animationEditorContext;

        int m_displayWidth = 200;
        int m_displayHeight = 200;

        void NewLayout();
        void LoadLayout(char const* path);
        void AddSubLayout(char const* path);
        void AddImage(char const* path);
        void Refresh();

        bool OnMouseDown(EventMouseDown const& event);
        bool OnMouseUp(EventMouseUp const& event);
        bool OnKey(EventKey const& event);
    };
};
