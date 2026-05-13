#pragma once

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <map>
#include <string>
#include <utility>

namespace nundertale {

struct SpriteAnim {
    int row = 0;
    int frames = 1;
    float fps = 4.0f;
    bool loop = true;
};

struct SpriteSheetDef {
    std::string image;
    int cellW = 0;
    int cellH = 0;
    int originX = 0;
    int originY = 0;
    std::map<std::string, SpriteAnim> anims;
};

struct FrameRect {
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;
};

class SpriteSheetJsonParser {
public:
    explicit SpriteSheetJsonParser(const std::string& text) : m_text(text) {}

    bool parse(SpriteSheetDef& output) {
        SpriteSheetDef parsed;
        bool hasImage = false;
        bool hasCell = false;
        bool hasOrigin = false;
        bool hasAnims = false;

        skipWhitespace();
        if (!consume('{')) return false;
        skipWhitespace();

        if (!consume('}')) {
            while (true) {
                std::string key;
                if (!parseString(key)) return false;
                skipWhitespace();
                if (!consume(':')) return false;
                skipWhitespace();

                if (key == "image") {
                    if (!parseString(parsed.image)) return false;
                    hasImage = true;
                } else if (key == "cell") {
                    if (!parsePair(parsed.cellW, parsed.cellH)) return false;
                    hasCell = true;
                } else if (key == "origin") {
                    if (!parsePair(parsed.originX, parsed.originY)) return false;
                    hasOrigin = true;
                } else if (key == "anims") {
                    if (!parseAnimations(parsed.anims)) return false;
                    hasAnims = true;
                } else if (!skipValue()) {
                    return false;
                }

                skipWhitespace();
                if (consume('}')) break;
                if (!consume(',')) return false;
                skipWhitespace();
            }
        }

        skipWhitespace();
        if (!atEnd() || !hasImage || !hasCell || !hasOrigin || !hasAnims ||
            parsed.image.empty() || parsed.cellW <= 0 || parsed.cellH <= 0) {
            return false;
        }

        for (const auto& entry : parsed.anims) {
            const SpriteAnim& anim = entry.second;
            if (entry.first.empty() || anim.row < 0 || anim.frames <= 0 || anim.fps <= 0.0f) {
                return false;
            }
        }

        output = std::move(parsed);
        return true;
    }

private:
    bool parseAnimations(std::map<std::string, SpriteAnim>& animations) {
        if (!consume('{')) return false;
        skipWhitespace();
        if (consume('}')) return true;

        while (true) {
            std::string name;
            if (!parseString(name)) return false;
            skipWhitespace();
            if (!consume(':')) return false;
            skipWhitespace();

            SpriteAnim anim;
            if (!parseAnimation(anim)) return false;
            animations[name] = anim;

            skipWhitespace();
            if (consume('}')) return true;
            if (!consume(',')) return false;
            skipWhitespace();
        }
    }

    bool parseAnimation(SpriteAnim& anim) {
        if (!consume('{')) return false;
        skipWhitespace();
        if (consume('}')) return true;

        while (true) {
            std::string key;
            if (!parseString(key)) return false;
            skipWhitespace();
            if (!consume(':')) return false;
            skipWhitespace();

            if (key == "row") {
                if (!parseInteger(anim.row)) return false;
            } else if (key == "frames") {
                if (!parseInteger(anim.frames)) return false;
            } else if (key == "fps") {
                if (!parseNumber(anim.fps)) return false;
            } else if (key == "loop") {
                if (!parseBoolean(anim.loop)) return false;
            } else if (!skipValue()) {
                return false;
            }

            skipWhitespace();
            if (consume('}')) return true;
            if (!consume(',')) return false;
            skipWhitespace();
        }
    }

    bool parsePair(int& first, int& second) {
        if (!consume('[')) return false;
        skipWhitespace();
        if (!parseInteger(first)) return false;
        skipWhitespace();
        if (!consume(',')) return false;
        skipWhitespace();
        if (!parseInteger(second)) return false;
        skipWhitespace();
        return consume(']');
    }

