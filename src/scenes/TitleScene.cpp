#include "scenes/TitleScene.hpp"
#include "scenes/FrontEndFlow.hpp"
#include "scenes/InstructionsScene.hpp"
#include "scenes/LoreScene.hpp"
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

void TitleScene::resetSplash() {
    m_screen = Screen::Splash;
    m_selectedIndex = 0;
    m_animTimer = 0.0f;
    m_idleTimer = 0.0f;
    m_settingsMessageTimer = 0.0f;
    m_promptSoundPlayed = false;
}

void TitleScene::onEnter(GameContext& context) {
    context.audio.playMusic("assets/music/main_menu_loop.ogg", true);
    resetSplash();

    m_hasSave = context.save.hasSaveGame();
    if (m_hasSave) {
        GameState preview;
        if (context.save.loadGame(preview)) {
            m_savedName = preview.player.name;
            m_savedLevel = preview.player.level;
            m_savedPlayTime = preview.playTime;
            m_savedRoom = context.save.getRoomDisplayName(preview.currentRoom);
        }
    }
}

void TitleScene::update(float dt, GameContext& context) {
    m_animTimer += dt;

    if (m_screen == Screen::Splash) {
        updateSplash(dt, context);
    } else {
        updateContinueCard(dt, context);
    }
}

void TitleScene::updateSplash(float dt, GameContext& context) {
    if (!m_promptSoundPlayed && pressPromptVisible(m_animTimer)) {
        context.audio.playSound("assets/sfx/n.ogg", 0.8f);
        m_promptSoundPlayed = true;
    }

    const bool confirmPressed = context.input.isConfirmPressed();
    const bool anyInput = confirmPressed
        || context.input.isCancelPressed()
        || context.input.isEscapePressed()
        || context.input.isMenuPressed()
        || context.input.isBackspacePressed()
        || context.input.isUpPressed()
        || context.input.isDownPressed()
        || context.input.isLeftPressed()
        || context.input.isRightPressed();

    if (anyInput) {
        m_idleTimer = 0.0f;
    } else {
        m_idleTimer += dt;
    }

    const bool acceptedConfirm = confirmPressed && pressPromptVisible(m_animTimer);
    switch (titleNext(acceptedConfirm, m_hasSave, m_idleTimer)) {
        case TitleNext::Instructions:
            context.audio.playSound("assets/sfx/u_activate.wav", 0.8f);
            context.scenes.changeScene(std::make_unique<InstructionsScene>(), 0.5f);
            break;
        case TitleNext::ContinueCard:
            context.audio.playSound("assets/sfx/u_activate.wav", 0.8f);
            m_screen = Screen::ContinueCard;
            m_selectedIndex = 0;
            m_settingsMessageTimer = 0.0f;
            break;
        case TitleNext::Intro:
            context.scenes.changeScene(std::make_unique<LoreScene>(), 0.5f);
            break;
        case TitleNext::Stay:
            break;
    }
}

void TitleScene::showSettingsPlaceholder(GameContext& context) {
    m_settingsMessageTimer = 2.0f;
    context.audio.playSound("assets/sfx/areusure.ogg", 0.8f);
}

void TitleScene::updateContinueCard(float dt, GameContext& context) {
    if (m_settingsMessageTimer > 0.0f) {
        m_settingsMessageTimer -= dt;
        if (m_settingsMessageTimer < 0.0f) {
            m_settingsMessageTimer = 0.0f;
        }
    }

    int nextSelection = m_selectedIndex;
    if (context.input.isLeftPressed()) {
        nextSelection = 0;
    } else if (context.input.isRightPressed()) {
        nextSelection = 1;
    } else if (context.input.isDownPressed() && m_selectedIndex != 2) {
        nextSelection = 2;
    } else if (context.input.isUpPressed() && m_selectedIndex == 2) {
        nextSelection = 0;
    }

    if (nextSelection != m_selectedIndex) {
        m_selectedIndex = nextSelection;
        context.audio.playSound("assets/sfx/selection_sound.wav", 0.5f);
    }

    if (context.input.isCancelPressed()) {
        context.audio.playSound("assets/sfx/selection_sound.wav", 0.7f);
        resetSplash();
        return;
    }

    if (context.input.isConfirmPressed()) {
        if (m_selectedIndex == 0) {
            context.audio.playSound("assets/sfx/u_activate.wav", 0.8f);
            context.save.loadGame(context.state);
            context.scenes.changeScene(std::make_unique<OverworldScene>(), 0.5f);
        } else if (m_selectedIndex == 1) {
            context.audio.playSound("assets/sfx/u_activate.wav", 0.8f);
            context.save.deleteSaveGame();
            context.state = GameState();
            context.scenes.changeScene(std::make_unique<NameSelectScene>(), 0.5f);
        } else {
            showSettingsPlaceholder(context);
        }
    }
}

