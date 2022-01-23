#include "game_pch.h"
#include "node_image.h"
#include "layouts/layout_entity.h"
#include "debug/inspectors.h"
#include "layouts/layout_entity_image.h"

// TODO
// this is needed to create sdl textures when we load a widget but
// what probably needs to happen is we just reference a texture/sprite
// sheet here and create the texture externally.
extern SDL_Renderer* g_renderer;

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
            ImGui::TreePop();
        }
    }
}
