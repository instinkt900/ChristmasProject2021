#pragma once

inline void ImGuiInspectMember(char const* name, bool& value) {
    ImGui::Checkbox(name, &value);
}

inline void ImGuiInspectMember(char const* name, int& value) {
    ImGui::InputInt(name, &value);
}

inline void ImGuiInspectMember(char const* name, int const& value) {
    ImGui::LabelText(name, "%d", value);
}

inline void ImGuiInspectMember(char const* name, float& value) {
    ImGui::InputFloat(name, &value);
}

inline void ImGuiInspectMember(char const* name, char const* value) {
    ImGui::LabelText(name, "%s", value);
}

inline void ImGuiInspectMember(char const* name, std::string const& value) {
    ImGui::LabelText(name, "%s", value.c_str());
}

inline void ImGuiInspectMember(char const* name, entt::entity value) {
    ImGui::LabelText(name, "%d", static_cast<uint32_t>(value));
}

inline void ImGuiInspectMember(char const* name, SDL_Rect& value) {
    if (ImGui::CollapsingHeader(name)) {
        ImGuiInspectMember("x", value.x);
        ImGuiInspectMember("y", value.y);
        ImGuiInspectMember("w", value.w);
        ImGuiInspectMember("h", value.h);
    }
}

inline void ImGuiInspectMember(char const* name, TextureRef& value) {
    ImGui::Image(value.get(), ImVec2(100, 100));
}

template <typename T>
void ImGuiInspectMember(char const* name, std::vector<T>& value) {
    if (ImGui::CollapsingHeader(name)) {
        for (int i = 0; i < value.size(); ++i) {
            auto const label = fmt::format("{}", i);
            ImGuiInspectMember(label.c_str(), value[i]);
        }
    }
}

template <typename T>
void ImGuiInspectMember(char const* name, std::function<T> const& value) {
    ImGui::LabelText(name, "<function>");
}

template <typename T>
void ImGuiInspectMember(char const* name, T* value) {
    if (nullptr == value) {
        ImGui::LabelText(name, "nullptr");
    } else {
        ImGuiInspectMember(name, *value);
    }
}
