#include "game_pch.h"
#include "editor_layer.h"
#include "ui/layouts/layout.h"
#include "ui/node.h"
#include "ui/node_image.h"
#include "events/event_dispatch.h"
#include "im_animation_tracks.h"

class SequenceTest : public ImAnimation::IAnimation {
    int GetFrameMin() const override { return 0; }
    int GetFrameMax() const override { return 100; }

    void BeginEdit(int /*index*/) override {}
    void EndEdit() override {}

    int GetClipCount() const override { return static_cast<int>(clips.size()); }
    void GetClip(int clipIndex, int** startFrame, int** endFrame, char const** clipName, unsigned int* color) override {
        if (startFrame)
            *startFrame = &clips[clipIndex].startFrame;
        if (endFrame)
            *endFrame = &clips[clipIndex].endFrame;
        if (clipName)
            *clipName = clips[clipIndex].name.c_str();
        if (color)
            *color = clips[clipIndex].color;
    }

    const char* GetTrackLabel(int index) const override {
        return tracks[index].name.c_str();
    }

    int GetTrackCount() const override { return static_cast<int>(tracks.size()); }
    void GetKeyframes(int trackIndex, int** keyframes, int** keyframeTypes, int* keyframeCount) override {
        if (keyframes)
            *keyframes = tracks[trackIndex].keyframes.data();
        if (keyframeTypes)
            *keyframeTypes = tracks[trackIndex].keyframeTypes.data();
        if (keyframeCount)
            *keyframeCount = static_cast<int>(tracks[trackIndex].keyframes.size());
    }

    void Add(int /*type*/) override {}
    void Del(int /*index*/) override {}
    void Duplicate(int /*index*/) override {}

    void Copy() override {}
    void Paste() override {}

    size_t GetCustomHeight(int /*index*/) override { return 0; }
    void DoubleClick(int /*index*/) override {}
    void CustomDraw(int /*index*/, ImDrawList* /*draw_list*/, const ImRect& /*rc*/, const ImRect& /*legendRect*/, const ImRect& /*clippingRect*/, const ImRect& /*legendClippingRect*/) override {}
    void CustomDrawCompact(int /*index*/, ImDrawList* /*draw_list*/, const ImRect& /*rc*/, const ImRect& /*clippingRect*/) override {}

private:
    struct Track {
        std::string name;
        std::vector<int> keyframes;
        std::vector<int> keyframeTypes;
    };

    struct Clip {
        std::string name;
        int startFrame;
        int endFrame;
        unsigned int color;
    };

    std::vector<Clip> clips{
        Clip{ "Clip 1", 0, 25, 0xFFCCAA00 },
        Clip{ "Clip 2", 26, 50, 0xFF00AA22 },
        Clip{ "Clip 3", 80, 90, 0xFF00CCEE },
    };
    std::vector<Track> tracks{
        Track{ "Track 1", { 0, 5, 25, 30 }, { 0, 0, 0, 0 } },
        Track{ "Track 2", { 0, 19, 21, 55 }, { 0, 0, 0, 0 } },
        Track{ "Track 3", { 0, 20, 85, 99 }, { 0, 0, 0, 0 } },
        Track{ "Track 4", { 0, 1, 2, 3 }, { 0, 0, 0, 0 } },
    };
};

namespace ui {
    EditorLayer::EditorLayer() {
    }

    EditorLayer::~EditorLayer() {
    }

    bool EditorLayer::OnEvent(Event const& event) {
        EventDispatch dispatch(event);
        dispatch.Dispatch(&m_boundsWidget);
        dispatch.Dispatch(this, &EditorLayer::OnMouseDown);
        dispatch.Dispatch(this, &EditorLayer::OnMouseUp);
        dispatch.Dispatch(this, &EditorLayer::OnKey);
        return dispatch.GetHandled();
    }

    void EditorLayer::Update(uint32_t ticks) {
    }

