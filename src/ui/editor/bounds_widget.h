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

        void BeginEdit();
        void EndEdit();

        bool OnEvent(Event const& event) override;
        void Draw(SDL_Renderer& renderer);

        EditorLayer& GetEditorLayer() { return m_editorLayer; }

        IntVec2 SnapToGrid(IntVec2 const& original);

    private:
        EditorLayer& m_editorLayer;

        std::array<std::unique_ptr<BoundsHandle>, 16> m_handles;

        bool m_holding = false;
        IntVec2 m_grabPosition;

        bool OnMouseDown(EventMouseDown const& event);
        bool OnMouseUp(EventMouseUp const& event);
        bool OnMouseMove(EventMouseMove const& event);

        void UpdatePosition(IntVec2 const& position);
    };
}
