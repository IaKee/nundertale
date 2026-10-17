#pragma once

#include <cctype>
#include <fstream>
#include <functional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>

namespace nundertale {

inline const char* DEFAULT_BATTLE_MUSIC = "assets/music/epic_background_loop.ogg";

struct MusicEntry {
    std::string path;
    float loopStart = 0.0f;
    float loopEnd = 0.0f;
};

inline bool shouldLoopSeek(float timePlayed, float loopEnd) {
    return loopEnd > 0.0f && timePlayed >= loopEnd;
}

inline std::string musicPathForId(const std::string& id) {
    if (id.empty() || id == "battle_default") {
        return DEFAULT_BATTLE_MUSIC;
    }
    return "assets/music/" + id + ".ogg";
}

inline std::string resolveMusicPath(
    const std::string& id,
    const std::function<bool(const std::string&)>& exists) {
    const std::string candidate = musicPathForId(id);
    if (!exists(candidate)) {
        return DEFAULT_BATTLE_MUSIC;
    }
    return candidate;
}

class MusicTable {
public:
    MusicTable() = default;

    bool parse(const std::string& jsonText) {
        std::unordered_map<std::string, MusicEntry> parsed;
        std::size_t pos = 0;
        skipWhitespace(jsonText, pos);
        if (!consume(jsonText, pos, '{')) return false;
        skipWhitespace(jsonText, pos);
        if (consume(jsonText, pos, '}')) {
            m_entries = std::move(parsed);
            return true;
        }

        while (pos < jsonText.size()) {
            std::string contextKey;
            if (!parseString(jsonText, pos, contextKey)) return false;
            skipWhitespace(jsonText, pos);
            if (!consume(jsonText, pos, ':')) return false;
            skipWhitespace(jsonText, pos);

            MusicEntry entry;
            if (!parseEntry(jsonText, pos, entry)) return false;
            parsed[contextKey] = entry;

            skipWhitespace(jsonText, pos);
            if (consume(jsonText, pos, '}')) {
                skipWhitespace(jsonText, pos);
                m_entries = std::move(parsed);
                return true;
            }
            if (!consume(jsonText, pos, ',')) return false;
            skipWhitespace(jsonText, pos);
        }
        return false;
    }

    bool loadFromFile(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) return false;
        std::ostringstream ss;
        ss << file.rdbuf();
        return parse(ss.str());
    }

    const std::unordered_map<std::string, MusicEntry>& entries() const {
        return m_entries;
    }

    void setEntry(const std::string& key, const MusicEntry& entry) {
        m_entries[key] = entry;
    }

    MusicEntry musicFor(
        const std::string& context,
        const std::function<bool(const std::string&)>& exists = [](const std::string&) { return true; }) const {
        // Direct lookup
        auto it = m_entries.find(context);
        if (it != m_entries.end() && !it->second.path.empty()) {
            return it->second;
        }

        // Context fallback chain:
        // overworld.<room> -> overworld.default -> hardcoded epic_background_loop
        if (context.rfind("overworld.", 0) == 0) {
            auto defIt = m_entries.find("overworld.default");
            if (defIt != m_entries.end() && !defIt->second.path.empty()) {
                return defIt->second;
            }
            return MusicEntry{ DEFAULT_BATTLE_MUSIC, 0.0f, 0.0f };
        }

        // battle.<id> -> BFL-007 resolveMusicPath(id, exists) -> battle.default -> hardcoded
        if (context.rfind("battle.", 0) == 0) {
            const std::string battleId = context.substr(7);
            const std::string candidate = musicPathForId(battleId);
            if (exists(candidate)) {
                return MusicEntry{ candidate, 0.0f, 0.0f };
            }
            auto defIt = m_entries.find("battle.default");
            if (defIt != m_entries.end() && !defIt->second.path.empty()) {
                return defIt->second;
            }
            return MusicEntry{ DEFAULT_BATTLE_MUSIC, 0.0f, 0.0f };
        }

        // General fallback
        return MusicEntry{ DEFAULT_BATTLE_MUSIC, 0.0f, 0.0f };
    }

private:
    std::unordered_map<std::string, MusicEntry> m_entries;

    static void skipWhitespace(const std::string& text, std::size_t& pos) {
        while (pos < text.size() && std::isspace(static_cast<unsigned char>(text[pos])) != 0) ++pos;
    }

    static bool consume(const std::string& text, std::size_t& pos, char expected) {
        if (pos < text.size() && text[pos] == expected) {
            ++pos;
            return true;
        }
        return false;
    }

    static bool parseString(const std::string& text, std::size_t& pos, std::string& out) {
        skipWhitespace(text, pos);
        if (!consume(text, pos, '"')) return false;
        out.clear();
        while (pos < text.size()) {
            const char ch = text[pos++];
            if (ch == '"') return true;
            if (ch == '\\' && pos < text.size()) {
                out.push_back(text[pos++]);
            } else {
                out.push_back(ch);
            }
        }
        return false;
    }

    static bool parseNumber(const std::string& text, std::size_t& pos, float& out) {
        skipWhitespace(text, pos);
        std::size_t start = pos;
        if (pos < text.size() && (text[pos] == '-' || text[pos] == '+')) {
            ++pos;
        }
        bool hasDigits = false;
        while (pos < text.size() && std::isdigit(static_cast<unsigned char>(text[pos])) != 0) {
            hasDigits = true;
            ++pos;
        }
        if (pos < text.size() && text[pos] == '.') {
            ++pos;
            while (pos < text.size() && std::isdigit(static_cast<unsigned char>(text[pos])) != 0) {
                hasDigits = true;
                ++pos;
            }
        }
        if (!hasDigits) return false;
        std::string sub = text.substr(start, pos - start);
        out = std::stof(sub);
        return true;
    }

    static bool parseEntry(const std::string& text, std::size_t& pos, MusicEntry& entry) {
        skipWhitespace(text, pos);
        if (!consume(text, pos, '{')) return false;
        skipWhitespace(text, pos);
        if (consume(text, pos, '}')) return true;

        while (pos < text.size()) {
            std::string key;
            if (!parseString(text, pos, key)) return false;
            skipWhitespace(text, pos);
            if (!consume(text, pos, ':')) return false;
            skipWhitespace(text, pos);

            if (key == "path") {
                if (!parseString(text, pos, entry.path)) return false;
            } else if (key == "loopStart") {
                if (!parseNumber(text, pos, entry.loopStart)) return false;
            } else if (key == "loopEnd") {
                if (!parseNumber(text, pos, entry.loopEnd)) return false;
            } else {
                // Skip unknown value: string, number, or boolean/null
                if (pos < text.size() && text[pos] == '"') {
                    std::string dummy;
                    if (!parseString(text, pos, dummy)) return false;
                } else {
                    float dummy;
                    if (!parseNumber(text, pos, dummy)) return false;
                }
            }

            skipWhitespace(text, pos);
            if (consume(text, pos, '}')) return true;
            if (!consume(text, pos, ',')) return false;
            skipWhitespace(text, pos);
        }
        return false;
    }
};

inline MusicTable& globalMusicTable() {
    static MusicTable table;
    return table;
}

inline MusicEntry musicFor(
    const std::string& context,
    const std::function<bool(const std::string&)>& exists = [](const std::string&) { return true; }) {
    return globalMusicTable().musicFor(context, exists);
}

} // namespace nundertale
