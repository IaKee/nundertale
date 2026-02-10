#include "scenes/TitleScene.hpp"
#include "scenes/InstructionsScene.hpp"
#include "scenes/NameSelectScene.hpp"
#include "core/GameContext.hpp"
#include "core/Constants.hpp"
#include "core/SceneManager.hpp"
#include <raylib.h>
#include <cmath>

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

void TitleScene::onEnter(GameContext& context) {
    context.audio.playMusic("assets/music/epic_background_loop.ogg", true);
    m_selectedIndex = 0;
    m_animTimer = 0.0f;
    m_options = { "New Game", "Instructions", "Exit" };
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

void TitleScene::render(GameContext& context) {
    ClearBackground(BLACK);

    Font fontTitle = context.resources.getFontTitleFore(48);
    Font font8Bit  = context.resources.getFont8Bit(24);
    Font fontShmup = context.resources.getFontShmup(18);

    // Floating title animation
    float floatOffset = std::sin(m_animTimer * 2.5f) * 5.0f;
    float titleY = 95.0f + floatOffset;

    // "Not Undertale" in authentic Undertale title font
    context.resources.drawCenteredText(fontTitle, "Not Undertale", titleY, 48, WHITE, 3.0f);

    // Red "#" emblem next to title (faithful to nundertale.c)
    float titleW = context.resources.measureTextWidth(fontTitle, "Not Undertale", 48, 3.0f);
    float hashX = (VIRTUAL_WIDTH + titleW) * 0.5f + 8.0f;
    context.resources.drawText(fontTitle, "#", { hashX, titleY + 4 }, 32, COLOR_UNDERTALE_RED);

    // Random quote in gray
    int quoteIndex = ((int)(m_animTimer * 0.15f)) % 9;
    context.resources.drawCenteredText(fontShmup, RANDOM_QUOTES[quoteIndex], titleY + 65, 18, { 140, 140, 140, 255 }, 1.0f);

    // Menu options with 1:1 Undertale style
    int startY = 240;
    for (size_t i = 0; i < m_options.size(); ++i) {
        bool selected = (m_selectedIndex == (int)i);
        Color col = selected ? COLOR_UNDERTALE_YELLOW : WHITE;

        float optW = context.resources.measureTextWidth(font8Bit, m_options[i], 24, 1.0f);
        float optX = (VIRTUAL_WIDTH - optW) * 0.5f;
        float optY = (float)(startY + (int)i * 45);

        context.resources.drawText(font8Bit, m_options[i], { optX, optY }, 24, col, 1.0f);

        if (selected) {
            // Draw green frog soul cursor
            DrawCircle((int)optX - 25, (int)optY + 13, 6, COLOR_UNDERTALE_GREEN);
        }
    }

    // Version & Project info footer (No YouTube mention)
    const char* footer = "nUndertale v2.0 - Indie Parody Project";
    context.resources.drawCenteredText(font8Bit, footer, 440, 16, COLOR_UNDERTALE_GRAY, 1.0f);
}

void TitleScene::onExit(GameContext& context) {
}

} // namespace nundertale
