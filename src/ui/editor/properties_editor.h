#pragma once

namespace ui {
    class EditorLayer;
    class LayoutEntity;
    class LayoutEntityGroup;
    class LayoutEntityImage;

    class PropertiesEditor {
    public:
        PropertiesEditor(EditorLayer& editorLayer);
        ~PropertiesEditor();

        void Draw();

    private:
        EditorLayer& m_editorLayer;

        imgui_ext::FocusGroupContext m_focusContext;

        void DrawEntityProperties();
        void DrawGroupProperties();
        void DrawImageProperties();
    };
}