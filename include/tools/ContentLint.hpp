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

} // namespace nundertale
