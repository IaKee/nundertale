#include "scenes/NameSelectScene.hpp"
#include "scenes/OverworldScene.hpp"
#include "scenes/TitleScene.hpp"
#include "scenes/DisclaimerScene.hpp"
#include "core/GameContext.hpp"
#include "core/Constants.hpp"
#include "core/SceneManager.hpp"
#include "core/Transition.hpp"
#include "core/NameRules.hpp"
#include <raylib.h>
#include <algorithm>

namespace nundertale {

static int getRowColCount(int row) {
    if (row == 3 || row == 7) return 5;
    if (row == 8) return 3;
    return 7;
}

static char getLetterAt(int row, int col) {
    if (row < 4) {
        int idx = (row == 0 ? col : (row == 1 ? 7 + col : (row == 2 ? 14 + col : 21 + col)));
        return static_cast<char>('A' + idx);
    }
    int r = row - 4;
    int idx = (r == 0 ? col : (r == 1 ? 7 + col : (r == 2 ? 14 + col : 21 + col)));
    return static_cast<char>('a' + idx);
}

void NameSelectScene::onEnter(GameContext& /*context*/) {
    m_state = NameSelectState::Selecting;
    m_currentName.clear();
    m_cursorX = 0;
    m_cursorY = 0;
    m_confirmChoice = false;
    m_animTimer = 0.0f;
    m_confirmTimer = 0.0f;
    m_reaction = NameReaction{};
    m_tooManyAttempts = 0;
}

void NameSelectScene::update(float dt, GameContext& context) {
    m_animTimer += dt;

    if (m_state == NameSelectState::Selecting) {
        if (context.input.isLeftPressed()) {
            if (m_cursorY == 8) {
                m_cursorX = (m_cursorX + 2) % 3;
            } else {
                int count = getRowColCount(m_cursorY);
                m_cursorX = (m_cursorX + count - 1) % count;
            }
            context.audio.playSound("assets/sfx/selection_sound.wav", 0.4f);
        } else if (context.input.isRightPressed()) {
            if (m_cursorY == 8) {
                m_cursorX = (m_cursorX + 1) % 3;
            } else {
                int count = getRowColCount(m_cursorY);
                m_cursorX = (m_cursorX + 1) % count;
            }
            context.audio.playSound("assets/sfx/selection_sound.wav", 0.4f);
        } else if (context.input.isUpPressed()) {
            if (m_cursorY == 8) {
                m_cursorY = 7;
                if (m_cursorX == 0) m_cursorX = 0;
                else if (m_cursorX == 1) m_cursorX = 2;
                else m_cursorX = 4;
            } else if (m_cursorY > 0) {
                m_cursorY--;
                int count = getRowColCount(m_cursorY);
                if (m_cursorX >= count) m_cursorX = count - 1;
            } else {
                m_cursorY = 8;
                if (m_cursorX <= 0) m_cursorX = 0;
                else if (m_cursorX <= 3) m_cursorX = 1;
                else m_cursorX = 2;
            }
            context.audio.playSound("assets/sfx/selection_sound.wav", 0.4f);
        } else if (context.input.isDownPressed()) {
            if (m_cursorY < 7) {
                m_cursorY++;
                int count = getRowColCount(m_cursorY);
                if (m_cursorX >= count) m_cursorX = count - 1;
            } else if (m_cursorY == 7) {
                m_cursorY = 8;
                if (m_cursorX <= 0) m_cursorX = 0;
                else if (m_cursorX <= 3) m_cursorX = 1;
                else m_cursorX = 2;
            } else {
                m_cursorY = 0;
                if (m_cursorX == 0) m_cursorX = 0;
                else if (m_cursorX == 1) m_cursorX = 2;
                else m_cursorX = 5;
            }
            context.audio.playSound("assets/sfx/selection_sound.wav", 0.4f);
        }

        // Backspace / Cancel key deletes last letter
        if ((context.input.isBackspacePressed() || context.input.isCancelPressed()) && !m_currentName.empty()) {
            m_currentName.pop_back();
            context.audio.playSound("assets/sfx/type.ogg", 0.5f);
            m_tooManyAttempts = 0;
        }

        // ESC quits to TitleScene
        if (context.input.isEscapePressed()) {
            context.scenes.changeScene(std::make_unique<TitleScene>(), 0.3f);
            return;
        }

        // Confirm (Z / Enter)
        if (context.input.isConfirmPressed()) {
            if (m_cursorY < 8) {
                if (m_currentName.size() < NAME_MAX_LEN) {
                    char c = getLetterAt(m_cursorY, m_cursorX);
                    m_currentName.push_back(c);
                    context.audio.playSound("assets/sfx/type.ogg", 0.6f);
                    m_tooManyAttempts = 0;
                } else {
                    m_tooManyAttempts++;
                    context.audio.playSound("assets/sfx/areusure.ogg", 0.6f);
                }
            } else {
                // Bottom row: Quit (0), Backspace (1), Done (2)
                if (m_cursorX == 0) {
                    context.scenes.changeScene(std::make_unique<TitleScene>(), 0.3f);
                    return;
                } else if (m_cursorX == 1) {
                    if (!m_currentName.empty()) {
                        m_currentName.pop_back();
                        context.audio.playSound("assets/sfx/type.ogg", 0.5f);
                        m_tooManyAttempts = 0;
                    }
                } else if (m_cursorX == 2) {
                    if (!m_currentName.empty()) {
                        m_state = NameSelectState::Confirming;
                        m_reaction = withSameNameReaction(reactionFor(m_currentName), m_currentName, context.save.getMeta().lastName);
                        m_confirmTimer = 0.0f;
                        m_confirmChoice = false;
                        context.audio.playSound("assets/sfx/selection_sound.wav", 0.5f);
                    }
                }
            }
        }
    } else if (m_state == NameSelectState::Confirming) {
        m_confirmTimer += dt;

        if (m_reaction.allowed) {
            if (context.input.isLeftPressed() || context.input.isRightPressed()) {
                m_confirmChoice = !m_confirmChoice;
                context.audio.playSound("assets/sfx/selection_sound.wav", 0.5f);
            }
        }

        if (context.input.isCancelPressed()) {
            m_state = NameSelectState::Selecting;
            context.audio.playSound("assets/sfx/selection_sound.wav", 0.5f);
            return;
        }

        if (context.input.isConfirmPressed()) {
            if (!m_reaction.allowed) {
                m_state = NameSelectState::Selecting;
                context.audio.playSound("assets/sfx/selection_sound.wav", 0.5f);
            } else {
                if (!m_confirmChoice) {
                    m_state = NameSelectState::Selecting;
                    context.audio.playSound("assets/sfx/selection_sound.wav", 0.5f);
                } else {
                    context.player.name = m_currentName;
                    context.player.isChad = (m_reaction.effect == NameEffect::Gold);
                    context.player.isSkyrim = (m_reaction.effect == NameEffect::Skyrim);
                    context.player.isWaifuFan = (m_reaction.effect == NameEffect::Waifu);

                    if (m_reaction.effect == NameEffect::RestartGame) {
                        context.audio.playSound("assets/sfx/pipipipopopo_backwards.ogg", 1.0f);
                        context.scenes.changeScene(std::make_unique<DisclaimerScene>(), cut());
                    } else {
                        MetaData meta = context.save.getMeta();
                        meta.lastName = m_currentName;
                        context.save.saveMeta(meta);
                        context.audio.playSound("assets/sfx/u_activate.wav", 1.0f);
                        context.scenes.changeScene(std::make_unique<OverworldScene>(), fadeWhite(3.0f, 0.6f));
                    }
                }
            }
        }
    }
}

void NameSelectScene::render(GameContext& context) {
    ClearBackground(BLACK);

    Font font8Bit = context.resources.getFont8Bit(24);

    if (m_state == NameSelectState::Selecting) {
        if (normalizeName(m_currentName) == "SKYRIM") {
            Texture2D skyrimTex = context.resources.getTexture("assets/img/whoru.bmp");
            DrawTexturePro(skyrimTex, { 0, 0, static_cast<float>(skyrimTex.width), static_cast<float>(skyrimTex.height) },
                           { 0, 0, static_cast<float>(VIRTUAL_WIDTH), static_cast<float>(VIRTUAL_HEIGHT) }, { 0, 0 }, 0.0f, { 255, 255, 255, 70 });
        }

        // Title "Name the fallen gamer." centered near y=58
        context.resources.drawCenteredText(font8Bit, "Name the fallen gamer.", 58.0f, 24.0f, WHITE, 2.0f);

        // Typed name (as typed, case preserved) drawn at x≈280,y≈110 (2x main font)
        if (!m_currentName.empty()) {
            DrawTextEx(font8Bit, m_currentName.c_str(), { 280.0f, 110.0f }, 32.0f, 2.0f, WHITE);
        }

        // Extra attempts warning
        if (m_tooManyAttempts > 0) {
            const char* msg = (m_tooManyAttempts > 4) ? "Stop it... Get some help..." : "You only need 6 characters...";
            context.resources.drawCenteredText(font8Bit, msg, 140.0f, 16.0f, COLOR_UNDERTALE_RED, 1.0f);
        }

        // Uppercase A-Z in rows of 7 starting x=120, y=152, step x=64, step y=28
        for (int r = 0; r < 4; ++r) {
            int cols = (r == 3) ? 5 : 7;
            for (int c = 0; c < cols; ++c) {
                char ch = getLetterAt(r, c);
                std::string s(1, ch);
                bool isSel = (m_cursorY == r && m_cursorX == c);
                Color col = isSel ? COLOR_UNDERTALE_YELLOW : WHITE;
                Vector2 pos = { 120.0f + static_cast<float>(c) * 64.0f, 152.0f + static_cast<float>(r) * 28.0f };
                context.resources.drawShakyText(font8Bit, s, pos, 24.0f, col, 0.8f, 1.0f);
            }
        }

        // Lowercase a-z in rows of 7 starting y=272
        for (int r = 0; r < 4; ++r) {
            int cols = (r == 3) ? 5 : 7;
            for (int c = 0; c < cols; ++c) {
                char ch = getLetterAt(r + 4, c);
                std::string s(1, ch);
                bool isSel = (m_cursorY == (r + 4) && m_cursorX == c);
                Color col = isSel ? COLOR_UNDERTALE_YELLOW : WHITE;
                Vector2 pos = { 120.0f + static_cast<float>(c) * 64.0f, 272.0f + static_cast<float>(r) * 28.0f };
                context.resources.drawShakyText(font8Bit, s, pos, 24.0f, col, 0.8f, 1.0f);
            }
        }

        // Bottom row at y≈400: Quit (x≈120), Backspace (x≈240), Done (x≈440)
        bool quitSel = (m_cursorY == 8 && m_cursorX == 0);
        bool backSel = (m_cursorY == 8 && m_cursorX == 1);
        bool doneSel = (m_cursorY == 8 && m_cursorX == 2);

        Color quitCol = quitSel ? COLOR_UNDERTALE_YELLOW : WHITE;
        Color backCol = backSel ? COLOR_UNDERTALE_YELLOW : WHITE;
        Color doneCol = doneSel ? COLOR_UNDERTALE_YELLOW : (m_currentName.empty() ? COLOR_UNDERTALE_DARK : WHITE);

        context.resources.drawText(font8Bit, "Quit", { 120.0f, 400.0f }, 24.0f, quitCol, 1.0f);
        context.resources.drawText(font8Bit, "Backspace", { 240.0f, 400.0f }, 24.0f, backCol, 1.0f);
        context.resources.drawText(font8Bit, "Done", { 440.0f, 400.0f }, 24.0f, doneCol, 1.0f);

    } else if (m_state == NameSelectState::Confirming) {
        if (m_reaction.effect == NameEffect::Skyrim) {
            Texture2D skyrimTex = context.resources.getTexture("assets/img/whoru.bmp");
            DrawTexturePro(skyrimTex, { 0, 0, static_cast<float>(skyrimTex.width), static_cast<float>(skyrimTex.height) },
                           { 0, 0, static_cast<float>(VIRTUAL_WIDTH), static_cast<float>(VIRTUAL_HEIGHT) }, { 0, 0 }, 0.0f, { 255, 255, 255, 70 });
        }

        // Top text: "Are you sure about that?" (keep), and below it the reaction message (cyan) if any; otherwise "Nice name bro." (length>1)
        context.resources.drawCenteredText(font8Bit, "Are you sure about that?", 60.0f, 24.0f, WHITE, 2.0f);

        if (!m_reaction.message.empty()) {
            context.resources.drawCenteredText(font8Bit, m_reaction.message, 95.0f, 18.0f, COLOR_UNDERTALE_CYAN, 1.0f);
        } else if (m_currentName.size() > 1) {
            context.resources.drawCenteredText(font8Bit, "Nice name bro.", 95.0f, 18.0f, WHITE, 1.0f);
        }

        // The name starts at the typed-name position/size and slowly grows over ~4 s to ~3.5x while moving to screen center, trembling
        float progress = std::clamp(m_confirmTimer / 4.0f, 0.0f, 1.0f);
        float scaleFactor = 1.0f + 2.5f * progress; // 1.0x to 3.5x
        float fontSize = 32.0f * scaleFactor;
        float spacing = 2.0f * scaleFactor;

        Color nameColor = WHITE;
        if (m_reaction.effect == NameEffect::Gold) {
            nameColor = COLOR_UNDERTALE_YELLOW;
        } else if (m_reaction.effect == NameEffect::Waifu) {
            nameColor = Color{ 255, 105, 180, 255 };
        } else if (m_reaction.effect == NameEffect::FrogSoul) {
            nameColor = COLOR_UNDERTALE_GREEN;
        } else if (m_reaction.effect == NameEffect::CrimsonShake) {
            nameColor = COLOR_UNDERTALE_RED;
        }

        float shakeAmount = (m_reaction.effect == NameEffect::CrimsonShake) ? 5.0f : 1.5f;

        Vector2 textSize = MeasureTextEx(font8Bit, m_currentName.c_str(), fontSize, spacing);
        float startX = 280.0f;
        float startY = 110.0f;
        float targetX = (static_cast<float>(VIRTUAL_WIDTH) - textSize.x) * 0.5f;
        float targetY = 200.0f;

        float curX = startX + (targetX - startX) * progress;
        float curY = startY + (targetY - startY) * progress;

        for (size_t i = 0; i < m_currentName.size(); ++i) {
            char ch = m_currentName[i];
            std::string s(1, ch);
            float jx = (static_cast<float>(GetRandomValue(-100, 100)) / 100.0f) * shakeAmount;
            float jy = (static_cast<float>(GetRandomValue(-100, 100)) / 100.0f) * shakeAmount;
            DrawTextEx(font8Bit, s.c_str(), { curX + jx, curY + jy }, fontSize, spacing, nameColor);
            curX += MeasureTextEx(font8Bit, s.c_str(), fontSize, spacing).x + spacing;
        }

        // Tag (e.g. [GOLD]) in yellow under it once grown
        if (progress >= 1.0f && !m_reaction.tag.empty()) {
            context.resources.drawCenteredText(font8Bit, m_reaction.tag, curY + textSize.y + 16.0f, 20.0f, COLOR_UNDERTALE_YELLOW, 2.0f);
        }

        // Choices under the name at y≈400: No (x≈146) and Yes (x≈460) with yellow selection; Left/Right switch. If !allowed: only Go back (centered)
        if (!m_reaction.allowed) {
            context.resources.drawCenteredText(font8Bit, "Go back", 400.0f, 24.0f, COLOR_UNDERTALE_YELLOW, 2.0f);
        } else {
            Color noColor = (!m_confirmChoice) ? COLOR_UNDERTALE_YELLOW : WHITE;
            Color yesColor = (m_confirmChoice) ? COLOR_UNDERTALE_YELLOW : WHITE;
            context.resources.drawText(font8Bit, "No", { 146.0f, 400.0f }, 24.0f, noColor, 2.0f);
            context.resources.drawText(font8Bit, "Yes", { 460.0f, 400.0f }, 24.0f, yesColor, 2.0f);
        }

        // Darken effect: translucent black overlay (~alpha 150)
        if (m_reaction.effect == NameEffect::Darken) {
            DrawRectangle(0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT, Color{ 0, 0, 0, 150 });
        }
    }
}

void NameSelectScene::onExit(GameContext& /*context*/) {
}

} // namespace nundertale