    bool parseString(std::string& value) {
        skipWhitespace();
        if (!consume('"')) return false;
        value.clear();

        while (!atEnd()) {
            const char ch = m_text[m_pos++];
            if (ch == '"') return true;
            if (ch != '\\') {
                value.push_back(ch);
                continue;
            }
            if (atEnd()) return false;

            const char escaped = m_text[m_pos++];
            switch (escaped) {
                case '"': value.push_back('"'); break;
                case '\\': value.push_back('\\'); break;
                case '/': value.push_back('/'); break;
                case 'b': value.push_back('\b'); break;
                case 'f': value.push_back('\f'); break;
                case 'n': value.push_back('\n'); break;
                case 'r': value.push_back('\r'); break;
                case 't': value.push_back('\t'); break;
                default: return false;
            }
        }
        return false;
    }

    bool parseNumber(float& value) {
        skipWhitespace();
        if (atEnd()) return false;
        const char* begin = m_text.c_str() + m_pos;
        char* end = nullptr;
        value = std::strtof(begin, &end);
        if (end == begin || !std::isfinite(value)) return false;
        m_pos = static_cast<std::size_t>(end - m_text.c_str());
        return true;
    }

    bool parseInteger(int& value) {
        float parsed = 0.0f;
        if (!parseNumber(parsed) || std::floor(parsed) != parsed) return false;
        value = static_cast<int>(parsed);
        return true;
    }

    bool parseBoolean(bool& value) {
        skipWhitespace();
        if (m_pos + 4 <= m_text.size() && m_text.compare(m_pos, 4, "true") == 0) {
            value = true;
            m_pos += 4;
            return true;
        }
        if (m_pos + 5 <= m_text.size() && m_text.compare(m_pos, 5, "false") == 0) {
            value = false;
            m_pos += 5;
            return true;
        }
        return false;
    }

    bool skipValue() {
        skipWhitespace();
        if (atEnd()) return false;
        if (m_text[m_pos] == '"') {
            std::string ignored;
            return parseString(ignored);
        }
        if (m_text[m_pos] == '{') return skipComposite('{', '}');
        if (m_text[m_pos] == '[') return skipComposite('[', ']');

        const std::size_t start = m_pos;
        while (!atEnd() && m_text[m_pos] != ',' && m_text[m_pos] != '}' &&
               m_text[m_pos] != ']' && std::isspace(static_cast<unsigned char>(m_text[m_pos])) == 0) {
            ++m_pos;
        }
        return m_pos > start;
    }

    bool skipComposite(char open, char close) {
        if (!consume(open)) return false;
        int depth = 1;
        while (!atEnd() && depth > 0) {
            if (m_text[m_pos] == '"') {
                std::string ignored;
                if (!parseString(ignored)) return false;
                continue;
            }
            const char ch = m_text[m_pos++];
            if (ch == open) ++depth;
            if (ch == close) --depth;
        }
        return depth == 0;
    }

    void skipWhitespace() {
        while (!atEnd() && std::isspace(static_cast<unsigned char>(m_text[m_pos])) != 0) {
            ++m_pos;
        }
    }

    bool consume(char expected) {
        if (!atEnd() && m_text[m_pos] == expected) {
            ++m_pos;
            return true;
        }
        return false;
    }

    bool atEnd() const { return m_pos >= m_text.size(); }

    const std::string& m_text;
    std::size_t m_pos = 0;
};

inline bool parseSpriteSheetJson(const std::string& text, SpriteSheetDef& output) {
    return SpriteSheetJsonParser(text).parse(output);
}

inline FrameRect frameRect(const SpriteSheetDef& sheet, const std::string& animation, float time) {
    SpriteAnim fallback;
    const SpriteAnim* selected = &fallback;

    const auto requested = sheet.anims.find(animation);
    if (requested != sheet.anims.end()) {
        selected = &requested->second;
    } else {
        const auto idle = sheet.anims.find("idle");
        if (idle != sheet.anims.end()) selected = &idle->second;
    }

    const int frameCount = std::max(1, selected->frames);
    const float safeTime = std::max(0.0f, time);
    const float safeFps = std::max(0.0f, selected->fps);
    const float elapsedFrames = std::floor(safeTime * safeFps);
    int frame = 0;

    if (selected->loop) {
        frame = static_cast<int>(std::fmod(elapsedFrames, static_cast<float>(frameCount)));
    } else if (elapsedFrames >= static_cast<float>(frameCount - 1)) {
        frame = frameCount - 1;
    } else {
        frame = static_cast<int>(elapsedFrames);
    }

    return {
        frame * sheet.cellW,
        std::max(0, selected->row) * sheet.cellH,
        std::max(0, sheet.cellW),
        std::max(0, sheet.cellH)
    };
}

} // namespace nundertale
