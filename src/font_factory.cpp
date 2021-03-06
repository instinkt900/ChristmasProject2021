#include "game_pch.h"
#include "font_factory.h"
#include "font.h"

FontFactory::FontFactory(SDL_Renderer& renderer)
    : m_renderer(renderer) {
    m_fontPaths["Pilot Command"] = "resources/fonts/pilotcommand.ttf";
    m_fontPaths["Daniel Davis"] = "resources/fonts/Daniel Davis.ttf";
    m_fontPaths["Game of Squids"] = "resources/fonts/Game Of Squids.ttf";
    m_fontPaths["Southern Aire"] = "resources/fonts/SouthernAire_Personal_Use_Only.ttf";
    m_fontPaths["28 Days Later"] = "resources/fonts/28 Days Later.ttf";
}

std::unique_ptr<moth_ui::IFont> FontFactory::GetDefaultFont(int size) {
    return GetFont(m_fontPaths.begin()->first.c_str(), size);
}

std::vector<std::string> FontFactory::GetFontNameList() {
    std::vector<std::string> nameList;
    for (auto& [fontName, fontPath] : m_fontPaths) {
        nameList.push_back(fontName);
    }
    return nameList;
}

std::unique_ptr<moth_ui::IFont> FontFactory::GetFont(char const* name, int size) {
    assert(!m_fontPaths.empty() && "No known fonts.");
    auto const it = m_fontPaths.find(name);
    if (std::end(m_fontPaths) == it) {
        return GetDefaultFont(size);
    }
    SDL_Color defaultColor{ 0x00, 0x00, 0x00, 0xFF };
    return std::make_unique<Font>(CreateCachedFontRef(&m_renderer, it->second.c_str(), size, defaultColor, TTF_STYLE_NORMAL));
}
