#pragma once

#include <raylib.h>
#include <string>
#include <vector>
#include <functional>

namespace nundertale {

struct GameContext;

class DialogBox {
public:
    DialogBox();

    void startDialog(const std::vector<std::string>& pages, const std::string& speaker = "", std::function<void()> onComplete = nullptr, const std::string& voiceSfx = "");
    void update(float dt, GameContext& context);
    void render(GameContext& context);

    bool isActive() const { return m_isActive; }
    void close();

private:
    std::vector<std::string> m_pages;
    std::string m_speaker;
    std::string m_voiceSfx = "assets/sfx/type.ogg";
    std::function<void()> m_onComplete;

    size_t m_currentPage = 0;
    size_t m_visibleChars = 0;
    float m_charTimer = 0.0f;
    float m_charDelay = 0.033f; // 30 cps

    bool m_isActive = false;
    bool m_isPageComplete = false;

    Rectangle m_boxRect{ 32, 315, 576, 150 };
};

} // namespace nundertale
