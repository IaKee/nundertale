#include "scenes/GameOverScene.hpp"
#include "scenes/OverworldScene.hpp"
#include "core/GameContext.hpp"
#include "core/Constants.hpp"
#include "core/SceneManager.hpp"
#include <cmath>

namespace nundertale {

GameOverScene::GameOverScene(Vector2 deathSoulPos)
    : m_soulPos(deathSoulPos) {}

void GameOverScene::onEnter(GameContext& context) {
    context.audio.stopMusic();
    m_state = State::Frozen;
    m_stateTimer = 0.0f;
    m_shards.clear();
    m_titleAlpha = 0.0f;
    m_lines = {
        "You cannot ragequit just yet...",
        context.player.name + "!",
        "Stay determined..."
    };
    m_currentLineIdx = 0;
    m_displayedChars = 0;
    m_charTimer = 0.0f;
    m_linePauseTimer = 0.0f;
}

void GameOverScene::update(float dt, GameContext& context) {
    m_stateTimer += dt;

    switch (m_state) {
        case State::Frozen:
            if (m_stateTimer >= 0.5f) {
                m_state = State::Cracking;
                m_stateTimer = 0.0f;
                context.audio.playSound("assets/sfx/pipipipopopo_backwards.ogg", 0.9f);
                context.shake(4.0f, 0.25f);
            }
            break;

        case State::Cracking:
            if (m_stateTimer >= 1.0f) {
                m_state = State::Shattering;
                m_stateTimer = 0.0f;
                context.audio.playSound("assets/sfx/cartoon_slip.ogg", 0.9f);
                context.shake(7.0f, 0.35f);
                context.flash(WHITE, 0.15f);

                // Spawn 6 soul fragments flying outward with gravity (BND-010)
                m_shards = {
                    { m_soulPos, { -140.0f, -180.0f }, 0.0f, -5.0f },
                    { m_soulPos, {  -70.0f, -240.0f }, 0.0f, -8.0f },
                    { m_soulPos, {    0.0f, -270.0f }, 0.0f,  4.0f },
                    { m_soulPos, {   70.0f, -240.0f }, 0.0f,  8.0f },
                    { m_soulPos, {  140.0f, -180.0f }, 0.0f,  5.0f },
                    { m_soulPos, {  180.0f, -120.0f }, 0.0f, 10.0f }
                };
            }
            break;

        case State::Shattering:
            for (auto& s : m_shards) {
                s.vel.y += 350.0f * dt; // gravity
                s.pos.x += s.vel.x * dt;
                s.pos.y += s.vel.y * dt;
                s.rotation += s.rotSpeed * dt;
            }
            if (m_stateTimer >= 1.5f) {
                m_state = State::FadingText;
                m_stateTimer = 0.0f;
                context.audio.playMusic("assets/music/epic_background_loop.ogg", true);
            }
            break;

        case State::FadingText:
            m_titleAlpha = m_stateTimer / 1.5f;
            if (m_titleAlpha > 1.0f) m_titleAlpha = 1.0f;

            if (m_stateTimer >= 1.8f) {
                m_state = State::TypingText;
                m_stateTimer = 0.0f;
            }
            break;

        case State::TypingText:
            if (m_currentLineIdx < m_lines.size()) {
                const std::string& currentLine = m_lines[m_currentLineIdx];

                // Cancel key skips typewriter for current line
                if (context.input.isCancelPressed()) {
                    m_displayedChars = currentLine.size();
                }

                if (m_displayedChars < currentLine.size()) {
                    m_charTimer += dt;
                    if (m_charTimer >= 0.05f) {
                        m_charTimer = 0.0f;
                        m_displayedChars++;
                        context.audio.playSound("assets/sfx/sans_type.wav", 0.5f);
                    }
                } else {
                    m_linePauseTimer += dt;
                    if (m_linePauseTimer >= 0.8f || context.input.isConfirmPressed()) {
                        m_linePauseTimer = 0.0f;
                        m_displayedChars = 0;
                        m_currentLineIdx++;
                        if (m_currentLineIdx >= m_lines.size()) {
                            m_state = State::WaitingInput;
                        }
                    }
                }
            }
            break;

        case State::WaitingInput:
            if (context.input.isConfirmPressed()) {
                context.audio.playSound("assets/sfx/u_activate.wav", 0.8f);

                // Reload from save if available, otherwise restore HP
                if (context.save.hasSaveGame()) {
                    context.save.loadGame(context.state);
                } else {
                    context.player.hp = context.player.maxHp;
                }

                context.scenes.changeScene(std::make_unique<OverworldScene>(), 0.5f);
            }
            break;
    }
}

void GameOverScene::drawSoul(float x, float y) const {
    DrawRectangle((int)x + 2, (int)y, 4, 3, SOUL_RED);
    DrawRectangle((int)x + 8, (int)y, 4, 3, SOUL_RED);
    DrawRectangle((int)x + 1, (int)y + 3, 12, 4, SOUL_RED);
    DrawRectangle((int)x + 2, (int)y + 7, 10, 3, SOUL_RED);
    DrawRectangle((int)x + 4, (int)y + 10, 6, 2, SOUL_RED);
    DrawRectangle((int)x + 6, (int)y + 12, 2, 2, SOUL_RED);
}

void GameOverScene::drawCrackedSoul(float x, float y) const {
    drawSoul(x, y);
    // Jagged white crack down the middle
    DrawLine((int)x + 7, (int)y + 1, (int)x + 5, (int)y + 5, WHITE);
    DrawLine((int)x + 5, (int)y + 5, (int)x + 8, (int)y + 8, WHITE);
    DrawLine((int)x + 8, (int)y + 8, (int)x + 6, (int)y + 12, WHITE);
}

void GameOverScene::render(GameContext& context) {
    ClearBackground(BLACK);

    Font fontTitle = context.resources.getFontTitleFore(52);
    Font font8Bit  = context.resources.getFont8Bit(24);

    if (m_state == State::Frozen) {
        drawSoul(m_soulPos.x, m_soulPos.y);
    } else if (m_state == State::Cracking) {
        drawCrackedSoul(m_soulPos.x, m_soulPos.y);
    } else if (m_state == State::Shattering) {
        for (const auto& s : m_shards) {
            // Shards: small red fragments/furry tufts
            DrawRectanglePro({ s.pos.x, s.pos.y, 6.0f, 6.0f }, { 3.0f, 3.0f }, s.rotation * 57.2958f, SOUL_RED);
            DrawRectanglePro({ s.pos.x, s.pos.y, 3.0f, 3.0f }, { 1.5f, 1.5f }, s.rotation * 57.2958f, WHITE);
        }
    } else {
        // "GAME OVER" title (BND-010)
        Color titleColor = { 255, 255, 255, (unsigned char)(m_titleAlpha * 255.0f) };
        context.resources.drawCenteredText(fontTitle, "GAME OVER", 90.0f, 52, titleColor, 4.0f);

        // Render typewriter text lines
        float textY = 250.0f;
        for (size_t i = 0; i < m_currentLineIdx && i < m_lines.size(); ++i) {
            context.resources.drawCenteredText(font8Bit, m_lines[i], textY, 24, WHITE, 1.0f);
            textY += 36.0f;
        }

        if (m_state == State::TypingText && m_currentLineIdx < m_lines.size()) {
            std::string partial = m_lines[m_currentLineIdx].substr(0, m_displayedChars);
            context.resources.drawCenteredText(font8Bit, partial, textY, 24, WHITE, 1.0f);
        }

        // Retry prompt in WaitingInput state
        if (m_state == State::WaitingInput) {
            float pulse = (std::sin(GetTime() * 4.0f) + 1.0f) * 0.5f;
            Color promptColor = { 255, 255, 0, (unsigned char)(150 + pulse * 105) };
            context.resources.drawCenteredText(font8Bit, "[Press Z or ENTER to retry]", 410.0f, 20, promptColor, 1.0f);
        }
    }
}

void GameOverScene::onExit(GameContext& context) {
}

} // namespace nundertale
