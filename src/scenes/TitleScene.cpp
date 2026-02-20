#include "scenes/TitleScene.hpp"
#include "scenes/InstructionsScene.hpp"
#include "scenes/NameSelectScene.hpp"
#include "scenes/OverworldScene.hpp"
#include "core/GameContext.hpp"
#include "core/Constants.hpp"
#include "core/SceneManager.hpp"
#include <raylib.h>
#include <cmath>
#include <cstdio>

namespace nundertale {

static const char* RANDOM_QUOTES[9] = {
    "definitely not undertails",
    "177013",
    "warning: you are cringe",
    "never underestimate your inabilities",
    "don't be afraid of failure. be sure of it.",
    "floor gang",
    "also try terraria",
    "reeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",
    "so it begins"
};

void TitleScene::drawSoul(float x, float y) const {
    DrawRectangle((int)x + 2, (int)y, 4, 3, SOUL_RED);
    DrawRectangle((int)x + 8, (int)y, 4, 3, SOUL_RED);
    DrawRectangle((int)x + 1, (int)y + 3, 12, 4, SOUL_RED);
    DrawRectangle((int)x + 2, (int)y + 7, 10, 3, SOUL_RED);
    DrawRectangle((int)x + 4, (int)y + 10, 6, 2, SOUL_RED);
    DrawRectangle((int)x + 6, (int)y + 12, 2, 2, SOUL_RED);
}

void TitleScene::onEnter(GameContext& context) {
    context.audio.playMusic("assets/music/epic_background_loop.ogg", true);
    m_selectedIndex = 0;
    m_animTimer = 0.0f;

    m_hasSave = context.save.hasSaveGame();
    if (m_hasSave) {
        GameState preview;
        if (context.save.loadGame(preview)) {
            m_savedName = preview.player.name;
            m_savedLevel = preview.player.level;
            m_savedPlayTime = preview.playTime;
            m_savedRoom = context.save.getRoomDisplayName(preview.currentRoom);
        }
        m_options = { "Continue", "Reset", "Instructions", "Exit" };
    } else {
        m_options = { "New Game", "Instructions", "Exit" };
    }
}

void TitleScene::update(float dt, GameContext& context) {
    m_animTimer += dt;

    if (context.input.isUpPressed()) {
        m_selectedIndex = (m_selectedIndex + (int)m_options.size() - 1) % (int)m_options.size();
        context.audio.playSound("assets/sfx/selection_sound.wav", 0.5f);
    } else if (context.input.isDownPressed()) {
        m_selectedIndex = (m_selectedIndex + 1) % (int)m_options.size();
        context.audio.playSound("assets/sfx/selection_sound.wav", 0.5f);
    }

    if (context.input.isConfirmPressed()) {
        context.audio.playSound("assets/sfx/u_activate.wav", 0.8f);

        if (m_hasSave) {
            switch (m_selectedIndex) {
                case 0: // Continue
                    context.save.loadGame(context.state);
                    context.scenes.changeScene(std::make_unique<OverworldScene>(), 0.5f);
                    break;
                case 1: // Reset
                    context.save.deleteSaveGame();
                    context.state = GameState();
                    context.scenes.changeScene(std::make_unique<NameSelectScene>(), 0.5f);
                    break;
                case 2: // Instructions
                    context.scenes.changeScene(std::make_unique<InstructionsScene>(), 0.4f);
                    break;
                case 3: // Exit
                    context.isRunning = false;
                    break;
            }
        } else {
            switch (m_selectedIndex) {
                case 0: // New Game
                    context.scenes.changeScene(std::make_unique<NameSelectScene>(), 0.5f);
                    break;
                case 1: // Instructions
                    context.scenes.changeScene(std::make_unique<InstructionsScene>(), 0.4f);
                    break;
                case 2: // Exit
                    context.isRunning = false;
                    break;
            }
        }
    }
}

void TitleScene::render(GameContext& context) {
    ClearBackground(BLACK);

    Font fontTitle = context.resources.getFontTitleFore(48);
    Font font8Bit  = context.resources.getFont8Bit(22);
    Font fontShmup = context.resources.getFontShmup(18);

    // Floating title animation
    float floatOffset = std::sin(m_animTimer * 2.5f) * 5.0f;
    float titleY = 75.0f + floatOffset;

    // "Not Undertale" in authentic Undertale title font
    context.resources.drawCenteredText(fontTitle, "Not Undertale", titleY, 48, WHITE, 3.0f);

    // Red "#" emblem next to title (faithful to nundertale.c)
    float titleW = context.resources.measureTextWidth(fontTitle, "Not Undertale", 48, 3.0f);
    float hashX = (VIRTUAL_WIDTH + titleW) * 0.5f + 8.0f;
    context.resources.drawText(fontTitle, "#", { hashX, titleY + 4 }, 32, SOUL_RED);

    // Random quote in gray
    int quoteIndex = ((int)(m_animTimer * 0.15f)) % 9;
    context.resources.drawCenteredText(fontShmup, RANDOM_QUOTES[quoteIndex], titleY + 60, 18, { 140, 140, 140, 255 }, 1.0f);

    int startY = 220;

    // If save exists, render Undertale Continue card (SAV-004, FRT-005)
    if (m_hasSave) {
        int totalSec = (int)m_savedPlayTime;
        int mins = totalSec / 60;
        int secs = totalSec % 60;
        char timeBuf[16];
        std::snprintf(timeBuf, sizeof(timeBuf), "%d:%02d", mins, secs);

        float cardY = 175.0f;
        context.resources.drawText(font8Bit, m_savedName, { 170.0f, cardY }, 20, WHITE, 1.0f);
        context.resources.drawText(font8Bit, "LV " + std::to_string(m_savedLevel), { 300.0f, cardY }, 20, WHITE, 1.0f);
        context.resources.drawText(font8Bit, timeBuf, { 420.0f, cardY }, 20, WHITE, 1.0f);
        context.resources.drawText(font8Bit, m_savedRoom, { 170.0f, cardY + 26.0f }, 20, WHITE, 1.0f);

        startY = 245;
    }

    // Menu options with 1:1 Undertale style
    for (size_t i = 0; i < m_options.size(); ++i) {
        bool selected = (m_selectedIndex == (int)i);
        Color col = selected ? UI_YELLOW : WHITE;

        float optW = context.resources.measureTextWidth(font8Bit, m_options[i], 22, 1.0f);
        float optX = (VIRTUAL_WIDTH - optW) * 0.5f;
        float optY = (float)(startY + (int)i * 38);

        context.resources.drawText(font8Bit, m_options[i], { optX, optY }, 22, col, 1.0f);

        if (selected) {
            // Authentic 16x16 red soul cursor (BUI-007)
            drawSoul(optX - 25, optY + 5);
        }
    }

    // Version footer
    const char* footer = "nUndertale v2.0 - Parody Project";
    context.resources.drawCenteredText(font8Bit, footer, 445, 16, { 128, 128, 128, 255 }, 1.0f);
}

void TitleScene::onExit(GameContext& context) {
}

} // namespace nundertale
