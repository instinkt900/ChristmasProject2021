#include "game_pch.h"
#include "imgui_ext.h"
#include "imgui_internal.h"

namespace {
    struct FocusContextDetails {
        imgui_ext::FocusGroupContext* ExistingContext;
        std::string NewFocusLabel;
        std::map<std::string, std::function<void()>> LostFocusCallbacks;
    };

    FocusContextDetails g_focusContext;
}

namespace imgui_ext {
    using namespace ui;

    void FocusGroupBegin(FocusGroupContext* context) {
        g_focusContext.ExistingContext = context;
        g_focusContext.NewFocusLabel.clear();
        g_focusContext.LostFocusCallbacks.clear();
    }

    void FocusGroupInputText(char const* label, std::string const& target, std::function<void(std::string const&)> const& onChanged, std::function<void()> const& onLostFocus) {
        static char buffer[1024];
        strncpy(buffer, target.c_str(), 1024);

        g_focusContext.LostFocusCallbacks[label] = onLostFocus;
        if (ImGui::InputText(label, buffer, 1024)) {
            onChanged(buffer);
        }

        if (ImGui::IsItemFocused()) {
            g_focusContext.NewFocusLabel = label;
        }
    }

    void FocusGroupInputInt(char const* label, int value, std::function<void(int)> const& onChanged, std::function<void()> const& onLostFocus) {
        g_focusContext.LostFocusCallbacks[label] = onLostFocus;

        if (ImGui::InputInt(label, &value, 0)) {
            onChanged(value);
        }

        if (ImGui::IsItemFocused()) {
            g_focusContext.NewFocusLabel = label;
        }
    }

    void FocusGroupInputFloat(char const* label, float value, std::function<void(float)> const& onChanged, std::function<void()> const& onLostFocus) {
        g_focusContext.LostFocusCallbacks[label] = onLostFocus;

        if (ImGui::InputFloat(label, &value, 0)) {
            onChanged(value);
        }

        if (ImGui::IsItemFocused()) {
            g_focusContext.NewFocusLabel = label;
        }
    }

    void FocusGroupInputLayoutRect(char const* label, LayoutRect value, std::function<void(LayoutRect const&)> const& onChanged, std::function<void()> const& onLostFocus) {
        g_focusContext.LostFocusCallbacks[label] = onLostFocus;

        bool changed = false;
        if (ImGui::CollapsingHeader(label)) {
            if (ImGui::TreeNode("Anchor")) {
                ImGui::PushItemWidth(50);
                changed |= ImGui::InputFloat("Top", &value.anchor.topLeft.y, 0, 0, "%.2f");
                if (ImGui::IsItemFocused()) {
                    g_focusContext.NewFocusLabel = label;
                }
                changed |= ImGui::InputFloat("Left", &value.anchor.topLeft.x, 0, 0, "%.2f");
                if (ImGui::IsItemFocused()) {
                    g_focusContext.NewFocusLabel = label;
                }
                changed |= ImGui::InputFloat("Bottom", &value.anchor.bottomRight.y, 0, 0, "%.2f");
                if (ImGui::IsItemFocused()) {
                    g_focusContext.NewFocusLabel = label;
                }
                changed |= ImGui::InputFloat("Right", &value.anchor.bottomRight.x, 0, 0, "%.2f");
                if (ImGui::IsItemFocused()) {
                    g_focusContext.NewFocusLabel = label;
                }
                ImGui::PopItemWidth();
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Offset")) {
                ImGui::PushItemWidth(50);
                changed |= ImGui::InputFloat("Top", &value.offset.topLeft.y, 0, 0, "%.2f");
                if (ImGui::IsItemFocused()) {
                    g_focusContext.NewFocusLabel = label;
                }
                changed |= ImGui::InputFloat("Left", &value.offset.topLeft.x, 0, 0, "%.2f");
                if (ImGui::IsItemFocused()) {
                    g_focusContext.NewFocusLabel = label;
                }
                changed |= ImGui::InputFloat("Bottom", &value.offset.bottomRight.y, 0, 0, "%.2f");
                if (ImGui::IsItemFocused()) {
                    g_focusContext.NewFocusLabel = label;
                }
                changed |= ImGui::InputFloat("Right", &value.offset.bottomRight.x, 0, 0, "%.2f");
                if (ImGui::IsItemFocused()) {
                    g_focusContext.NewFocusLabel = label;
                }
                ImGui::PopItemWidth();
                ImGui::TreePop();
            }
        }

        if (changed) {
            onChanged(value);
        }
    }

    void FocusGroupEnd() {
        if (!g_focusContext.ExistingContext->currentFocusLabel.empty() && g_focusContext.ExistingContext->currentFocusLabel != g_focusContext.NewFocusLabel) {
            auto const it = g_focusContext.LostFocusCallbacks.find(g_focusContext.ExistingContext->currentFocusLabel);
            if (std::end(g_focusContext.LostFocusCallbacks) != it) {
                it->second();
            }
        }
        g_focusContext.ExistingContext->currentFocusLabel = g_focusContext.NewFocusLabel;
        g_focusContext.LostFocusCallbacks.clear();
    }

    bool InputString(char const* label, std::string* str) {
        static char buffer[1024];
        strncpy(buffer, str->c_str(), 1024);
        if (ImGui::InputText(label, buffer, 1024)) {
            *str = buffer;
            return true;
        }
        return false;
    }

    bool InputKeyframeValue(char const* label, ui::KeyframeValue* value) {
        if (value->index() == 0) {
            return ImGui::InputFloat(label, &std::get<float>(*value));
        } else {
            return InputString(label, &std::get<std::string>(*value));
        }
    }

    void InputIntVec2(char const* label, IntVec2* vec) {
        auto const inputWidth = ImMax(1.0f, (ImGui::CalcItemWidth() - 20) / 2.0f);
        ImGui::SetNextItemWidth(inputWidth);
        ImGui::PushID(&vec->x);
        ImGui::InputInt("", &vec->x, 0);
        ImGui::PopID();
        ImGui::SameLine();
        ImGui::SetNextItemWidth(20);
        ImGui::Text("x");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(inputWidth);
        ImGui::PushID(&vec->y);
        ImGui::InputInt("", &vec->y, 0);
        ImGui::PopID();
        ImGui::SameLine(0, 4);
        ImGui::Text(label);
    }

    void InputFloatVec2(char const* label, FloatVec2* vec) {
        auto const inputWidth = ImMax(1.0f, (ImGui::CalcItemWidth() - 20) / 2.0f);
        ImGui::SetNextItemWidth(inputWidth);
        ImGui::PushID(&vec->x);
        ImGui::InputFloat("", &vec->x, 0);
        ImGui::PopID();
        ImGui::SameLine();
        ImGui::SetNextItemWidth(20);
        ImGui::Text("x");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(inputWidth);
        ImGui::PushID(&vec->y);
        ImGui::InputFloat("", &vec->y, 0);
        ImGui::PopID();
        ImGui::SameLine(0, 4);
        ImGui::Text(label);
    }
}
