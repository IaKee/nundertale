#pragma once

#include <functional>
#include <string>
#include <utility>
#include <vector>

namespace nundertale {

inline std::vector<std::string> cellEntries(const std::function<bool(const std::string&)>& hasFlag) {
    std::vector<std::string> entries;
    entries.push_back("Guild");
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
        { "Say Hello", "Hello? This is Furiell. ...Did you drink water today? Actual water?" },
        { "Puzzle Help", "Look for the glitter, my child. The glitter is always right." },
        { "About Yourself", "I used to run a convention. Now I run a household of one. Soon two!" },
        { "Call Her \"Mod\"", "...Did you just call me \"Mod\"? Oh my. Would you like to be pinned? To the fridge?" },
        { "Send a Meme", "Is this... a frog? Is this you? I'm printing it." }
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
