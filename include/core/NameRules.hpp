#pragma once

#include <cctype>
#include <cstddef>
#include <fstream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace nundertale {

enum class NameEffect { None, Gold, Skyrim, Waifu, Darken, FrogSoul, CrimsonShake, RestartGame };

inline NameEffect nameEffectFromString(const std::string& str) {
    if (str == "gold") return NameEffect::Gold;
    if (str == "skyrim") return NameEffect::Skyrim;
    if (str == "waifu") return NameEffect::Waifu;
    if (str == "darken") return NameEffect::Darken;
    if (str == "frog_soul") return NameEffect::FrogSoul;
    if (str == "crimson_shake") return NameEffect::CrimsonShake;
    if (str == "restart_game") return NameEffect::RestartGame;
    return NameEffect::None;
}

inline const char* nameEffectToString(NameEffect effect) {
    switch (effect) {
        case NameEffect::Gold: return "gold";
        case NameEffect::Skyrim: return "skyrim";
        case NameEffect::Waifu: return "waifu";
        case NameEffect::Darken: return "darken";
        case NameEffect::FrogSoul: return "frog_soul";
        case NameEffect::CrimsonShake: return "crimson_shake";
        case NameEffect::RestartGame: return "restart_game";
        default: return "none";
    }
}

struct NameReaction {
    std::string tag;       // e.g. "[GOLD]" or "" (shown in yellow under the name on the confirm screen)
    std::string message;   // flavor line, "" = default
    bool allowed = true;   // false => confirm screen only offers "Go back"
    NameEffect effect = NameEffect::None;

    bool operator==(const NameReaction& other) const {
        return tag == other.tag && message == other.message &&
               allowed == other.allowed && effect == other.effect;
    }
    bool operator!=(const NameReaction& other) const {
        return !(*this == other);
    }
};

inline constexpr std::size_t NAME_MAX_LEN = 6;

inline std::string normalizeName(const std::string& raw) {
    std::string out = raw;
    for (char& c : out) {
        if (c >= 'a' && c <= 'z') {
            c = static_cast<char>(c - 'a' + 'A');
        }
    }
    return out;
}

struct NameRuleEntry {
    std::vector<std::string> names;
    NameReaction reaction;
};

class NameTableParser {
public:
    explicit NameTableParser(const std::string& text) : m_text(text), m_pos(0) {}

    bool parseEntries(std::vector<NameRuleEntry>& entries) {
        skipWhitespace();
        if (!consume('[')) return false;
        skipWhitespace();
        if (consume(']')) {
            skipWhitespace();
            return atEnd();
        }

        while (true) {
            NameRuleEntry entry;
            if (!parseEntry(entry)) return false;
            entries.push_back(std::move(entry));

            skipWhitespace();
            if (consume(']')) {
                skipWhitespace();
                return atEnd();
            }
            if (!consume(',')) return false;
            skipWhitespace();
        }
    }

private:
    bool parseEntry(NameRuleEntry& entry) {
        skipWhitespace();
        if (!consume('{')) return false;
        skipWhitespace();
        if (consume('}')) return true;

        entry.reaction = NameReaction{};
        std::string effectStr = "none";

        while (true) {
            std::string key;
            if (!parseString(key)) return false;
            skipWhitespace();
            if (!consume(':')) return false;
            skipWhitespace();

            if (key == "names") {
                if (!parseStringArray(entry.names)) return false;
            } else if (key == "tag") {
                if (!parseString(entry.reaction.tag)) return false;
            } else if (key == "message") {
                if (!parseString(entry.reaction.message)) return false;
            } else if (key == "allowed") {
                if (!parseBool(entry.reaction.allowed)) return false;
            } else if (key == "effect") {
                if (!parseString(effectStr)) return false;
                entry.reaction.effect = nameEffectFromString(effectStr);
            } else {
                if (!skipValue()) return false;
            }

            skipWhitespace();
            if (consume('}')) return true;
            if (!consume(',')) return false;
            skipWhitespace();
        }
    }

