#include "game_pch.h"
#include "image_factory.h"
#include "image.h"

ImageFactory::ImageFactory(SDL_Renderer& renderer)
    : m_renderer(renderer) {
}

std::unique_ptr<moth_ui::IImage> ImageFactory::GetImage(std::filesystem::path const& path) {
    if (auto texture = CreateTextureRef(&m_renderer, path.string().c_str())) {
        moth_ui::IntVec2 textureDimensions{};
        SDL_QueryTexture(texture.get(), NULL, NULL, &textureDimensions.x, &textureDimensions.y);

        moth_ui::IntRect sourceRect{ { 0, 0 }, textureDimensions };
        return std::make_unique<Image>(texture, textureDimensions, sourceRect);
    }
    return nullptr;
}