void TitleScene::render(GameContext& context) {
    ClearBackground(BLACK);

    if (m_screen == Screen::Splash) {
        renderSplash(context);
    } else {
        renderContinueCard(context);
    }
}

void TitleScene::renderSplash(GameContext& context) {
    Font fontTitle = context.resources.getFontTitleFore(48);
    Font fontShmup = context.resources.getFontShmup(18);

    const float floatOffset = std::sin(m_animTimer * 2.5f) * 3.0f;
    const float titleY = 200.0f + floatOffset;
    const float fontSize = 48.0f;
    const float spacing = 3.0f;
    const float nWidth = context.resources.measureTextWidth(fontTitle, "n", fontSize, spacing);
    const float undertaleWidth = context.resources.measureTextWidth(fontTitle, "UNDERTALE", fontSize, spacing);
    const float titleX = (static_cast<float>(VIRTUAL_WIDTH) - nWidth - undertaleWidth) * 0.5f;
    const float jitterX = (std::sin(m_animTimer * 29.0f) >= 0.0f) ? 1.0f : -1.0f;
    const float jitterY = (std::sin(m_animTimer * 37.0f) >= 0.0f) ? 1.0f : -1.0f;

    context.resources.drawText(fontTitle, "n", { titleX + jitterX, titleY + jitterY }, fontSize, SOUL_RED, spacing);
    context.resources.drawText(fontTitle, "UNDERTALE", { titleX + nWidth, titleY }, fontSize, WHITE, spacing);

    const int quoteIndex = static_cast<int>(m_animTimer * 0.15f) % 9;
    context.resources.drawCenteredText(fontShmup, RANDOM_QUOTES[quoteIndex], 250.0f, 18.0f, { 140, 140, 140, 255 }, 1.0f);

    if (pressPromptVisible(m_animTimer)) {
        context.resources.drawCenteredText(fontShmup, "[PRESS Z OR ENTER]", 360.0f, 16.0f, COLOR_UNDERTALE_GRAY, 1.0f);
    }
}

void TitleScene::renderContinueCard(GameContext& context) {
    Font font8Bit = context.resources.getFont8Bit(22);
    Font fontShmup = context.resources.getFontShmup(16);

    const int totalSeconds = static_cast<int>(m_savedPlayTime);
    const int minutes = totalSeconds / 60;
    const int seconds = totalSeconds % 60;
    char timeBuffer[16];
    std::snprintf(timeBuffer, sizeof(timeBuffer), "%d:%02d", minutes, seconds);

    context.resources.drawText(font8Bit, m_savedName, { 140.0f, 120.0f }, 22.0f, WHITE, 1.0f);
    context.resources.drawText(font8Bit, "LV " + std::to_string(m_savedLevel), { 300.0f, 120.0f }, 22.0f, WHITE, 1.0f);
    context.resources.drawText(font8Bit, timeBuffer, { 440.0f, 120.0f }, 22.0f, WHITE, 1.0f);
    context.resources.drawText(font8Bit, m_savedRoom, { 140.0f, 160.0f }, 22.0f, WHITE, 1.0f);

    context.resources.drawText(font8Bit, "Continue", { 170.0f, 210.0f }, 22.0f, m_selectedIndex == 0 ? UI_YELLOW : WHITE, 1.0f);
    context.resources.drawText(font8Bit, "Reset", { 390.0f, 210.0f }, 22.0f, m_selectedIndex == 1 ? UI_YELLOW : WHITE, 1.0f);
    context.resources.drawCenteredText(font8Bit, "Settings", 260.0f, 22.0f, m_selectedIndex == 2 ? UI_YELLOW : WHITE, 1.0f);

    if (m_settingsMessageTimer > 0.0f) {
        context.resources.drawCenteredText(fontShmup, "* Settings are still being installed. (F4 works.)", 310.0f, 16.0f, COLOR_UNDERTALE_GRAY, 1.0f);
    }
}

void TitleScene::onExit(GameContext& /*context*/) {
}

} // namespace nundertale
