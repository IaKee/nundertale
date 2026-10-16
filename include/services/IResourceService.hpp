#pragma once

#include <raylib.h>
#include <string>

namespace nundertale {

class IResourceService {
public:
    virtual ~IResourceService() = default;

    virtual Texture2D getTexture(const std::string& path) = 0;
    virtual Texture2D getMaskedTexture(const std::string& path, Color maskColor) = 0;

    virtual Font getFont(const std::string& path, int fontSize = 32) = 0;
    virtual Font getFont8Bit(int fontSize = 24) = 0;
    virtual Font getFontTitle(int fontSize = 42) = 0;
    virtual Font getFontTitleFore(int fontSize = 42) = 0;
    virtual Font getFontShmup(int fontSize = 20) = 0;
    virtual Font getDefaultFont() = 0;

    virtual Sound getSound(const std::string& path) = 0;
    virtual Music getMusic(const std::string& path) = 0;

    virtual void drawText(Font font, const std::string& text, Vector2 pos, float fontSize, Color color, float spacing = 1.0f) = 0;
    virtual void drawCenteredText(Font font, const std::string& text, float centerY, float fontSize, Color color, float spacing = 1.0f) = 0;
    virtual float measureTextWidth(Font font, const std::string& text, float fontSize, float spacing = 1.0f) = 0;

    virtual void clearCache() = 0;
};

} // namespace nundertale
