#pragma once

#include "layouts/layout_types.h"

inline bool ImGuiInspectMember(char const* label, ui::LayoutRect& widgetBounds) {
    bool changed = false;
    ImGui::PushID(label);
    if (ImGui::CollapsingHeader(label)) {
        if (ImGui::TreeNode("Anchor")) {
            ImGui::PushItemWidth(50);
            changed |= ImGui::InputFloat("Top", &widgetBounds.anchor.topLeft.y, 0, 0, "%.2f");
            changed |= ImGui::InputFloat("Left", &widgetBounds.anchor.topLeft.x, 0, 0, "%.2f");
            changed |= ImGui::InputFloat("Bottom", &widgetBounds.anchor.bottomRight.y, 0, 0, "%.2f");
            changed |= ImGui::InputFloat("Right", &widgetBounds.anchor.bottomRight.x, 0, 0, "%.2f");
            ImGui::PopItemWidth();
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Offset")) {
            ImGui::PushItemWidth(50);
            changed |= ImGui::InputFloat("Top", &widgetBounds.offset.topLeft.y, 0, 0, "%.2f");
            changed |= ImGui::InputFloat("Left", &widgetBounds.offset.topLeft.x, 0, 0, "%.2f");
            changed |= ImGui::InputFloat("Bottom", &widgetBounds.offset.bottomRight.y, 0, 0, "%.2f");
            changed |= ImGui::InputFloat("Right", &widgetBounds.offset.bottomRight.x, 0, 0, "%.2f");
            ImGui::PopItemWidth();
            ImGui::TreePop();
        }
    }
    ImGui::PopID();
    return changed;
}

inline bool ImGuiInspectMember(char const* label, IntRect& widgetRect) {
    bool changed = false;
    ImGui::PushID(label);
    if (ImGui::CollapsingHeader(label)) {
        ImGui::PushItemWidth(108);
        changed |= ImGui::InputInt("Top", &widgetRect.topLeft.y, 0);
        changed |= ImGui::InputInt("Left", &widgetRect.topLeft.x, 0);
        changed |= ImGui::InputInt("Bottom", &widgetRect.bottomRight.y, 0);
        changed |= ImGui::InputInt("Right", &widgetRect.bottomRight.x, 0);
        ImGui::PopItemWidth();
    }
    ImGui::PopID();
    return changed;
}
