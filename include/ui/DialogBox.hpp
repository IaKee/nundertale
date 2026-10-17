#pragma once

#include <raylib.h>
#include <string>
#include <vector>
#include <functional>
#include <sstream>

namespace nundertale {

struct GameContext;

struct ParsedChoicePage {
    std::string text;
    std::vector<std::string> options;
};

inline ParsedChoicePage parseChoiceToken(const std::string& page) {
    const std::string prefix = "{choice:";
    size_t startPos = page.find(prefix);
    if (startPos == std::string::npos) {
        return { page, {} };
    }

    size_t endPos = page.find('}', startPos + prefix.size());
    if (endPos == std::string::npos) {
        return { page, {} };
    }

    std::string optionsStr = page.substr(startPos + prefix.size(), endPos - (startPos + prefix.size()));
    std::vector<std::string> options;
    std::stringstream ss(optionsStr);
    std::string opt;
    while (std::getline(ss, opt, '|')) {
        size_t first = opt.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) {
            continue;
        }
        size_t last = opt.find_last_not_of(" \t\r\n");
        options.push_back(opt.substr(first, last - first + 1));
    }

    if (options.size() < 2) {
        return { page, {} };
    }

    std::string text = page.substr(0, startPos) + page.substr(endPos + 1);
    return { text, options };
}

class DialogBox {
public:
    DialogBox();

    void startDialog(const std::vector<std::string>& pages, const std::string& speaker = "", std::function<void()> onComplete = nullptr, const std::string& voiceSfx = "");
    void update(float dt, GameContext& context);
    void render(GameContext& context);

    bool isActive() const { return m_isActive; }
    void close();

    void setOnChoice(std::function<void(int index, const std::string& option)> cb);
    int lastChoice() const { return m_lastChoice; }

private:
    std::vector<std::string> m_pages;
    std::vector<std::vector<std::string>> m_pageOptions;
    std::string m_speaker;
    std::string m_voiceSfx = "assets/sfx/type.ogg";
    std::function<void()> m_onComplete;
    std::function<void(int index, const std::string& option)> m_onChoice;

    size_t m_currentPage = 0;
    size_t m_visibleChars = 0;
    float m_charTimer = 0.0f;
    float m_charDelay = 0.033f; // 30 cps

    bool m_isActive = false;
    bool m_isPageComplete = false;

    int m_selectedChoice = 0;
    int m_lastChoice = -1;

    Rectangle m_boxRect{ 32, 315, 576, 150 };
};

} // namespace nundertale
