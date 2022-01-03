#pragma once

#include "widget.h"

class WidgetImage : public Widget {
public:
    WidgetImage();
    virtual ~WidgetImage();

    void Draw(SDL_Renderer& renderer) override;

    TextureRef GetImage() const { return m_image; }
    void SetImage(TextureRef image) { m_image = image; }

    SDL_Rect* GetSourceRect() const { return m_sourceRect.get(); }
    void SetSourceRect(SDL_Rect const* rect);

protected:
    TextureRef m_image;
    std::unique_ptr<SDL_Rect> m_sourceRect;
};
