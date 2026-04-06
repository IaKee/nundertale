#pragma once

#include <string>
#include <vector>
#include <cstddef>

namespace nundertale {

inline const std::vector<std::string>& asgrrLines() {
    static const std::vector<std::string> lines = {
        "Don't lose hope, [NAME]... The ears suit you, honestly.",
        "[NAME]! Stay determined... and maybe a little fluffy.",
        "This is not the end, [NAME]. It is a... rebrand.",
        "Get up, [NAME]. Your guild is counting on you. ...Probably.",
        "You cannot give up just yet... You have not even picked a species.",
        "Your SOUL said no, [NAME]. ...This time."
    };
    return lines;
}

inline std::string withName(std::string line, const std::string& name) {
    const std::string token = "[NAME]";
    size_t pos = 0;
    while ((pos = line.find(token, pos)) != std::string::npos) {
        line.replace(pos, token.length(), name);
        pos += name.length();
    }
    return line;
}

inline std::vector<std::string> furryOverLines(const std::string& name, const std::string& enemyId, int timesTransformed, bool toxicRoute, int pick) {
    if (toxicRoute) {
        return {};
    }

    std::vector<std::string> lines;
    if (timesTransformed >= 3) {
        lines.push_back(withName("...Again? Keep the tail, [NAME].", name));
    } else {
        const auto& asgrr = asgrrLines();
        size_t idx = static_cast<size_t>((pick % 6 + 6) % 6);
        lines.push_back(withName(asgrr[idx], name));
    }

    if (enemyId == "barky") {
        lines.push_back("...welcome to the pack? no? ok.");
    }

    return lines;
}

} // namespace nundertale
