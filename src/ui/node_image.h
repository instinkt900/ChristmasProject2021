#pragma once

#include "node.h"

class LayoutEntityImage;

class NodeImage : public Node {
public:
    NodeImage();
    NodeImage(std::shared_ptr<LayoutEntityImage> layoutEntity);
    virtual ~NodeImage();

    void Draw(SDL_Renderer& renderer) override;

    void DebugDraw();

protected:
    TextureRef m_texture;
    SDL_Rect m_sourceRect;
};
