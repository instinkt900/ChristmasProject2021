#include "game_pch.h"
#include "widget_image.h"

// TODO
// this is needed to create sdl textures when we load a widget but
// what probably needs to happen is we just reference a texture/sprite
// sheet here and create the texture externally.
extern SDL_Renderer* g_renderer;

WidgetImage::WidgetImage() {
}

WidgetImage::WidgetImage(WidgetDesc const& desc)
    : Widget(desc) {
    m_image = CreateTextureRef(g_renderer, desc.imagePath.c_str());
    m_sourceRect = std::make_unique<SDL_Rect>();
    m_sourceRect->x = desc.imageRect.position.x;
    m_sourceRect->y = desc.imageRect.position.y;
    m_sourceRect->w = desc.imageRect.dimensions.x;
    m_sourceRect->h = desc.imageRect.dimensions.y;
}

WidgetImage::~WidgetImage() {
}

void WidgetImage::Draw(SDL_Renderer& renderer) {
    if (m_image) {
        SDL_Rect destRect;
        destRect.x = m_screenRect.topLeft.x;
        destRect.y = m_screenRect.topLeft.y;
        destRect.w = m_screenRect.bottomRight.x - m_screenRect.topLeft.x;
        destRect.h = m_screenRect.bottomRight.y - m_screenRect.topLeft.y;
        SDL_RenderCopy(&renderer, m_image.get(), m_sourceRect.get(), &destRect);
    }
    Widget::Draw(renderer);
}

void WidgetImage::SetSourceRect(SDL_Rect const* rect) {
    if (nullptr == rect) {
        m_sourceRect.reset();
    } else {
        m_sourceRect = std::make_unique<SDL_Rect>(*rect);
    }
}
