#include "game_pch.h"
#include "imgui_ext.h"

namespace {
    struct FocusContextDetails {
        imgui_ext::FocusGroupContext* ExistingContext;
        std::string NewFocusLabel;
        std::map<std::string, std::function<void()>> LostFocusCallbacks;
    };

    FocusContextDetails g_focusContext;
}

namespace imgui_ext {
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

    void FocusGroupEnd() {
        if (!g_focusContext.ExistingContext->currentFocusLabel.empty() && g_focusContext.ExistingContext->currentFocusLabel != g_focusContext.NewFocusLabel) {
            g_focusContext.LostFocusCallbacks[g_focusContext.ExistingContext->currentFocusLabel]();
        }
        g_focusContext.ExistingContext->currentFocusLabel = g_focusContext.NewFocusLabel;
        g_focusContext.LostFocusCallbacks.clear();
    }
}
