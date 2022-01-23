#pragma once

#include "layers/layer.h"
#include "ui/layouts/layout_entity_group.h"

#include "imfilebrowser.h"

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
        ImGui::FileBrowser m_fileDialog;
        std::shared_ptr<LayoutEntityGroup> m_currentLayout;
    };
};
