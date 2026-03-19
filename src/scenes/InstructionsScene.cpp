#include "scenes/InstructionsScene.hpp"
#include "scenes/NameSelectScene.hpp"
#include "scenes/TitleScene.hpp"
#include "core/GameContext.hpp"
#include "core/Constants.hpp"
#include "core/SceneManager.hpp"
#include <raylib.h>

namespace nundertale {

void InstructionsScene::onEnter(GameContext& context) {
    context.audio.playMusic("assets/music/main_menu_loop.ogg", true);
    m_selectedIndex = 0;
    m_settingsMessageTimer = 0.0f;
}

void InstructionsScene::update(float dt, GameContext& context) {
    if (m_settingsMessageTimer > 0.0f) {
        m_settingsMessageTimer -= dt;
        if (m_settingsMessageTimer < 0.0f) {
            m_settingsMessageTimer = 0.0f;
        }
    }

    if (context.input.isUpPressed() || context.input.isDownPressed()) {
        m_selectedIndex = 1 - m_selectedIndex;
        context.audio.playSound("assets/sfx/selection_sound.wav", 0.5f);
    }

    if (context.input.isCancelPressed() || context.input.isEscapePressed()) {
        context.audio.playSound("assets/sfx/selection_sound.wav", 0.7f);
        context.scenes.changeScene(std::make_unique<TitleScene>(), 0.3f);
        return;
    }

    if (context.input.isConfirmPressed()) {
        if (m_selectedIndex == 0) {
            context.audio.playSound("assets/sfx/u_activate.wav", 0.8f);
            context.scenes.changeScene(std::make_unique<NameSelectScene>(), 0.5f);
        } else {
            m_settingsMessageTimer = 2.0f;
            context.audio.playSound("assets/sfx/areusure.ogg", 0.8f);
        }
    }
}

void InstructionsScene::render(GameContext& context) {
    ClearBackground(BLACK);

    Font font8Bit = context.resources.getFont8Bit(22);
    Font fontShmup = context.resources.getFontShmup(16);

    context.resources.drawCenteredText(font8Bit, "--- Instruction ---", 60.0f, 30.0f, COLOR_UNDERTALE_GRAY, 2.0f);

    const char* controls[] = {
        "[Z or ENTER] - Confirm",
        "[X or SHIFT] - Cancel",
        "[C or CTRL] - Menu (In-game)",
        "[F4] - Fullscreen",
        "[Hold ESC] - Quit",
        "When HP is 0, you become a furry."
    };

    const int startY = 120;
    for (int i = 0; i < 6; ++i) {
        const Color color = (i == 5) ? COLOR_UNDERTALE_YELLOW : WHITE;
        context.resources.drawText(font8Bit, controls[i], { 170.0f, static_cast<float>(startY + i * 36) }, 20.0f, color, 1.0f);
    }

    context.resources.drawText(font8Bit, "Begin Game", { 170.0f, 360.0f }, 22.0f, m_selectedIndex == 0 ? UI_YELLOW : WHITE, 1.0f);
    context.resources.drawText(font8Bit, "Settings", { 170.0f, 400.0f }, 22.0f, m_selectedIndex == 1 ? UI_YELLOW : WHITE, 1.0f);

    if (m_settingsMessageTimer > 0.0f) {
        context.resources.drawCenteredText(fontShmup, "* Settings are still being installed. (F4 works.)", 438.0f, 16.0f, COLOR_UNDERTALE_GRAY, 1.0f);
    }
}

void InstructionsScene::onExit(GameContext& /*context*/) {
}

} // namespace nundertale
