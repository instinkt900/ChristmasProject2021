#pragma once

#include "editor_action.h"
#include "ui/layouts/layout_types.h"

namespace ui {
    class Node;

    class ChangeBoundsAction : public IEditorAction {
    public:
        ChangeBoundsAction(std::shared_ptr<Node> node);
        virtual ~ChangeBoundsAction();

        void EndChange();
        bool IsZero() const;

        void Do() override;
        void Undo() override;

        void OnImGui() override;
    protected:
        std::shared_ptr<Node> m_node;
        LayoutRect m_delta;
    };
}
