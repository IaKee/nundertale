#pragma once

#include <cctype>
#include <string>
#include <unordered_map>
#include <utility>

namespace nundertale {

struct FurTable {
    FurTable()
        : values{
              {"bullet_neutral", 3},
              {"bullet_cute", 7},
              {"bullet_very_cute", 10},
              {"floofy_hug_pellet", 20},
              {"accept_hug", 15},
              {"accept_hug_furiell", 30},
              {"furiell_bed", 5},
              {"fursuit_head", 40},
              {"quiz_wrong", 5},
          } {}

    bool parse(const std::string& jsonText) {
        std::unordered_map<std::string, int> parsed = FurTable{}.values;
        std::size_t pos = 0;
        skipWhitespace(jsonText, pos);
        if (!consume(jsonText, pos, '{')) return false;
        skipWhitespace(jsonText, pos);
        if (consume(jsonText, pos, '}')) {
            values = std::move(parsed);
            return true;
        }

        while (pos < jsonText.size()) {
            std::string key;
            int value = 0;
            if (!parseString(jsonText, pos, key)) return false;
            skipWhitespace(jsonText, pos);
            if (!consume(jsonText, pos, ':')) return false;
            skipWhitespace(jsonText, pos);
            if (!parseInt(jsonText, pos, value)) return false;
            parsed[key] = value;
            skipWhitespace(jsonText, pos);
            if (consume(jsonText, pos, '}')) {
                skipWhitespace(jsonText, pos);
                if (pos != jsonText.size()) return false;
                values = std::move(parsed);
                return true;
            }
            if (!consume(jsonText, pos, ',')) return false;
            skipWhitespace(jsonText, pos);
        }
        return false;
    }

    int get(const std::string& key, int fallback) const {
        const auto it = values.find(key);
        return it != values.end() ? it->second : fallback;
    }

    std::unordered_map<std::string, int> values;

private:
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

    static bool parseInt(const std::string& text, std::size_t& pos, int& out) {
        bool negative = false;
        if (pos < text.size() && text[pos] == '-') {
            negative = true;
            ++pos;
        }
        if (pos >= text.size() || !std::isdigit(static_cast<unsigned char>(text[pos]))) return false;
        int value = 0;
        while (pos < text.size() && std::isdigit(static_cast<unsigned char>(text[pos])) != 0) {
            value = value * 10 + (text[pos] - '0');
            ++pos;
        }
        out = negative ? -value : value;
        return true;
    }
};

inline FurTable& globalFurTable() {
    static FurTable table;
    return table;
}

} // namespace nundertale
