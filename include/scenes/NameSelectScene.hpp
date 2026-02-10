#pragma once

#include "core/IScene.hpp"
#include <string>
#include <vector>

namespace nundertale {

enum class NameSelectState {
    Selecting,
    Confirming
};

class NameSelectScene : public IScene {
public:
    NameSelectScene() = default;
    ~NameSelectScene() override = default;

    void onEnter(GameContext& context) override;
    void update(float dt, GameContext& context) override;
    void render(GameContext& context) override;
    void onExit(GameContext& context) override;

private:
    void checkEasterEggs(GameContext& context);

    NameSelectState m_state = NameSelectState::Selecting;
    std::string m_currentName = "";
    int m_cursorX = 0;
    int m_cursorY = 0;
    bool m_confirmChoice = true; // true = YES, false = NO

    // Easter egg status flags
    bool m_isChad = false;
    bool m_isSkyrim = false;
    bool m_isWaifu = false;
    bool m_isUndertaleMeta = false;
    bool m_isIllegal = false;
    int m_tooManyAttempts = 0;

    std::string m_subtitle;
    std::string m_metaMessage;
    float m_animTimer = 0.0f;
};

} // namespace nundertale
