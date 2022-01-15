#pragma once

#include "layout_types.h"

inline void ImGuiInspectMember(char const* label, LayoutRect& widgetBounds) {
    ImGui::PushItemWidth(50);
    ImGui::PushID(&widgetBounds.topLeft.offset.y);
    ImGui::InputFloat("", &widgetBounds.topLeft.offset.y, 0, 0, "%.0f");
    ImGui::PopID();
    ImGui::SameLine();
    ImGui::InputFloat("Top", &widgetBounds.topLeft.anchor.y, 0, 0, "%.2f");
    ImGui::PushID(&widgetBounds.bottomRight.offset.y);
    ImGui::InputFloat("", &widgetBounds.bottomRight.offset.y, 0, 0, "%.0f");
    ImGui::PopID();
    ImGui::SameLine();
    ImGui::InputFloat("Bottom", &widgetBounds.bottomRight.anchor.y, 0, 0, "%.2f");
    ImGui::PushID(&widgetBounds.topLeft.offset.x);
    ImGui::InputFloat("", &widgetBounds.topLeft.offset.x, 0, 0, "%.0f");
    ImGui::PopID();
    ImGui::SameLine();
    ImGui::InputFloat("Left", &widgetBounds.topLeft.anchor.x, 0, 0, "%.2f");
    ImGui::PushID(&widgetBounds.bottomRight.offset.x);
    ImGui::InputFloat("", &widgetBounds.bottomRight.offset.x, 0, 0, "%.0f");
    ImGui::PopID();
    ImGui::SameLine();
    ImGui::InputFloat("Right", &widgetBounds.bottomRight.anchor.x, 0, 0, "%.2f");
    ImGui::PopItemWidth();
}

inline void ImGuiInspectMember(char const* label, IntRect& widgetRect) {
    ImGui::PushItemWidth(108);
    ImGui::InputInt("Top", &widgetRect.topLeft.y, 0);
    ImGui::InputInt("Left", &widgetRect.topLeft.x, 0);
    ImGui::InputInt("Bottom", &widgetRect.bottomRight.y, 0);
    ImGui::InputInt("Right", &widgetRect.bottomRight.x, 0);
    ImGui::PopItemWidth();
}
