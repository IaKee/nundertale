#include "scenes/InstructionsScene.hpp"
#include "scenes/TitleScene.hpp"
#include "core/GameContext.hpp"
#include "core/Constants.hpp"
#include "core/SceneManager.hpp"
#include <raylib.h>

namespace nundertale {

void InstructionsScene::onEnter(GameContext& /*context*/) {
}

void InstructionsScene::update(float /*dt*/, GameContext& context) {
    if (context.input.isConfirmPressed() || context.input.isCancelPressed() || context.input.isEscapePressed()) {
        context.audio.playSound("assets/sfx/selection_sound.wav", 0.7f);
        context.scenes.changeScene(std::make_unique<TitleScene>(), 0.3f);
    }
}

void InstructionsScene::render(GameContext& context) {
    ClearBackground(BLACK);

    Font font8Bit = context.resources.getFont8Bit(24);
    Font fontShmup = context.resources.getFontShmup(16);

    // Header (faithful to nundertale.c)
    context.resources.drawCenteredText(font8Bit, "-- Instructions --", 60, 32, WHITE, 2.0f);

    const char* controls[] = {
        "[Z or ENTER] - Select / Confirm / Use",
        "[X or SHIFT] - Esc / Cancel",
        "[C or CTRL]  - Inventory",
        "[ESC]        - Quit game",
        "When your HP is 0, you become a furry."
    };

    int startY = 140;
    for (int i = 0; i < 5; ++i) {
        Color c = (i == 4) ? COLOR_UNDERTALE_YELLOW : WHITE;
        context.resources.drawText(font8Bit, controls[i], { 110.0f, (float)(startY + i * 36) }, 22, c, 1.0f);
    }

    // Bottom subtitle
    const char* sub = "This is NOT UNDERTALE  - v_dev - Personal Indie Parody Project";
    context.resources.drawCenteredText(fontShmup, sub, 395, 16, COLOR_UNDERTALE_GRAY, 1.0f);

    // Return prompt
    context.resources.drawCenteredText(font8Bit, "[Press Z or ENTER to return]", 430, 18, COLOR_UNDERTALE_YELLOW, 1.0f);
}

void InstructionsScene::onExit(GameContext& /*context*/) {
}

} // namespace nundertale
