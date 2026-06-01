#pragma once

#include "core/IScene.hpp"
#include "core/NameRules.hpp"
#include <raylib.h>
#include <string>

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
    void drawGridLetter(GameContext& context, Font font, const std::string& letter, Vector2 pos, Color color, bool selected);
    NameSelectState m_state = NameSelectState::Selecting;
    std::string m_currentName = "";
    int m_cursorX = 0;
    int m_cursorY = 0;
    bool m_confirmChoice = false; // false = NO, true = YES

    NameReaction m_reaction;
    int m_tooManyAttempts = 0;
    float m_animTimer = 0.0f;
    float m_confirmTimer = 0.0f;
};

} // namespace nundertale
