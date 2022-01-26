#pragma once

#include "bounds_handle.h"

namespace ui {
    class OffsetBoundsHandle : public BoundsHandle {
    public:
        OffsetBoundsHandle(BoundsHandleAnchor const& anchor);
        virtual ~OffsetBoundsHandle();

        void Draw(SDL_Renderer& renderer) override;

    private:
        int m_size = 10;

        bool IsInBounds(IntVec2 const& pos) const override;
        bool OnMouseMove(EventMouseMove const& event) override;
    };
}
