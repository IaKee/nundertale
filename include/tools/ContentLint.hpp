#pragma once

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace nundertale {

struct LintFinding {
    int line;
    int col;
    std::string rule;
    std::string snippet;
};

struct Utf8CodePoint {
    char32_t value;
    std::size_t length;
};

inline Utf8CodePoint decodeUtf8(const std::string& text, std::size_t offset) {
    const auto first = static_cast<unsigned char>(text[offset]);
    if (first < 0x80U) {
        return {static_cast<char32_t>(first), 1U};
    }

    std::size_t length = 0U;
    char32_t value = 0;
    if ((first & 0xE0U) == 0xC0U) {
        length = 2U;
        value = static_cast<char32_t>(first & 0x1FU);
    } else if ((first & 0xF0U) == 0xE0U) {
        length = 3U;
        value = static_cast<char32_t>(first & 0x0FU);
    } else if ((first & 0xF8U) == 0xF0U) {
        length = 4U;
        value = static_cast<char32_t>(first & 0x07U);
    } else {
        return {static_cast<char32_t>(first), 1U};
    }

    if (offset + length > text.size()) {
        return {static_cast<char32_t>(first), 1U};
    }
    for (std::size_t index = 1U; index < length; ++index) {
        const auto continuation = static_cast<unsigned char>(text[offset + index]);
        if ((continuation & 0xC0U) != 0x80U) {
            return {static_cast<char32_t>(first), 1U};
        }
        value = (value << 6U) | static_cast<char32_t>(continuation & 0x3FU);
    }
    return {value, length};
}

inline bool isNonEnglishLetter(char32_t codePoint) {
    return codePoint >= U'\u00C0' && codePoint <= U'\u024F';
}

namespace content_lint_detail {

struct PositionedCodePoint {
    char32_t value;
    int line;
    int col;
};

inline char32_t foldCase(char32_t codePoint) {
    if (codePoint >= U'A' && codePoint <= U'Z') {
        return codePoint + (U'a' - U'A');
    }
    if ((codePoint >= U'\u00C0' && codePoint <= U'\u00D6') ||
        (codePoint >= U'\u00D8' && codePoint <= U'\u00DE')) {
        return codePoint + 0x20;
    }
    if (codePoint == U'\u0178') {
        return U'\u00FF';
    }
    return codePoint;
}

inline bool isWordCharacter(char32_t codePoint) {
    if (codePoint == U'_') {
        return true;
    }
    if (codePoint < 128) {
        return std::isalnum(static_cast<unsigned char>(codePoint)) != 0;
    }
    return isNonEnglishLetter(codePoint);
}

inline std::vector<char32_t> foldedCodePoints(std::string_view value) {
    const std::string owned(value);
    std::vector<char32_t> result;
    for (std::size_t offset = 0U; offset < owned.size();) {
        const Utf8CodePoint decoded = decodeUtf8(owned, offset);
        result.push_back(foldCase(decoded.value));
        offset += decoded.length;
    }
    return result;
}

inline std::string lineSnippet(const std::string& text, int requestedLine) {
    int line = 1;
    std::size_t start = 0U;
    for (std::size_t index = 0U; index <= text.size(); ++index) {
        if (index == text.size() || text[index] == '\n') {
            if (line == requestedLine) {
                std::size_t end = index;
                if (end > start && text[end - 1U] == '\r') {
                    --end;
                }
                constexpr std::size_t maxLength = 160U;
                const std::size_t length = std::min(end - start, maxLength);
                std::string snippet = text.substr(start, length);
                if (end - start > maxLength) {
                    snippet += "...";
                }
                return snippet;
            }
            ++line;
            start = index + 1U;
        }
    }
    return {};
}

inline bool matchesAt(const std::vector<PositionedCodePoint>& text,
                      std::size_t start,
                      const std::vector<char32_t>& needle) {
    if (needle.empty() || start + needle.size() > text.size()) {
        return false;
    }
    for (std::size_t index = 0U; index < needle.size(); ++index) {
        if (foldCase(text[start + index].value) != needle[index]) {
            return false;
        }
    }
    return true;
}

inline std::string trim(std::string value) {
    const auto notSpace = [](unsigned char character) {
        return std::isspace(character) == 0;
    };
    const auto first = std::find_if(value.begin(), value.end(), notSpace);
    if (first == value.end()) {
        return {};
    }
    const auto last = std::find_if(value.rbegin(), value.rend(), notSpace).base();
    return std::string(first, last);
}

} // namespace content_lint_detail

inline std::vector<LintFinding> lintText(const std::string& text,
                                         const std::vector<std::string>& blocked) {
    using namespace content_lint_detail;

    std::vector<PositionedCodePoint> characters;
    int line = 1;
    int col = 1;
    for (std::size_t offset = 0U; offset < text.size();) {
        const Utf8CodePoint decoded = decodeUtf8(text, offset);
        characters.push_back({decoded.value, line, col});
        offset += decoded.length;
        if (decoded.value == U'\n') {
            ++line;
            col = 1;
        } else {
            ++col;
        }
    }

    std::vector<LintFinding> findings;
    const auto addFinding = [&](const PositionedCodePoint& position, const std::string& rule) {
        findings.push_back({position.line, position.col, rule, lineSnippet(text, position.line)});
    };

    for (const PositionedCodePoint& character : characters) {
        if (isNonEnglishLetter(character.value)) {
            addFinding(character, "non-english-char");
        }
    }

    static const std::vector<std::string> portugueseWords = {
        "não", "nao", "você", "voce", "obrigado", "obrigada", "olá", "então",
        "também", "tambem", "porque", "muito", "coisa", "isso", "aqui", "agora",
        "jogo", "jogador", "vida", "ataque", "amigo", "gato", "cachorro", "sim",
        "tchau", "oi"
    };

    for (const std::string& word : portugueseWords) {
        const std::vector<char32_t> needle = foldedCodePoints(word);
        for (std::size_t index = 0U; index < characters.size(); ++index) {
            if (!matchesAt(characters, index, needle)) {
                continue;
            }
            const bool leftBoundary = index == 0U || !isWordCharacter(characters[index - 1U].value);
            const std::size_t after = index + needle.size();
            const bool rightBoundary = after == characters.size() ||
                                       !isWordCharacter(characters[after].value);
            if (leftBoundary && rightBoundary) {
                addFinding(characters[index], "portuguese-word");
            }
        }
    }

    for (const std::string& rawTerm : blocked) {
        const std::string term = trim(rawTerm);
        if (term.empty() || term.front() == '#') {
            continue;
        }
        const std::vector<char32_t> needle = foldedCodePoints(term);
        for (std::size_t index = 0U; index < characters.size(); ++index) {
            if (matchesAt(characters, index, needle)) {
                addFinding(characters[index], "blocked-term");
            }
        }
    }

    std::stable_sort(findings.begin(), findings.end(), [](const LintFinding& left,
                                                          const LintFinding& right) {
        if (left.line != right.line) {
            return left.line < right.line;
        }
        if (left.col != right.col) {
            return left.col < right.col;
        }
        return left.rule < right.rule;
    });
    return findings;
}

namespace content_lint_detail {

inline std::vector<PositionedCodePoint> decodePositionedCharacters(const std::string& text) {
    std::vector<PositionedCodePoint> characters;
    int line = 1;
    int col = 1;
    for (std::size_t offset = 0U; offset < text.size();) {
        const Utf8CodePoint decoded = decodeUtf8(text, offset);
        characters.push_back({decoded.value, line, col});
        offset += decoded.length;
        if (decoded.value == U'\n') {
            ++line;
            col = 1;
        } else {
            ++col;
        }
    }
    return characters;
}

class StrictJsonValidator {
public:
    explicit StrictJsonValidator(const std::string& text)
        : m_text(text),
          m_chars(decodePositionedCharacters(text)) {
        if (!m_chars.empty()) {
            m_eofPos = m_chars.back();
            if (m_eofPos.value == U'\n') {
                m_eofPos.line += 1;
                m_eofPos.col = 1;
            } else {
                m_eofPos.col += 1;
            }
        } else {
            m_eofPos = {0, 1, 1};
        }
    }

    bool validate(LintFinding* errorOut) {
        skipWhitespace();
        if (atEnd()) {
            return fail(m_eofPos, "empty JSON input", errorOut);
        }

        if (!parseValue(errorOut)) {
            return false;
        }

        skipWhitespace();
        if (!atEnd()) {
            return fail(currentPos(), "unexpected trailing characters after root JSON value", errorOut);
        }

        return true;
    }

private:
    bool fail(const PositionedCodePoint& pos, const std::string& message, LintFinding* errorOut) {
        (void)message;
        if (errorOut != nullptr) {
            errorOut->line = pos.line;
            errorOut->col = pos.col;
            errorOut->rule = "invalid-json";
            errorOut->snippet = lineSnippet(m_text, pos.line);
        }
        return false;
    }

    bool atEnd() const {
        return m_pos >= m_chars.size();
    }

    PositionedCodePoint currentPos() const {
        return atEnd() ? m_eofPos : m_chars[m_pos];
    }

    char32_t peek() const {
        return atEnd() ? 0 : m_chars[m_pos].value;
    }

    char32_t advance() {
        return atEnd() ? 0 : m_chars[m_pos++].value;
    }

    bool consume(char32_t expected) {
        if (peek() == expected) {
            advance();
            return true;
        }
        return false;
    }

    void skipWhitespace() {
        while (!atEnd()) {
            const char32_t ch = peek();
            if (ch == U' ' || ch == U'\t' || ch == U'\n' || ch == U'\r') {
                advance();
            } else {
                break;
            }
        }
    }

    bool parseValue(LintFinding* errorOut) {
        skipWhitespace();
        if (atEnd()) {
            return fail(m_eofPos, "expected JSON value", errorOut);
        }

        const char32_t ch = peek();
        if (ch == U'{') {
            return parseObject(errorOut);
        }
        if (ch == U'[') {
            return parseArray(errorOut);
        }
        if (ch == U'"') {
            return parseString(errorOut);
        }
        if (ch == U'-' || (ch >= U'0' && ch <= U'9')) {
            return parseNumber(errorOut);
        }
        if (ch == U't') {
            return parseLiteral(U"true", errorOut);
        }
        if (ch == U'f') {
            return parseLiteral(U"false", errorOut);
        }
        if (ch == U'n') {
            return parseLiteral(U"null", errorOut);
        }

        return fail(currentPos(), "unexpected character at start of value", errorOut);
    }

    bool parseObject(LintFinding* errorOut) {
        advance(); // consume '{'
        skipWhitespace();
        if (consume(U'}')) {
            return true;
        }

        while (true) {
            skipWhitespace();
            if (peek() != U'"') {
                return fail(currentPos(), "expected string key in object", errorOut);
            }
            if (!parseString(errorOut)) {
                return false;
            }

            skipWhitespace();
            if (!consume(U':')) {
                return fail(currentPos(), "expected ':' after key in object", errorOut);
            }

            if (!parseValue(errorOut)) {
                return false;
            }

            skipWhitespace();
            if (consume(U'}')) {
                return true;
            }

            if (peek() == U',') {
                const PositionedCodePoint commaPos = currentPos();
                advance(); // consume ','
                skipWhitespace();
                if (peek() == U'}') {
                    return fail(commaPos, "trailing comma in object", errorOut);
                }
            } else {
                return fail(currentPos(), "expected ',' or '}' in object", errorOut);
            }
        }
    }

    bool parseArray(LintFinding* errorOut) {
        advance(); // consume '['
        skipWhitespace();
        if (consume(U']')) {
            return true;
        }

        while (true) {
            if (!parseValue(errorOut)) {
                return false;
            }

            skipWhitespace();
            if (consume(U']')) {
                return true;
            }

            if (peek() == U',') {
                const PositionedCodePoint commaPos = currentPos();
                advance(); // consume ','
                skipWhitespace();
                if (peek() == U']') {
                    return fail(commaPos, "trailing comma in array", errorOut);
                }
            } else {
                return fail(currentPos(), "expected ',' or ']' in array", errorOut);
            }
        }
    }

    bool parseString(LintFinding* errorOut) {
        const PositionedCodePoint openQuote = currentPos();
        advance(); // consume '"'

        while (!atEnd()) {
            const PositionedCodePoint charPos = currentPos();
            const char32_t ch = advance();

            if (ch == U'"') {
                return true;
            }

            if (ch == U'\\') {
                if (atEnd()) {
                    return fail(charPos, "unclosed string ending in escape", errorOut);
                }
                const PositionedCodePoint escapePos = charPos;
                const char32_t escaped = advance();
                switch (escaped) {
                    case U'"':
                    case U'\\':
                    case U'/':
                    case U'b':
                    case U'f':
                    case U'n':
                    case U'r':
                    case U't':
                        break;
                    case U'u': {
                        for (int i = 0; i < 4; ++i) {
                            if (atEnd()) {
                                return fail(escapePos, "incomplete unicode escape in string", errorOut);
                            }
                            const char32_t hex = advance();
                            const bool isHex = (hex >= U'0' && hex <= U'9') ||
                                              (hex >= U'a' && hex <= U'f') ||
                                              (hex >= U'A' && hex <= U'F');
                            if (!isHex) {
                                return fail(escapePos, "invalid hex digit in unicode escape", errorOut);
                            }
                        }
                        break;
                    }
                    default:
                        return fail(escapePos, "invalid escape sequence in string", errorOut);
                }
                continue;
            }

            if (ch < 0x20U) {
                if (ch == U'\n' || ch == U'\r') {
                    return fail(openQuote, "unclosed string", errorOut);
                }
                return fail(charPos, "unescaped control character in string", errorOut);
            }
        }

        return fail(openQuote, "unclosed string", errorOut);
    }

    bool parseNumber(LintFinding* errorOut) {
        const PositionedCodePoint startPos = currentPos();

        if (peek() == U'-') {
            advance();
            if (atEnd() || peek() < U'0' || peek() > U'9') {
                return fail(startPos, "invalid number: '-' not followed by digit", errorOut);
            }
        }

        if (peek() == U'0') {
            advance();
            if (!atEnd() && peek() >= U'0' && peek() <= U'9') {
                return fail(startPos, "invalid number: leading zero not allowed", errorOut);
            }
        } else if (peek() >= U'1' && peek() <= U'9') {
            while (!atEnd() && peek() >= U'0' && peek() <= U'9') {
                advance();
            }
        } else {
            return fail(startPos, "invalid number: expected digit", errorOut);
        }

        // Fraction
        if (peek() == U'.') {
            advance();
            if (atEnd() || peek() < U'0' || peek() > U'9') {
                return fail(startPos, "invalid number: '.' must be followed by digit", errorOut);
            }
            while (!atEnd() && peek() >= U'0' && peek() <= U'9') {
                advance();
            }
        }

        // Exponent
        if (peek() == U'e' || peek() == U'E') {
            advance();
            if (peek() == U'+' || peek() == U'-') {
                advance();
            }
            if (atEnd() || peek() < U'0' || peek() > U'9') {
                return fail(startPos, "invalid number: exponent must be followed by digit", errorOut);
            }
            while (!atEnd() && peek() >= U'0' && peek() <= U'9') {
                advance();
            }
        }

        if (!atEnd()) {
            const char32_t ch = peek();
            const bool validDelimiter = isJsonWhitespace(ch) || ch == U',' || ch == U']' || ch == U'}';
            if (!validDelimiter) {
                return fail(currentPos(), "unexpected character after number", errorOut);
            }
        }

        return true;
    }

    bool parseLiteral(std::u32string_view literal, LintFinding* errorOut) {
        const PositionedCodePoint startPos = currentPos();
        for (char32_t expected : literal) {
            if (atEnd() || peek() != expected) {
                return fail(startPos, "unexpected token", errorOut);
            }
            advance();
        }

        if (!atEnd()) {
            const char32_t ch = peek();
            const bool isWord = (ch >= U'a' && ch <= U'z') ||
                                (ch >= U'A' && ch <= U'Z') ||
                                (ch >= U'0' && ch <= U'9') ||
                                ch == U'_';
            if (isWord) {
                return fail(startPos, "unexpected identifier suffix", errorOut);
            }
        }

        return true;
    }

    static bool isJsonWhitespace(char32_t ch) {
        return ch == U' ' || ch == U'\t' || ch == U'\n' || ch == U'\r';
    }

    const std::string& m_text;
    std::vector<PositionedCodePoint> m_chars;
    PositionedCodePoint m_eofPos{0, 1, 1};
    std::size_t m_pos = 0;
};

} // namespace content_lint_detail

inline bool validateJson(const std::string& text, LintFinding* errorOut = nullptr) {
    content_lint_detail::StrictJsonValidator validator(text);
    if (!validator.validate(errorOut)) return false;

    // Cutscene files are the only content documents with a top-level "steps" array.
    // Keep this deliberately small and header-only so both the lint executable and tests
    // enforce the same allow-list while the strict validator handles JSON structure.
    if (text.find("\"steps\"") == std::string::npos) return true;
    std::size_t position = 0U;
    while ((position = text.find("\"type\"", position)) != std::string::npos) {
        const std::size_t colon = text.find(':', position + 6U);
        const std::size_t quote = colon == std::string::npos
            ? std::string::npos : text.find('"', colon + 1U);
        const std::size_t end = quote == std::string::npos
            ? std::string::npos : text.find('"', quote + 1U);
        if (end == std::string::npos) break;
        const std::string type = text.substr(quote + 1U, end - quote - 1U);
        const bool known = type == "wait" || type == "dialog" || type == "sound" ||
            type == "setFlag" || type == "shake" || type == "flash" ||
            type == "startBattle" || type == "setFur" || type == "healFull" ||
            type == "facePlayer" || type == "playerAnim" || type == "giveItem" ||
            type == "addFlag" || type == "addFur" || type == "npcFace" ||
            type == "npcAnim" || type == "hideNpc" || type == "showNpc" ||
            type == "moveNpc" || type == "movePlayer" || type == "lockControls";
        if (!known) {
            if (errorOut != nullptr) {
                *errorOut = {1, 1, "unknown-cutscene-step", type};
            }
            return false;
        }
        position = end + 1U;
    }
    return true;
}

inline bool isKnownRoomObjectType(const std::string& type) {
    return type == "spawn" || type == "door" || type == "sign" || type == "examine" ||
        type == "prop" || type == "npc" || type == "itemPickup" || type == "savepoint" ||
        type == "trigger" || type == "interactable" || type == "hazard";
}

inline bool isKnownRoomObjectProperty(const std::string& property) {
    return property == "showIf" || property == "hideIf" || property == "cutscene" ||
        property == "bumpCutscene" ||
        property == "altAnim" || property == "altAnimIf" || property == "damage" ||
        property == "lines" || property == "sound";
}

inline bool validateJson(const std::string& text, LintFinding& errorOut) {
    return validateJson(text, &errorOut);
}

inline std::vector<LintFinding> lintJson(const std::string& text) {
    LintFinding error;
    if (!validateJson(text, &error)) {
        return {error};
    }
    return {};
}

} // namespace nundertale

