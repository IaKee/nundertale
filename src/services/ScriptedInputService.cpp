#include "services/ScriptedInputService.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>

namespace nundertale {

namespace {

std::string trim(std::string value) {
    const auto notSpace = [](unsigned char ch) { return std::isspace(ch) == 0; };
    const auto first = std::find_if(value.begin(), value.end(), notSpace);
    if (first == value.end()) return {};
    const auto last = std::find_if(value.rbegin(), value.rend(), notSpace).base();
    return std::string(first, last);
}

bool parseTick(const std::string& text, int& value) {
    try {
        std::size_t parsed = 0;
        const int result = std::stoi(text, &parsed);
        if (parsed != text.size() || result < 0) return false;
        value = result;
        return true;
    } catch (...) {
        return false;
    }
}

} // namespace

ScriptedInputService::ScriptedInputService(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        std::cerr << "[ScriptedInputService] Cannot open input script: " << path << '\n';
        return;
    }

    const std::unordered_map<std::string, Key> keys = {
        { "confirm", Key::Confirm }, { "cancel", Key::Cancel }, { "menu", Key::Menu },
        { "up", Key::Up }, { "down", Key::Down }, { "left", Key::Left },
        { "right", Key::Right }
    };

    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        ++lineNumber;
        const std::size_t comment = line.find('#');
        if (comment != std::string::npos) line.erase(comment);
        line = trim(std::move(line));
        if (line.empty()) continue;

        std::istringstream fields(line);
        std::string tickField;
        std::string keyField;
        std::string extra;
        if (!(fields >> tickField >> keyField) || (fields >> extra)) {
            std::cerr << "[ScriptedInputService] Invalid line " << lineNumber << ": " << line << '\n';
            continue;
        }

        const auto keyIt = keys.find(keyField);
        if (keyIt == keys.end()) {
            std::cerr << "[ScriptedInputService] Unknown key on line " << lineNumber << ": " << keyField << '\n';
            continue;
        }

        int firstTick = 0;
        int lastTick = 0;
        const std::size_t dash = tickField.find('-');
        if (dash == std::string::npos) {
            if (!parseTick(tickField, firstTick)) {
                std::cerr << "[ScriptedInputService] Invalid tick on line " << lineNumber << '\n';
                continue;
            }
            lastTick = firstTick;
        } else {
            if (!parseTick(tickField.substr(0, dash), firstTick) ||
                !parseTick(tickField.substr(dash + 1), lastTick) || lastTick < firstTick) {
                std::cerr << "[ScriptedInputService] Invalid tick range on line " << lineNumber << '\n';
                continue;
            }
            if (keyIt->second == Key::Confirm || keyIt->second == Key::Cancel ||
                keyIt->second == Key::Menu) {
                std::cerr << "[ScriptedInputService] Only directions can be held (line "
                          << lineNumber << ")\n";
                continue;
            }
        }

        m_events.push_back({ firstTick, lastTick, keyIt->second });
    }
}

void ScriptedInputService::update() {
}

void ScriptedInputService::endTick() {
    ++m_tick;
    m_consumed.fill(false);
}

bool ScriptedInputService::isHeld(Key key) const {
    return std::any_of(m_events.begin(), m_events.end(), [this, key](const Event& event) {
        return event.key == key && m_tick >= event.firstTick && m_tick <= event.lastTick;
    });
}

bool ScriptedInputService::consumePress(Key key) const {
    const std::size_t index = static_cast<std::size_t>(key);
    if (m_consumed[index]) return false;
    const bool pressed = std::any_of(m_events.begin(), m_events.end(), [this, key](const Event& event) {
        return event.key == key && event.firstTick == m_tick;
    });
    if (pressed) m_consumed[index] = true;
    return pressed;
}

bool ScriptedInputService::isUp() const { return isHeld(Key::Up); }
bool ScriptedInputService::isDown() const { return isHeld(Key::Down); }
bool ScriptedInputService::isLeft() const { return isHeld(Key::Left); }
bool ScriptedInputService::isRight() const { return isHeld(Key::Right); }

bool ScriptedInputService::isUpPressed() const { return consumePress(Key::Up); }
bool ScriptedInputService::isDownPressed() const { return consumePress(Key::Down); }
bool ScriptedInputService::isLeftPressed() const { return consumePress(Key::Left); }
bool ScriptedInputService::isRightPressed() const { return consumePress(Key::Right); }
bool ScriptedInputService::isConfirmPressed() const { return consumePress(Key::Confirm); }
bool ScriptedInputService::isCancelPressed() const { return consumePress(Key::Cancel); }
bool ScriptedInputService::isMenuPressed() const { return consumePress(Key::Menu); }
bool ScriptedInputService::isEscapePressed() const { return false; }
bool ScriptedInputService::isBackspacePressed() const { return false; }
bool ScriptedInputService::isQuitRequested() const { return false; }
int ScriptedInputService::getCharPressed() const { return 0; }

} // namespace nundertale
