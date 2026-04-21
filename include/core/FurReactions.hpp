#pragma once

#include <string>

namespace nundertale {

inline std::string furReactionLine(int fur, const std::string& who) {
    if (who == "barky" && fur >= 40) {
        return "Dude. Your paws are SO much better than mine.";
    }
    if (who == "generic" && fur >= 20) {
        return "* Nice ears, newcomer!";
    }
    return "";
}

} // namespace nundertale
