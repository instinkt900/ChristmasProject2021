#include "game_pch.h"
#include "widget_image.h"

WidgetImage::WidgetImage() {
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
