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

struct PomPom {
    Vector2 pos;
    Vector2 vel;
    float radius;
    Color color;
    float alpha;
};

class GameOverScene : public IScene {
public:
    explicit GameOverScene(Vector2 deathSoulPos = { 320.0f, 320.0f }, std::string enemyId = "");

    void onEnter(GameContext& context) override;
    void update(float dt, GameContext& context) override;
    void render(GameContext& context) override;
    void onExit(GameContext& context) override;

private:
    enum class State {
        Frozen,       // Soul is stationary on black screen (0.5s)
        Sprouting,    // Ears and tail sprout (0.8s)
        Inflating,    // Soul inflates and wobbles (0.7s)
        Pomf,         // Soul pops into pink pom-poms (1.0s)
        FadingText,   // "FURRY OVER" fades in (1.8s)
        TypingText,   // Asgrr voice typing
        WaitingInput  // Waiting for Z to retry
    };

    Vector2 m_soulPos;
    std::string m_enemyId;
    State m_state = State::Frozen;
    float m_stateTimer = 0.0f;
    std::vector<PomPom> m_pompoms;
    std::vector<SoulShard> m_shards;
    float m_titleAlpha = 0.0f;

    // Typewriter message (BND-010 & BND-011)
    std::vector<std::string> m_lines;
    size_t m_currentLineIdx = 0;
    float m_charTimer = 0.0f;
    size_t m_displayedChars = 0;
    float m_linePauseTimer = 0.0f;

    void drawSoul(float x, float y) const;
    void drawSproutingSoul(float x, float y, float progress) const;
    void drawScaledSoul(float x, float y, float scaleX, float scaleY) const;
    void spawnPomPoms();

    // Helper kept for future GAMER OVER (FLF-007)
    void drawCrackedSoul(float x, float y) const;
};

} // namespace nundertale
