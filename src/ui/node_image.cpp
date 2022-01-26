#include "game_pch.h"
#include "node_image.h"
#include "layouts/layout_entity.h"
#include "debug/inspectors.h"
#include "layouts/layout_entity_image.h"

#include "imfilebrowser.h"

// TODO
// this is needed to create sdl textures when we load a widget but
// what probably needs to happen is we just reference a texture/sprite
// sheet here and create the texture externally.
extern SDL_Renderer* g_renderer;

namespace {
    ImGui::FileBrowser s_fileBrowser;
    ui::NodeImage* s_loadingNodeImage = nullptr;
}

namespace ui {
    NodeImage::NodeImage() {
    }

    NodeImage::NodeImage(std::shared_ptr<LayoutEntityImage> layoutEntity)
        : Node(layoutEntity) {
        m_texture = CreateTextureRef(g_renderer, layoutEntity->GetTexturePath().c_str());
        auto const& sourceRect = layoutEntity->GetSourceRect();
        m_sourceRect.x = sourceRect.topLeft.x;
        m_sourceRect.y = sourceRect.topLeft.y;
        m_sourceRect.w = sourceRect.bottomRight.x - sourceRect.topLeft.x;
        m_sourceRect.h = sourceRect.bottomRight.y - sourceRect.topLeft.y;
    }

    NodeImage::~NodeImage() {
    }

    void NodeImage::Draw(SDL_Renderer& renderer) {
        if (!IsVisible()) {
            return;
        }

        if (m_texture) {
            SDL_Rect destRect;
            destRect.x = m_screenRect.topLeft.x;
            destRect.y = m_screenRect.topLeft.y;
            destRect.w = m_screenRect.bottomRight.x - m_screenRect.topLeft.x;
            destRect.h = m_screenRect.bottomRight.y - m_screenRect.topLeft.y;
            SDL_RenderCopy(&renderer, m_texture.get(), &m_sourceRect, &destRect);
        }

        Node::Draw(renderer);
    }

    void NodeImage::DebugDraw() {
        Node::DebugDraw();
        if (ImGui::TreeNode("NodeImage")) {
            ImGuiInspectMember("texture", m_texture);
            ImGuiInspectMember("source rect", m_sourceRect);
            if (ImGui::Button("Load Image..")) {
                s_fileBrowser.SetTitle("Load Image..");
                s_fileBrowser.SetTypeFilters({ ".jpg", ".jpeg", ".png", ".bmp" });
                s_fileBrowser.Open();
                s_loadingNodeImage = this;
            }
            ImGui::TreePop();
        }

        if (s_loadingNodeImage == this) {
            s_fileBrowser.Display();
            if (s_fileBrowser.HasSelected()) {
                m_texture = CreateTextureRef(g_renderer, s_fileBrowser.GetSelected().string().c_str());
                s_fileBrowser.ClearSelected();
                s_loadingNodeImage = nullptr;
            }
        }
    }
}
