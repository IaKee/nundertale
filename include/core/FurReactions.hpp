#pragma once

#include <string>
#include <sstream>

namespace nundertale {

constexpr const char* THOUGHT_SQUEAK_SUFFIX = " (squeak)";

inline std::string thoughtSqueakSuffix() {
    return THOUGHT_SQUEAK_SUFFIX;
}

inline bool isThoughtLine(const std::string& line) {
    size_t start = line.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
        return false;
    }
    if (line.compare(start, 3, "* (") != 0) {
        return false;
    }
    size_t endParen = line.find(')', start + 3);
    return endParen != std::string::npos;
}

inline bool hasThoughtSqueak(const std::string& line) {
    if (line.find("(squeak)") != std::string::npos) {
        return true;
    }
    size_t last = line.find_last_not_of(" \t\r\n.)");
    if (last != std::string::npos && last >= 5) {
        if (line.substr(last - 5, 6) == "squeak") {
            return true;
        }
    }
    return false;
}

inline std::string furReactionLine(int fur, const std::string& who) {
    if (who == "barky" && fur >= 40) {
        return "Dude. Your paws are SO much better than mine.";
    }
    if (who == "generic" && fur >= 20) {
        return "* Nice ears, newcomer!";
    }
    if (fur >= 80) {
        if (who == "thought" || who == "pepe_thought" || who == "grub_thought") {
            return THOUGHT_SQUEAK_SUFFIX;
        }
        if (isThoughtLine(who) && !hasThoughtSqueak(who)) {
            return THOUGHT_SQUEAK_SUFFIX;
        }
    }
    return "";
}

inline std::string applyThoughtSqueak(const std::string& line, int fur) {
    if (fur >= 80 && isThoughtLine(line) && !hasThoughtSqueak(line)) {
        if (!line.empty() && line.back() == '\r') {
            return line.substr(0, line.size() - 1) + furReactionLine(fur, line) + "\r";
        }
        return line + furReactionLine(fur, line);
    }
    return line;
}

inline std::string applyThoughtSqueakToPage(const std::string& page, int fur) {
    if (fur < 80) {
        return page;
    }
    std::string result;
    std::stringstream ss(page);
    std::string line;
    bool first = true;
    while (std::getline(ss, line, '\n')) {
        if (!first) {
            result += '\n';
        }
        first = false;
        result += applyThoughtSqueak(line, fur);
    }
    return result;
}

} // namespace nundertale
