#pragma once

#include "events/event_listener.h"
#include "events/event_mouse.h"

namespace ui {
    class Node;

    struct BoundsHandleAnchor {
        int Top = 0;
        int Left = 0;
        int Bottom = 0;
        int Right = 0;
    };

    class BoundsHandle : public EventListener {
    public:
        BoundsHandle(BoundsHandleAnchor const& anchor);
        virtual ~BoundsHandle();

        virtual void SetTarget(Node* node);

        virtual bool OnEvent(Event const& event) override;
        virtual void Draw(SDL_Renderer& renderer) = 0;

        static BoundsHandleAnchor constexpr TopLeft{ 1, 1, 0, 0 };
        static BoundsHandleAnchor constexpr TopRight{ 1, 0, 0, 1 };
        static BoundsHandleAnchor constexpr BottomLeft{ 0, 1, 1, 0 };
        static BoundsHandleAnchor constexpr BottomRight{ 0, 0, 1, 1 };
        static BoundsHandleAnchor constexpr Top{ 1, 0, 0, 0 };
        static BoundsHandleAnchor constexpr Left{ 0, 1, 0, 0 };
        static BoundsHandleAnchor constexpr Bottom{ 0, 0, 1, 0 };
        static BoundsHandleAnchor constexpr Right{ 0, 0, 0, 1 };

    protected:
        Node* m_target = nullptr;
        BoundsHandleAnchor m_anchor;
        IntVec2 m_position;
        bool m_holding = false;

        virtual bool IsInBounds(IntVec2 const& pos) const = 0;

        bool OnMouseDown(EventMouseDown const& event);
        bool OnMouseUp(EventMouseUp const& event);
        virtual bool OnMouseMove(EventMouseMove const& event) = 0;
    };
}