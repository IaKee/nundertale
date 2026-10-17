#include "scenes/DisclaimerScene.hpp"
#include "scenes/LoreScene.hpp"
#include "core/GameContext.hpp"
#include "core/Constants.hpp"
#include "core/SceneManager.hpp"
#include <raylib.h>

namespace nundertale {

void DisclaimerScene::onEnter(GameContext& context) {
    m_timer = 0.0f;
    m_alpha = 0.0f;
    m_fadingIn = true;
    context.audio.playSound("assets/sfx/pipipipopopo_backwards.ogg", 0.5f);
}

void DisclaimerScene::update(float dt, GameContext& context) {
    m_timer += dt;

    if (m_fadingIn) {
        m_alpha += dt * 1.5f;
        if (m_alpha >= 1.0f) {
            m_alpha = 1.0f;
            m_fadingIn = false;
        }
    }

    // Skip on confirm key or after 12 seconds
    if (context.input.isConfirmPressed() || m_timer >= 12.0f) {
        context.audio.playSound("assets/sfx/menu_select.wav", 0.7f);
        context.scenes.changeScene(std::make_unique<LoreScene>(), 0.5f);
    }
}

void DisclaimerScene::render(GameContext& context) {
    ClearBackground(BLACK);

    unsigned char a = static_cast<unsigned char>(m_alpha * 255.0f);
    Color textColor = { 255, 255, 255, a };
    Color redColor   = { 255, 60, 60, a };
    Color dimColor   = { 220, 220, 220, a };

    Font fontTitle = context.resources.getFontTitle(44);
    Font fontShmup = context.resources.getFontShmup(20);
    Font font8Bit  = context.resources.getFont8Bit(18);

    // Title: Not Undertale in monster_friend font
    context.resources.drawCenteredText(fontTitle, "Not Undertale", 40, 44, textColor, 2.0f);

    // Warning
    context.resources.drawCenteredText(fontShmup, "Trigger Warning", 100, 30, redColor, 1.0f);
    context.resources.drawCenteredText(fontShmup, "This is", 135, 22, redColor, 1.0f);

    // Disclaimer Body (Faithful to original C implementation, strictly personal project reference)
    const char* lines[] = {
        "- This game may and probably will be offensive to some people -",
        "",
        "It was made this way to be a comical parody of the UNDERTALE title,",
        "however it is not supposed to be associated with the original game",
        "or creator, as we don't want to deprecate their public image.",
        "",
        "This piece of content was developed solely as an indie passion project",
        "and personal comedy experiment in C++ game architecture.",
        "",
        "Enjoy the journey down Mt. Dew with an open mind and a sense of humor."
    };

    int startY = 175;
    for (int i = 0; i < 10; ++i) {
        if (lines[i][0] != '\0') {
            context.resources.drawCenteredText(fontShmup, lines[i], static_cast<float>(startY + i * 20), 14, dimColor, 1.0f);
        }
    }

    // Prompt
    if ((int)(m_timer * 2.5f) % 2 == 0) {
        context.resources.drawCenteredText(font8Bit, "[Press Z or ENTER to continue]", 425, 18, COLOR_UNDERTALE_YELLOW, 1.0f);
    }
}

void DisclaimerScene::onExit(GameContext& /*context*/) {
}

} // namespace nundertale
