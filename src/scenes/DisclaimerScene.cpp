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
    Color white = { 255, 255, 255, a };
    Color gray  = { 160, 160, 160, a };
    Color red   = { 255, 60, 60, a };

    // FRT-011: a few big lines a player will actually read. 8bitoperator only at 16/32/48 px,
    // the sizes where it lands on whole pixels (BUG-34).
    Font big   = context.resources.getFont8Bit(48);
    Font body  = context.resources.getFont8Bit(32);
    Font small = context.resources.getFont8Bit(16);

    context.resources.drawCenteredText(big, "WARNING", 56.0f, 48.0f, red, 1.0f);

    context.resources.drawCenteredText(body, "A fan-made PARODY of UNDERTALE.", 140.0f, 32.0f, white, 1.0f);
    context.resources.drawCenteredText(small, "Not affiliated with UNDERTALE or its creator.", 182.0f, 16.0f, gray, 1.0f);

    context.resources.drawCenteredText(body, "For ADULTS only (18+).", 236.0f, 32.0f, red, 1.0f);

    context.resources.drawCenteredText(body, "Crude internet humor about", 290.0f, 32.0f, white, 1.0f);
    context.resources.drawCenteredText(body, "gamers and furries.", 326.0f, 32.0f, white, 1.0f);
    context.resources.drawCenteredText(body, "Nothing explicit, lots of cringe.", 362.0f, 32.0f, white, 1.0f);

    if (static_cast<int>(m_timer * 2.5f) % 2 == 0) {
        context.resources.drawCenteredText(small, "[Press Z or ENTER]", 432.0f, 16.0f, Color{ COLOR_UNDERTALE_YELLOW.r, COLOR_UNDERTALE_YELLOW.g, COLOR_UNDERTALE_YELLOW.b, a }, 1.0f);
    }
}

void DisclaimerScene::onExit(GameContext& /*context*/) {
}

} // namespace nundertale