    void EditorLayer::Draw(SDL_Renderer& renderer) {
        SDL_SetRenderDrawColor(&renderer, 0xAA, 0xAA, 0xAA, 0xFF);
        SDL_RenderClear(&renderer);

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Open..", "Ctrl+O")) {
                    m_fileDialog.SetTitle("Open..");
                    m_fileDialog.SetTypeFilters({ ".json" });
                    m_fileDialog.Open();
                    m_fileOpenMode = FileOpenMode::Layout;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        if (ImGui::Begin("Elements")) {
            if (ImGui::Button("Image")) {
                auto newImageNode = std::make_unique<ui::NodeImage>();
                auto& layoutRect = newImageNode->GetLayoutRect();
                layoutRect.topLeft.anchor = { 0.5f, 0.5f };
                layoutRect.bottomRight.anchor = { 0.5f, 0.5f };
                layoutRect.topLeft.offset = { -50, -50 };
                layoutRect.bottomRight.offset = { 50, 50 };
                newImageNode->SetShowRect(true);
                m_root->AddChild(std::move(newImageNode));
                m_root->RecalculateBounds();
            } else if (ImGui::Button("SubLayout")) {
                m_fileDialog.SetTitle("Open..");
                m_fileDialog.SetTypeFilters({ ".json" });
                m_fileDialog.Open();
                m_fileOpenMode = FileOpenMode::SubLayout;
            }
            ImGui::End();
        }

        m_fileDialog.Display();

        if (m_fileDialog.HasSelected()) {
            if (m_fileOpenMode == FileOpenMode::Layout) {
                //m_currentLayout = ui::LoadLayout(m_fileDialog.GetSelected().string());
                m_fileDialog.ClearSelected();
            } else if (m_fileOpenMode == FileOpenMode::SubLayout) {
                auto newSubLayout = ui::LoadLayout(m_fileDialog.GetSelected().string());
                auto newNode = std::make_unique<ui::Group>(newSubLayout);
                auto& layoutRect = newNode->GetLayoutRect();
                layoutRect.topLeft.anchor = { 0.5f, 0.5f };
                layoutRect.bottomRight.anchor = { 0.5f, 0.5f };
                layoutRect.topLeft.offset = { -50, -50 };
                layoutRect.bottomRight.offset = { 50, 50 };
                newNode->SetShowRect(true);
                m_root->AddChild(std::move(newNode));
                m_root->RecalculateBounds();
                m_fileDialog.ClearSelected();
            }
        }

        if (m_root) {
            m_root->Draw(renderer);
            m_boundsWidget.Draw(renderer);
            if (ImGui::Begin("Layout")) {
                m_root->DebugDraw();
                ImGui::End();
            }
        }

        static SequenceTest sequenceTest;
        static int curFrame = 0;
        static bool expanded = true;
        static int selectedEntry = 0;
        static int firstFrame = 0;
        if (ImGui::Begin("Animation")) {
            ImAnimation::Animation(&sequenceTest, &curFrame, &expanded, &firstFrame);
            ImGui::End();
        }
    }

    void EditorLayer::DebugDraw() {
    }

    void EditorLayer::OnAddedToStack(LayerStack* layerStack) {
        Layer::OnAddedToStack(layerStack);
        NewLayout();
    }

    void EditorLayer::OnRemovedFromStack() {
        Layer::OnRemovedFromStack();
    }

    void EditorLayer::NewLayout() {
        m_root = std::make_unique<ui::Group>();
        IntRect displayRect;
        displayRect.topLeft = { (GetWidth() - m_displayWidth) / 2, (GetHeight() - m_displayHeight) / 2 };
        displayRect.bottomRight = { (GetWidth() + m_displayWidth) / 2, (GetHeight() + m_displayHeight) / 2 };
        m_root->SetScreenRect(displayRect);
        m_root->SetShowRect(true);
    }

    bool EditorLayer::OnMouseDown(EventMouseDown const& event) {
        for (auto&& child : m_root->GetChildren()) {
            if (child->IsInBounds(event.GetPosition())) {
                m_selection = child;
                m_boundsWidget.SetSelection(m_selection.get());
                return true;
            }
        }
        m_selection = nullptr;
        m_boundsWidget.SetSelection(nullptr);
        return false;
    }

    bool EditorLayer::OnMouseUp(EventMouseUp const& event) {
        return false;
    }

    bool EditorLayer::OnKey(EventKey const& event) {
        if (event.GetAction() == KeyAction::Up) {
            switch (event.GetKey()) {
            case Key::Delete:
                if (m_selection) {
                    m_root->RemoveChild(m_selection);
                    m_selection = nullptr;
                    m_boundsWidget.SetSelection(nullptr);
                }
                return true;
            }
        }
        return false;
    }
}