    bool parseStringArray(std::vector<std::string>& list) {
        skipWhitespace();
        if (!consume('[')) return false;
        skipWhitespace();
        if (consume(']')) return true;

        while (true) {
            std::string item;
            if (!parseString(item)) return false;
            list.push_back(std::move(item));

            skipWhitespace();
            if (consume(']')) return true;
            if (!consume(',')) return false;
            skipWhitespace();
        }
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

    bool parseBool(bool& value) {
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
        while (!atEnd() && m_text[m_pos] != ',' && m_text[m_pos] != '}' && m_text[m_pos] != ']') {
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
            else if (ch == close) --depth;
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

    bool atEnd() const {
        return m_pos >= m_text.size();
    }

    const std::string& m_text;
    std::size_t m_pos;
};

struct NameTable {
    std::vector<NameRuleEntry> entries;
    bool loaded = false;

    bool empty() const { return entries.empty(); }
    bool isLoaded() const { return loaded; }
    void clear() { entries.clear(); loaded = false; }

    bool parse(const std::string& jsonText) {
        NameTableParser parser(jsonText);
        std::vector<NameRuleEntry> parsedEntries;
        if (!parser.parseEntries(parsedEntries)) {
            return false;
        }
        entries = std::move(parsedEntries);
        loaded = true;
        return true;
    }

    const NameReaction* find(const std::string& rawName) const {
        const std::string target = normalizeName(rawName);
        for (const auto& entry : entries) {
            for (const auto& n : entry.names) {
                if (normalizeName(n) == target) {
                    return &entry.reaction;
                }
            }
        }
        return nullptr;
    }
};

inline NameTable& globalNameTable() {
    static NameTable instance;
    return instance;
}

inline void resetGlobalNames() {
    globalNameTable().clear();
}

inline bool loadNamesFromFile(const std::string& filePath, NameTable& table) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    std::ostringstream ss;
    ss << file.rdbuf();
    return table.parse(ss.str());
}

inline bool loadGlobalNames(const std::string& filePath = "data/names.json") {
    return loadNamesFromFile(filePath, globalNameTable());
}

inline bool lookupBuiltinReaction(const std::string& rawName, NameReaction& out) {
    const std::string name = normalizeName(rawName);

    if (name == "CHAD") {
        out = NameReaction{ "[GOLD]", "Gold plated. You are the ultimate gamer.", true, NameEffect::Gold };
        return true;
    }
    if (name == "GRUB") {
        out = NameReaction{ "[THE PROTAGONIST]", "Feels good man. Ready to climb down Mt. Dew.", true, NameEffect::FrogSoul };
        return true;
    }
    if (name == "GOBLIN" || name == "FROG") {
        out = NameReaction{ "[RIBBIT]", "Feels froggy man.", true, NameEffect::FrogSoul };
        return true;
    }
    if (name == "PEPE") {
        out = NameReaction{ "", "* Not that frog.", true, NameEffect::None };
        return true;
    }
    if (name == "SKYRIM") {
        out = NameReaction{ "[TAMRIEL]", "Hey you, you're finally awake...", true, NameEffect::Skyrim };
        return true;
    }
    if (name == "SANS") {
        out = NameReaction{ "[COMEDIAN]", "heh, you really think you have the backbone for this?", true, NameEffect::None };
        return true;
    }
    if (name == "FRISK") {
        out = NameReaction{ "[FALLEN CHILD]", "Warning: This mode triggers extreme allergy to fursuits.", true, NameEffect::None };
        return true;
    }
    if (name == "CHARA") {
        out = NameReaction{ "[THE FIRST]", "The true meaning of pure toxic gamer rage.", true, NameEffect::CrimsonShake };
        return true;
    }
    if (name == "FLOWEY") {
        out = NameReaction{ "[WEED]", "Not very original, are you?", true, NameEffect::None };
        return true;
    }
    if (name == "TORIEL") {
        out = NameReaction{ "[MOM]", "Furiell has already trademarked this name.", true, NameEffect::None };
        return true;
    }
    if (name == "ASGORE") {
        out = NameReaction{ "[KING]", "King of questionable herbal tea.", true, NameEffect::None };
        return true;
    }
    if (name == "RUIVA" || name == "LOIRA" || name == "OTOME" ||
        name == "GOTICA" || name == "MORENA" || name == "YANDER") {
        out = NameReaction{ "[WAIFU CONFIRMED]", "A true cultured gamer of fine 2D taste.", true, NameEffect::Waifu };
        return true;
    }
    if (name == "FURIEL" || name == "FURIELL") {
        out = NameReaction{ "", "Furiell is flattered. But no.", false, NameEffect::None };
        return true;
    }
    if (name == "BARKY") {
        out = NameReaction{ "", "AWOO! ...wait. That's MY name.", false, NameEffect::None };
        return true;
    }
    if (name == "FLOOFY") {
        out = NameReaction{ "", "No shame. Respect. ...Not allowed, though.", false, NameEffect::None };
        return true;
    }
    if (name == "AZZY") {
        out = NameReaction{ "", "...", true, NameEffect::Darken };
        return true;
    }
    if (name == "YIFF") {
        out = NameReaction{ "", "The narrator refuses.", false, NameEffect::None };
        return true;
    }
    if (name == "OWO") {
        out = NameReaction{ "", "What's this?", true, NameEffect::None };
        return true;
    }
    if (name == "GAMER") {
        out = NameReaction{ "[REDUNDANT]", "We know.", true, NameEffect::None };
        return true;
    }
    if (name == "PRO") {
        out = NameReaction{ "[PRO]", "Your K/D ratio is showing.", true, NameEffect::None };
        return true;
    }
    if (name == "GASTER") {
        out = NameReaction{ "", "", true, NameEffect::RestartGame };
        return true;
    }
    return false;
}

inline const std::vector<std::string>& builtinTableNames() {
    static const std::vector<std::string> names = {
        "CHAD", "GRUB", "GOBLIN", "FROG", "PEPE", "SKYRIM", "SANS", "FRISK",
        "CHARA", "FLOWEY", "TORIEL", "ASGORE", "RUIVA", "LOIRA",
        "OTOME", "GOTICA", "MORENA", "YANDER", "FURIEL", "FURIELL",
        "BARKY", "FLOOFY", "AZZY", "YIFF", "OWO", "GAMER",
        "PRO", "GASTER"
    };
    return names;
}

inline NameReaction reactionFor(const std::string& rawName, const NameTable& table) {
    const std::string name = normalizeName(rawName);

    if (table.isLoaded()) {
        const NameReaction* found = table.find(name);
        if (found != nullptr) {
            return *found;
        }
    } else {
        NameReaction builtinReaction{};
        if (lookupBuiltinReaction(name, builtinReaction)) {
            return builtinReaction;
        }
    }

    // Generic rules after the table
    bool allSame = !name.empty();
    for (std::size_t i = 1; i < name.size(); ++i) {
        if (name[i] != name[0]) {
            allSame = false;
            break;
        }
    }
    if (allSame && name.size() > 1) {
        return NameReaction{ "", "Not very creative... Did your keyboard jam?", true, NameEffect::None };
    }
    if (name.size() == 1) {
        return NameReaction{ "", "You call that a gamer? Saving characters?", true, NameEffect::None };
    }

    return NameReaction{};
}

inline NameReaction reactionFor(const std::string& rawName) {
    return reactionFor(rawName, globalNameTable());
}

// MET-004: confirming the same name as the previous game. Only replaces the default reaction.
inline NameReaction withSameNameReaction(NameReaction reaction, const std::string& rawName, const std::string& lastName) {
    const bool isDefault = reaction.tag.empty() && reaction.message.empty() && reaction.allowed && reaction.effect == NameEffect::None;
    if (isDefault && !lastName.empty() && normalizeName(rawName) == normalizeName(lastName)) {
        reaction.message = "Again? You must really love this name.";
    }
    return reaction;
}

} // namespace nundertale
