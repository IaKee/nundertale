#pragma once

#include <cctype>
#include <cstdlib>
#include <string>
#include <utility>
#include <vector>

namespace nundertale {

struct ItemDef {
    std::string id;
    std::string name;
    std::string shortName;
    std::string type;
    int heal = 0;
    int at = 0;
    int df = 0;
    int furry = 0;
    float furMult = 1.0f;
    std::string info;
    std::string useText;
};

class ItemDatabase {
public:
    bool parse(const std::string& jsonText) {
        Parser parser(jsonText);
        std::vector<ItemDef> parsedItems;
        if (!parser.parseItems(parsedItems)) {
            return false;
        }
        m_items = std::move(parsedItems);
        return true;
    }

    const ItemDef* findItemByName(const std::string& name) const {
        for (const ItemDef& item : m_items) {
            if (item.name == name) {
                return &item;
            }
        }
        return nullptr;
    }

    const std::vector<ItemDef>& items() const {
        return m_items;
    }

private:
    class Parser {
    public:
        explicit Parser(const std::string& text) : m_text(text) {}

        bool parseItems(std::vector<ItemDef>& items) {
            skipWhitespace();
            if (!consume('[')) {
                return false;
            }
            skipWhitespace();
            if (consume(']')) {
                skipWhitespace();
                return atEnd();
            }

            while (true) {
                ItemDef item;
                if (!parseItem(item)) {
                    return false;
                }
                items.push_back(std::move(item));

                skipWhitespace();
                if (consume(']')) {
                    skipWhitespace();
                    return atEnd();
                }
                if (!consume(',')) {
                    return false;
                }
            }
        }

    private:
        bool parseItem(ItemDef& item) {
            skipWhitespace();
            if (!consume('{')) {
                return false;
            }
            skipWhitespace();
            if (consume('}')) {
                return true;
            }

            while (true) {
                std::string key;
                if (!parseString(key)) {
                    return false;
                }
                skipWhitespace();
                if (!consume(':')) {
                    return false;
                }
                skipWhitespace();

                if (!parseItemValue(key, item)) {
                    return false;
                }

                skipWhitespace();
                if (consume('}')) {
                    return true;
                }
                if (!consume(',')) {
                    return false;
                }
                skipWhitespace();
            }
        }

        bool parseItemValue(const std::string& key, ItemDef& item) {
            if (key == "id") return parseString(item.id);
            if (key == "name") return parseString(item.name);
            if (key == "shortName") return parseString(item.shortName);
            if (key == "type") return parseString(item.type);
            if (key == "info") return parseString(item.info);
            if (key == "useText") return parseString(item.useText);

            if (key == "heal") return parseInt(item.heal);
            if (key == "at") return parseInt(item.at);
            if (key == "df") return parseInt(item.df);
            if (key == "furry") return parseInt(item.furry);
            if (key == "furMult") return parseFloat(item.furMult);

            return skipValue();
        }

        bool parseString(std::string& value) {
            skipWhitespace();
            if (!consume('"')) {
                return false;
            }

            value.clear();
            while (!atEnd()) {
                const char ch = m_text[m_pos++];
                if (ch == '"') {
                    return true;
                }
                if (ch != '\\') {
                    value.push_back(ch);
                    continue;
                }
                if (atEnd()) {
                    return false;
                }

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

        bool parseInt(int& value) {
            double number = 0.0;
            if (!parseNumber(number)) {
                return false;
            }
            value = static_cast<int>(number);
            return true;
        }

        bool parseFloat(float& value) {
            double number = 0.0;
            if (!parseNumber(number)) {
                return false;
            }
            value = static_cast<float>(number);
            return true;
        }

        bool parseNumber(double& value) {
            skipWhitespace();
            if (atEnd()) {
                return false;
            }

            const char* begin = m_text.c_str() + m_pos;
            char* end = nullptr;
            value = std::strtod(begin, &end);
            if (end == begin) {
                return false;
            }
            m_pos += static_cast<std::size_t>(end - begin);
            return true;
        }

        bool skipValue() {
            skipWhitespace();
            if (atEnd()) {
                return false;
            }

            if (m_text[m_pos] == '"') {
                std::string ignored;
                return parseString(ignored);
            }
            if (m_text[m_pos] == '{') {
                return skipComposite('{', '}');
            }
            if (m_text[m_pos] == '[') {
                return skipComposite('[', ']');
            }

            const std::size_t start = m_pos;
            while (!atEnd() && m_text[m_pos] != ',' && m_text[m_pos] != '}' && m_text[m_pos] != ']') {
                ++m_pos;
            }
            return m_pos > start;
        }

        bool skipComposite(char open, char close) {
            if (!consume(open)) {
                return false;
            }
            int depth = 1;
            while (!atEnd() && depth > 0) {
                if (m_text[m_pos] == '"') {
                    std::string ignored;
                    if (!parseString(ignored)) {
                        return false;
                    }
                    continue;
                }
                const char ch = m_text[m_pos++];
                if (ch == open) {
                    ++depth;
                } else if (ch == close) {
                    --depth;
                }
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
        std::size_t m_pos = 0;
    };

    std::vector<ItemDef> m_items;
};

inline ItemDatabase parseItemsJson(const std::string& jsonText) {
    ItemDatabase database;
    database.parse(jsonText);
    return database;
}

} // namespace nundertale
