#pragma once

#include "events/event_listener.h"
#include "events/event_mouse.h"

namespace ui {
    class Node;
    class BoundsHandle;

    class BoundsWidget : public EventListener {
    public:
        BoundsWidget();
        ~BoundsWidget();

        void SetSelection(Node* selection);

        bool OnEvent(Event const& event) override;
        void Draw(SDL_Renderer& renderer);

    private:
        std::array<std::unique_ptr<BoundsHandle>, 16> m_handles;
        Node* m_selectedNode = nullptr;

        bool m_holding = false;
        bool OnMouseDown(EventMouseDown const& event);
        bool OnMouseUp(EventMouseUp const& event);
        bool OnMouseMove(EventMouseMove const& event);
    };
}
