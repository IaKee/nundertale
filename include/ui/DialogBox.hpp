#pragma once

#include <raylib.h>
#include <string>
#include <vector>
#include <functional>
#include <sstream>
#include "core/FurReactions.hpp"

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
    DialogBox() = default;

    void startDialog(const std::vector<std::string>& pages, const std::string& speaker = "", std::function<void()> onComplete = nullptr, const std::string& voiceSfx = "", int fur = -1);
    void update(float dt, GameContext& context);
    void render(GameContext& context);

    bool isActive() const { return m_isActive; }
    void close();

    void setOnChoice(std::function<void(int index, const std::string& option)> cb);
    int lastChoice() const { return m_lastChoice; }

    void setFur(int fur);
    int getFur() const { return m_fur; }
    const std::vector<std::string>& getPages() const { return m_pages; }
    void applyFurToPages();

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

    int m_fur = 0;

    Rectangle m_boxRect{ 32, 315, 576, 150 };
};

inline void DialogBox::startDialog(const std::vector<std::string>& pages, const std::string& speaker, std::function<void()> onComplete, const std::string& voiceSfx, int fur) {
    m_pages.clear();
    m_pageOptions.clear();
    for (const auto& rawPage : pages) {
        auto parsed = parseChoiceToken(rawPage);
        m_pages.push_back(parsed.text);
        m_pageOptions.push_back(parsed.options);
    }
    if (fur >= 0) {
        m_fur = fur;
    }
    applyFurToPages();
    m_speaker = speaker;
    m_onComplete = onComplete;
    m_onChoice = nullptr;
    m_lastChoice = -1;
    m_selectedChoice = 0;
    m_currentPage = 0;
    m_visibleChars = 0;
    m_charTimer = 0.0f;
    m_isActive = true;
    m_isPageComplete = false;

    // TXT-006: Voice per character
    if (!voiceSfx.empty()) {
        m_voiceSfx = voiceSfx;
    } else if (speaker.find("Sans") != std::string::npos) {
        m_voiceSfx = "assets/sfx/sans_type.wav";
    } else if (speaker.find("Furiell") != std::string::npos || speaker.find("FURIELL") != std::string::npos) {
        m_voiceSfx = "assets/sfx/voice_furiell.ogg";
    } else if (speaker.find("Floofy") != std::string::npos || speaker.find("FLOOFY") != std::string::npos) {
        m_voiceSfx = "assets/sfx/voice_floofy.ogg";
    } else if (speaker.find("Discord") != std::string::npos || speaker.find("Raid") != std::string::npos) {
        m_voiceSfx = "assets/sfx/voice_default.ogg";
    } else {
        m_voiceSfx = "assets/sfx/voice_default.ogg";
    }
}

inline void DialogBox::setFur(int fur) {
    m_fur = fur;
    applyFurToPages();
}

inline void DialogBox::applyFurToPages() {
    if (m_fur < 80) return;
    for (auto& page : m_pages) {
        page = applyThoughtSqueakToPage(page, m_fur);
    }
}

inline void DialogBox::setOnChoice(std::function<void(int index, const std::string& option)> cb) {
    m_onChoice = cb;
}

inline void DialogBox::close() {
    m_isActive = false;
    if (m_onComplete) {
        auto cb = m_onComplete;
        m_onComplete = nullptr;
        cb();
    }
}

} // namespace nundertale
