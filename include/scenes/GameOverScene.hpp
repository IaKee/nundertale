#pragma once

#include "core/IScene.hpp"
#include <raylib.h>
#include <vector>
#include <string>

namespace nundertale {

struct SoulShard {
    Vector2 pos;
    Vector2 vel;
    float rotation;
    float rotSpeed;
};

class GameOverScene : public IScene {
public:
    explicit GameOverScene(Vector2 deathSoulPos = { 320.0f, 320.0f });

    void onEnter(GameContext& context) override;
    void update(float dt, GameContext& context) override;
    void render(GameContext& context) override;
    void onExit(GameContext& context) override;

private:
    enum class State {
        Frozen,       // Soul is stationary on black screen (0.5s)
        Cracking,     // Soul cracks with sound (1.0s)
        Shattering,   // Soul breaks into shards with gravity (1.5s)
        FadingText,   // "GAME OVER" fades in (1.5s)
        TypingText,   // Raid leader determination voice typing
        WaitingInput  // Waiting for Z to retry
    };

    Vector2 m_soulPos;
    State m_state = State::Frozen;
    float m_stateTimer = 0.0f;
    std::vector<SoulShard> m_shards;
    float m_titleAlpha = 0.0f;

    // Typewriter message (BND-010)
    std::vector<std::string> m_lines;
    size_t m_currentLineIdx = 0;
    float m_charTimer = 0.0f;
    size_t m_displayedChars = 0;
    float m_linePauseTimer = 0.0f;

    void drawSoul(float x, float y) const;
    void drawCrackedSoul(float x, float y) const;
};

} // namespace nundertale
