#include "scenes/GameOverScene.hpp"
#include "scenes/OverworldScene.hpp"
#include "scenes/FurryOver.hpp"
#include "core/GameContext.hpp"
#include "core/Constants.hpp"
#include "core/SceneManager.hpp"
#include <cmath>
#include <algorithm>

namespace nundertale {

GameOverScene::GameOverScene(Vector2 deathSoulPos, std::string enemyId)
    : m_soulPos(deathSoulPos), m_enemyId(std::move(enemyId)) {}

void GameOverScene::onEnter(GameContext& context) {
    context.audio.stopMusic();
    m_state = State::Frozen;
    m_stateTimer = 0.0f;
    m_pompoms.clear();
    m_shards.clear();
    m_titleAlpha = 0.0f;

    auto meta = context.save.getMeta();
    int timesTransformed = meta.timesTransformed;
    meta.timesTransformed++;
    context.save.saveMeta(meta);

    bool toxicRoute = (context.state.getRoute() == GameState::RouteType::ToxicGamer);
    int pick = GetRandomValue(0, 5);
    m_lines = furryOverLines(context.player.name, m_enemyId, timesTransformed, toxicRoute, pick);

    m_currentLineIdx = 0;
    m_displayedChars = 0;
    m_charTimer = 0.0f;
    m_linePauseTimer = 0.0f;
}

void GameOverScene::spawnPomPoms() {
    m_pompoms.clear();
    const Color colors[3] = {
        Color{ 255, 170, 220, 255 },
        Color{ 255, 120, 200, 255 },
        WHITE
    };

    float cx = m_soulPos.x + 7.0f;
    float cy = m_soulPos.y + 7.0f;

    for (int i = 0; i < 15; ++i) {
        float angle = static_cast<float>(i) * (2.0f * 3.14159265f / 15.0f);
        float speed = 120.0f + static_cast<float>((i * 37) % 120);
        float r = 3.0f + static_cast<float>((i * 7) % 4);
        PomPom p;
        p.pos = { cx, cy };
        p.vel = { std::cos(angle) * speed, std::sin(angle) * speed - 60.0f };
        p.radius = r;
        p.color = colors[static_cast<size_t>(i % 3)];
        p.alpha = 1.0f;
        m_pompoms.push_back(p);
    }
}

void GameOverScene::update(float dt, GameContext& context) {
    m_stateTimer += dt;

    switch (m_state) {
        case State::Frozen:
            if (m_stateTimer >= 0.5f) {
                m_state = State::Sprouting;
                m_stateTimer = 0.0f;
                context.audio.playSound("assets/sfx/type.ogg", 0.8f);
            }
            break;

        case State::Sprouting:
            if (m_stateTimer >= 0.8f) {
                m_state = State::Inflating;
                m_stateTimer = 0.0f;
            }
            break;

        case State::Inflating:
            if (m_stateTimer >= 0.7f) {
                m_state = State::Pomf;
                m_stateTimer = 0.0f;
                spawnPomPoms();
                context.flash(Color{ 255, 170, 220, 255 }, 0.15f);
                context.shake(4.0f, 0.2f);
                context.audio.playSound("assets/sfx/cartoon_slip.ogg", 0.9f);
            }
            break;

        case State::Pomf:
            for (auto& p : m_pompoms) {
                p.vel.y += 320.0f * dt; // gravity
                p.pos.x += p.vel.x * dt;
                p.pos.y += p.vel.y * dt;
                p.alpha -= dt * 1.0f;
                if (p.alpha < 0.0f) p.alpha = 0.0f;
            }
            if (m_stateTimer >= 1.0f) {
                m_state = State::FadingText;
                m_stateTimer = 0.0f;
                context.audio.playMusic("assets/music/epic_background_loop.ogg", true);
            }
            break;

        case State::FadingText:
            m_titleAlpha = m_stateTimer / 1.5f;
            if (m_titleAlpha > 1.0f) m_titleAlpha = 1.0f;

            if (m_stateTimer >= 1.8f) {
                m_titleAlpha = 1.0f;
                if (m_lines.empty()) {
                    m_state = State::WaitingInput;
                } else {
                    m_state = State::TypingText;
                    m_stateTimer = 0.0f;
                }
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
            } else {
                m_state = State::WaitingInput;
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
    DrawRectangle(static_cast<int>(x) + 2, static_cast<int>(y), 4, 3, SOUL_RED);
    DrawRectangle(static_cast<int>(x) + 8, static_cast<int>(y), 4, 3, SOUL_RED);
    DrawRectangle(static_cast<int>(x) + 1, static_cast<int>(y) + 3, 12, 4, SOUL_RED);
    DrawRectangle(static_cast<int>(x) + 2, static_cast<int>(y) + 7, 10, 3, SOUL_RED);
    DrawRectangle(static_cast<int>(x) + 4, static_cast<int>(y) + 10, 6, 2, SOUL_RED);
    DrawRectangle(static_cast<int>(x) + 6, static_cast<int>(y) + 12, 2, 2, SOUL_RED);
}

void GameOverScene::drawSproutingSoul(float x, float y, float progress) const {
    drawSoul(x, y);

    Color pink = { 255, 170, 220, 255 };

    // Left ear (small triangle pointing up)
    Vector2 l1 = { x + 2.0f, y };
    Vector2 l2 = { x + 6.0f, y };
    Vector2 l3 = { x + 4.0f, y - 6.0f * progress };
    DrawTriangle(l1, l2, l3, pink);

    // Right ear (small triangle pointing up)
    Vector2 r1 = { x + 8.0f, y };
    Vector2 r2 = { x + 12.0f, y };
    Vector2 r3 = { x + 10.0f, y - 6.0f * progress };
    DrawTriangle(r1, r2, r3, pink);

    // Curly tail from bottom-right (x + 12, y + 8)
    Vector2 t0 = { x + 12.0f, y + 8.0f };
    Vector2 t1 = { x + 12.0f + 4.0f * progress, y + 8.0f - 1.0f * progress };
    Vector2 t2 = { x + 12.0f + 7.0f * progress, y + 8.0f - 5.0f * progress };
    Vector2 t3 = { x + 12.0f + 4.0f * progress, y + 8.0f - 8.0f * progress };
    Vector2 t4 = { x + 12.0f + 2.0f * progress, y + 8.0f - 6.0f * progress };

    DrawLineEx(t0, t1, 2.0f, pink);
    DrawLineEx(t1, t2, 2.0f, pink);
    DrawLineEx(t2, t3, 2.0f, pink);
    DrawLineEx(t3, t4, 2.0f, pink);
}

void GameOverScene::drawScaledSoul(float x, float y, float scaleX, float scaleY) const {
    float cx = x + 7.0f;
    float cy = y + 7.0f;

    auto drawScaledRect = [&](float rx, float ry, float rw, float rh, Color color) {
        float nx = cx + (rx - cx) * scaleX;
        float ny = cy + (ry - cy) * scaleY;
        float nw = rw * scaleX;
        float nh = rh * scaleY;
        DrawRectangle(static_cast<int>(std::round(nx)),
                      static_cast<int>(std::round(ny)),
                      std::max(1, static_cast<int>(std::round(nw))),
                      std::max(1, static_cast<int>(std::round(nh))),
                      color);
    };

    auto drawScaledTriangle = [&](Vector2 v1, Vector2 v2, Vector2 v3, Color color) {
        Vector2 sv1 = { cx + (v1.x - cx) * scaleX, cy + (v1.y - cy) * scaleY };
        Vector2 sv2 = { cx + (v2.x - cx) * scaleX, cy + (v2.y - cy) * scaleY };
        Vector2 sv3 = { cx + (v3.x - cx) * scaleX, cy + (v3.y - cy) * scaleY };
        DrawTriangle(sv1, sv2, sv3, color);
    };

    auto drawScaledLine = [&](Vector2 p1, Vector2 p2, float thick, Color color) {
        Vector2 sp1 = { cx + (p1.x - cx) * scaleX, cy + (p1.y - cy) * scaleY };
        Vector2 sp2 = { cx + (p2.x - cx) * scaleX, cy + (p2.y - cy) * scaleY };
        DrawLineEx(sp1, sp2, thick * ((scaleX + scaleY) * 0.5f), color);
    };

    // Scaled soul body
    drawScaledRect(x + 2.0f, y, 4.0f, 3.0f, SOUL_RED);
    drawScaledRect(x + 8.0f, y, 4.0f, 3.0f, SOUL_RED);
    drawScaledRect(x + 1.0f, y + 3.0f, 12.0f, 4.0f, SOUL_RED);
    drawScaledRect(x + 2.0f, y + 7.0f, 10.0f, 3.0f, SOUL_RED);
    drawScaledRect(x + 4.0f, y + 10.0f, 6.0f, 2.0f, SOUL_RED);
    drawScaledRect(x + 6.0f, y + 12.0f, 2.0f, 2.0f, SOUL_RED);

    Color pink = { 255, 170, 220, 255 };

    // Scaled ears
    Vector2 l1 = { x + 2.0f, y };
    Vector2 l2 = { x + 6.0f, y };
    Vector2 l3 = { x + 4.0f, y - 6.0f };
    drawScaledTriangle(l1, l2, l3, pink);

    Vector2 r1 = { x + 8.0f, y };
    Vector2 r2 = { x + 12.0f, y };
    Vector2 r3 = { x + 10.0f, y - 6.0f };
    drawScaledTriangle(r1, r2, r3, pink);

    // Scaled curly tail
    Vector2 t0 = { x + 12.0f, y + 8.0f };
    Vector2 t1 = { x + 16.0f, y + 7.0f };
    Vector2 t2 = { x + 19.0f, y + 3.0f };
    Vector2 t3 = { x + 16.0f, y };
    Vector2 t4 = { x + 14.0f, y + 2.0f };

    drawScaledLine(t0, t1, 2.0f, pink);
    drawScaledLine(t1, t2, 2.0f, pink);
    drawScaledLine(t2, t3, 2.0f, pink);
    drawScaledLine(t3, t4, 2.0f, pink);
}

void GameOverScene::drawCrackedSoul(float x, float y) const {
    drawSoul(x, y);
    // Jagged white crack down the middle (preserved for future FLF-007)
    DrawLine(static_cast<int>(x) + 7, static_cast<int>(y) + 1, static_cast<int>(x) + 5, static_cast<int>(y) + 5, WHITE);
    DrawLine(static_cast<int>(x) + 5, static_cast<int>(y) + 5, static_cast<int>(x) + 8, static_cast<int>(y) + 8, WHITE);
    DrawLine(static_cast<int>(x) + 8, static_cast<int>(y) + 8, static_cast<int>(x) + 6, static_cast<int>(y) + 12, WHITE);
}

void GameOverScene::render(GameContext& context) {
    ClearBackground(BLACK);

    Font fontTitle = context.resources.getFontTitleFore(52);
    Font font8Bit  = context.resources.getFont8Bit(24);

    if (m_state == State::Frozen) {
        drawSoul(m_soulPos.x, m_soulPos.y);
    } else if (m_state == State::Sprouting) {
        float progress = std::clamp(m_stateTimer / 0.8f, 0.0f, 1.0f);
        drawSproutingSoul(m_soulPos.x, m_soulPos.y, progress);
    } else if (m_state == State::Inflating) {
        float progress = std::clamp(m_stateTimer / 0.7f, 0.0f, 1.0f);
        float scale = 1.0f + 1.2f * progress;
        float wobble = std::sin(m_stateTimer * 40.0f) * 0.08f * progress;
        drawScaledSoul(m_soulPos.x, m_soulPos.y, scale + wobble, scale - wobble);
    } else if (m_state == State::Pomf) {
        for (const auto& p : m_pompoms) {
            Color c = p.color;
            c.a = static_cast<unsigned char>(std::clamp(p.alpha * 255.0f, 0.0f, 255.0f));
            DrawCircle(static_cast<int>(std::round(p.pos.x)),
                       static_cast<int>(std::round(p.pos.y)),
                       p.radius,
                       c);
        }
    } else {
        // "FURRY OVER" title (BND-010)
        Color titleColor = { 255, 255, 255, static_cast<unsigned char>(std::clamp(m_titleAlpha * 255.0f, 0.0f, 255.0f)) };
        context.resources.drawCenteredText(fontTitle, "FURRY", 120.0f, 52, titleColor, 4.0f);
        context.resources.drawCenteredText(fontTitle, "OVER", 190.0f, 52, titleColor, 4.0f);

        // Render typewriter text lines
        float textY = 270.0f;
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
            float pulse = (std::sin(static_cast<float>(GetTime()) * 4.0f) + 1.0f) * 0.5f;
            Color promptColor = { 255, 255, 0, static_cast<unsigned char>(150.0f + pulse * 105.0f) };
            context.resources.drawCenteredText(font8Bit, "[Press Z or ENTER to retry]", 410.0f, 20, promptColor, 1.0f);
        }
    }
}

void GameOverScene::onExit(GameContext& /*context*/) {
}

} // namespace nundertale
