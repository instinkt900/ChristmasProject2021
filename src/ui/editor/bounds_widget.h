#pragma once

#include "events/event_listener.h"
#include "events/event_mouse.h"
#include "ui/layouts/layout_types.h"

namespace ui {
    class Node;
    class BoundsHandle;
    class ChangeBoundsAction;
    class EditorLayer;
    class LayoutEntity;

    class BoundsWidget : public EventListener {
    public:
        BoundsWidget(EditorLayer& editorLayer);
        ~BoundsWidget();

        void SetSelection(std::shared_ptr<Node> selection);
        void BeginEdit();
        void EndEdit();

        bool OnEvent(Event const& event) override;
        void Draw(SDL_Renderer& renderer);

    private:
        EditorLayer& m_editorLayer;

        std::array<std::unique_ptr<BoundsHandle>, 16> m_handles;
        std::weak_ptr<Node> m_selectedNode;

        bool m_holding = false;
        bool OnMouseDown(EventMouseDown const& event);
        bool OnMouseUp(EventMouseUp const& event);
        bool OnMouseMove(EventMouseMove const& event);

        void CheckSelectionValid();
    };
}
