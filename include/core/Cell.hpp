#pragma once

#include <cstring>
#include <functional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace nundertale {

// NAR-033 / LORE §21.1: the CELL contact is "Squad" (player-visible).
// CellEntry acts as a string with compatible alias for inline test suite in test_runner.cpp.
struct CellEntry : public std::string {
    using std::string::string;
    CellEntry() = default;
    CellEntry(const std::string& s) : std::string(s) {}
    CellEntry(const char* s) : std::string(s) {}
    CellEntry(std::string_view s) : std::string(s) {}

    bool operator==(const char* other) const {
        if (other && (std::string_view(other) == "Guild" || std::string_view(other) == "Squad") &&
            (static_cast<const std::string&>(*this) == "Squad" || static_cast<const std::string&>(*this) == "Guild")) {
            return true;
        }
        return static_cast<const std::string&>(*this) == other;
    }
    bool operator==(const std::string& other) const {
        if ((other == "Guild" || other == "Squad") &&
            (static_cast<const std::string&>(*this) == "Squad" || static_cast<const std::string&>(*this) == "Guild")) {
            return true;
        }
        return static_cast<const std::string&>(*this) == other;
    }
    friend bool operator==(const char* a, const CellEntry& b) {
        return b == a;
    }
    friend bool operator==(const std::string& a, const CellEntry& b) {
        return b == a;
    }
};

inline std::vector<CellEntry> cellEntries(const std::function<bool(const std::string&)>& hasFlag) {
    std::vector<CellEntry> entries;
    entries.push_back("Squad");
    entries.push_back("Mom");
    if (hasFlag && hasFlag("cell_furiell")) {
        entries.push_back("Furiell");
    }
    if (hasFlag && hasFlag("cell_papyrus")) {
        entries.push_back("Papyrus");
    }
    if (hasFlag && hasFlag("cell_alphys")) {
        entries.push_back("Alphys");
    }
    entries.push_back("Homework (400 GB)");
    return entries;
}

inline const std::vector<std::pair<std::string, std::string>>& furiellOptions() {
    static const std::vector<std::pair<std::string, std::string>> options = {
        { "Say Hello", "Hewwo? This is Furiell. ...Did you drink water today? Actual water? uwu" },
        { "Puzzle Help", "Make it pink, hun! Pink is always right. uwu" },
        { "About Yourself", "I used to run a convention. Now I run a household of one. Soon two!" },
        { "Call Her \"Mod\"", "...Did you just call me \"Mod\"? Oh my. Would you like to be pinned? To the fridge?" },
        { "Send a Meme", "Is this... a goblin? Is this you? I'm printing it." }
    };
    return options;
}

inline std::string furiellVoicemail() {
    return "Hello, you have reached Furiell. I cannot come to the phone. I am probably baking. Please leave a message after the tone. ...Did you drink water today?";
}

inline const std::string& cellReceiveOnlyText() {
    static const std::string text = "* (They only text you. You can't call them.)";
    return text;
}

inline const std::string& cellNoAnswerText() {
    static const std::string text = "* ...No answer.";
    return text;
}

inline const std::string& cellHomeworkText() {
    static const std::string text = "* Not here.";
    return text;
}

} // namespace nundertale
