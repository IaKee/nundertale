#pragma once

#include "IResourceService.hpp"
#include <unordered_map>
#include <string>

namespace nundertale {

class ResourceService : public IResourceService {
public:
    ResourceService();
    ~ResourceService() override;

    Texture2D getTexture(const std::string& path) override;
    Texture2D getMaskedTexture(const std::string& path, Color maskColor) override;

    Font getFont(const std::string& path, int fontSize = 32) override;
    Font getFont8Bit(int fontSize = 24) override;
    Font getFontTitle(int fontSize = 42) override;
    Font getFontTitleFore(int fontSize = 42) override;
    Font getFontShmup(int fontSize = 20) override;
    Font getDefaultFont() override;

    Sound getSound(const std::string& path) override;
    Music getMusic(const std::string& path) override;

    void drawText(Font font, const std::string& text, Vector2 pos, float fontSize, Color color, float spacing = 1.0f) override;
    void drawShakyText(Font font, const std::string& text, Vector2 pos, float fontSize, Color color, float shakeAmount = 1.0f, float spacing = 1.0f) override;
    void drawCenteredText(Font font, const std::string& text, float centerY, float fontSize, Color color, float spacing = 1.0f) override;
    float measureTextWidth(Font font, const std::string& text, float fontSize, float spacing = 1.0f) override;

    void clearCache() override;

private:
    std::unordered_map<std::string, Texture2D> m_textures;
    std::unordered_map<std::string, Font> m_fonts;
    std::unordered_map<std::string, Sound> m_sounds;
    std::unordered_map<std::string, Music> m_music;
    Font m_defaultFont{};
};

} // namespace nundertale
