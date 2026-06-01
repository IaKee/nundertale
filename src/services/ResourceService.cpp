#include "services/ResourceService.hpp"
#include "core/Constants.hpp"
#include <cmath>
#include <iostream>

namespace nundertale {

ResourceService::ResourceService() {
    m_defaultFont = GetFontDefault();
}

ResourceService::~ResourceService() {
    clearCache();
}

Texture2D ResourceService::getTexture(const std::string& path) {
    auto it = m_textures.find(path);
    if (it != m_textures.end()) {
        return it->second;
    }

    if (!FileExists(path.c_str())) {
        std::cerr << "[ResourceService] Texture not found: " << path << std::endl;
        Image placeholder = GenImageColor(32, 32, MAGENTA);
        Texture2D tex = LoadTextureFromImage(placeholder);
        UnloadImage(placeholder);
        m_textures[path] = tex;
        return tex;
    }

    Texture2D tex = LoadTexture(path.c_str());
    m_textures[path] = tex;
    return tex;
}

Texture2D ResourceService::getMaskedTexture(const std::string& path, Color maskColor) {
    std::string key = path + "_masked";
    auto it = m_textures.find(key);
    if (it != m_textures.end()) {
        return it->second;
    }

    if (!FileExists(path.c_str())) {
        return getTexture(path);
    }

    Image img = LoadImage(path.c_str());
    ImageColorReplace(&img, maskColor, Color{ 0, 0, 0, 0 });
    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);

    m_textures[key] = tex;
    return tex;
}

// BUG-34: the pixel fonts are rasterized with anti-aliasing, and they only land on whole pixels
// at a few sizes (8bitoperator: 16/32/48). At every other size the glyph edges come out gray,
// which reads as blurry text. Make every atlas pixel fully opaque or fully clear.
void ResourceService::hardenGlyphEdges(Font& font) {
    if (font.texture.id == 0) return;
    Image atlas = LoadImageFromTexture(font.texture);
    ImageFormat(&atlas, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    Color* pixels = static_cast<Color*>(atlas.data);
    const int count = atlas.width * atlas.height;
    for (int i = 0; i < count; ++i) {
        const bool ink = pixels[i].a >= 128;
        pixels[i] = ink ? WHITE : Color{ 255, 255, 255, 0 };
    }
    UnloadTexture(font.texture);
    font.texture = LoadTextureFromImage(atlas);
    UnloadImage(atlas);
}

// BUG-34: text drawn at a fractional position samples the atlas between pixels
static Vector2 snapToPixel(Vector2 pos) {
    return { std::round(pos.x), std::round(pos.y) };
}

Font ResourceService::getFont(const std::string& path, int fontSize) {
    std::string key = path + "_" + std::to_string(fontSize);
    auto it = m_fonts.find(key);
    if (it != m_fonts.end()) {
        return it->second;
    }

    if (FileExists(path.c_str())) {
        Font font = LoadFontEx(path.c_str(), fontSize, nullptr, 0);
        hardenGlyphEdges(font);
        SetTextureFilter(font.texture, TEXTURE_FILTER_POINT);
        m_fonts[key] = font;
        return font;
    }

    return m_defaultFont;
}

Font ResourceService::getFont8Bit(int fontSize) {
    return getFont("assets/fonts/8bitoperator.ttf", fontSize);
}

Font ResourceService::getFontTitle(int fontSize) {
    return getFont("assets/fonts/monster_friend.otf", fontSize);
}

Font ResourceService::getFontTitleFore(int fontSize) {
    return getFont("assets/fonts/monster_friend_fore.otf", fontSize);
}

Font ResourceService::getFontShmup(int fontSize) {
    return getFont("assets/fonts/gomarice_shmup_in_the_zone.ttf", fontSize);
}

Font ResourceService::getDefaultFont() {
    return getFont8Bit(24);
}

Sound ResourceService::getSound(const std::string& path) {
    auto it = m_sounds.find(path);
    if (it != m_sounds.end()) {
        return it->second;
    }

    if (FileExists(path.c_str())) {
        Sound snd = LoadSound(path.c_str());
        m_sounds[path] = snd;
        return snd;
    }

    Sound dummy{};
    return dummy;
}

Music ResourceService::getMusic(const std::string& path) {
    auto it = m_music.find(path);
    if (it != m_music.end()) {
        return it->second;
    }

    if (FileExists(path.c_str())) {
        Music mus = LoadMusicStream(path.c_str());
        m_music[path] = mus;
        return mus;
    }

    Music dummy{};
    return dummy;
}

void ResourceService::drawText(Font font, const std::string& text, Vector2 pos, float fontSize, Color color, float spacing) {
    // BUG-15 & GFX-003: Render at exact font.baseSize to prevent POINT distortion
    float effectiveSize = (font.baseSize > 0) ? (float)font.baseSize : ((fontSize > 0.0f) ? fontSize : 20.0f);
    DrawTextEx(font, text.c_str(), snapToPixel(pos), effectiveSize, spacing, color);
}

void ResourceService::drawShakyText(Font font, const std::string& text, Vector2 pos, float fontSize, Color color, float shakeAmount, float spacing) {
    // GFX-009: Render each glyph with jitter offset ±shakeAmount px
    float effectiveSize = (font.baseSize > 0) ? (float)font.baseSize : ((fontSize > 0.0f) ? fontSize : 20.0f);
    float curX = pos.x;
    float curY = pos.y;
    float lineH = effectiveSize + 4.0f;

    for (size_t i = 0; i < text.size(); ++i) {
        char c = text[i];
        if (c == '\n') {
            curX = pos.x;
            curY += lineH;
            continue;
        }

        std::string s(1, c);
        // Whole-pixel jitter (BUG-34): a fractional offset smears the glyph
        float jx = std::round(((float)GetRandomValue(-100, 100) / 100.0f) * shakeAmount);
        float jy = std::round(((float)GetRandomValue(-100, 100) / 100.0f) * shakeAmount);

        DrawTextEx(font, s.c_str(), snapToPixel({ curX + jx, curY + jy }), effectiveSize, spacing, color);
        curX += MeasureTextEx(font, s.c_str(), effectiveSize, spacing).x + spacing;
    }
}

void ResourceService::drawCenteredText(Font font, const std::string& text, float centerY, float fontSize, Color color, float spacing) {
    float effectiveSize = (font.baseSize > 0) ? (float)font.baseSize : ((fontSize > 0.0f) ? fontSize : 20.0f);
    Vector2 size = MeasureTextEx(font, text.c_str(), effectiveSize, spacing);
    Vector2 pos = { (VIRTUAL_WIDTH - size.x) * 0.5f, centerY };
    DrawTextEx(font, text.c_str(), snapToPixel(pos), effectiveSize, spacing, color);
}

float ResourceService::measureTextWidth(Font font, const std::string& text, float fontSize, float spacing) {
    float effectiveSize = (font.baseSize > 0) ? (float)font.baseSize : ((fontSize > 0.0f) ? fontSize : 20.0f);
    return MeasureTextEx(font, text.c_str(), effectiveSize, spacing).x;
}

void ResourceService::clearCache() {
    for (auto& pair : m_textures) {
        UnloadTexture(pair.second);
    }
    m_textures.clear();

    for (auto& pair : m_fonts) {
        UnloadFont(pair.second);
    }
    m_fonts.clear();

    for (auto& pair : m_sounds) {
        UnloadSound(pair.second);
    }
    m_sounds.clear();

    for (auto& pair : m_music) {
        UnloadMusicStream(pair.second);
    }
    m_music.clear();
}

} // namespace nundertale
