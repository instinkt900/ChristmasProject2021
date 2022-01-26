#pragma once

#include "layers/layer.h"
#include "ui/layouts/layout_entity_group.h"
#include "ui/group.h"
#include "imfilebrowser.h"
#include "bounds_widget.h"
#include "events/event_mouse.h"
#include "events/event_key.h"

namespace ui {
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
        };
        ImGui::FileBrowser m_fileDialog;
        FileOpenMode m_fileOpenMode;
        std::unique_ptr<ui::Group> m_root;
        BoundsWidget m_boundsWidget;
        std::shared_ptr<Node> m_selection;

        int m_displayWidth = 200;
        int m_displayHeight = 200;

        void NewLayout();

        bool OnMouseDown(EventMouseDown const& event);
        bool OnMouseUp(EventMouseUp const& event);
        bool OnKey(EventKey const& event);
    };
};
