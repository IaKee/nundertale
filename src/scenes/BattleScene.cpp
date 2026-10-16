#include "scenes/BattleScene.hpp"
#include "scenes/OverworldScene.hpp"
#include "scenes/GameOverScene.hpp"
#include "core/GameContext.hpp"
#include "core/Constants.hpp"
#include "core/SceneManager.hpp"
#include <raylib.h>

namespace nundertale {

BattleScene::BattleScene() {
    auto barky = std::make_unique<Enemy>("Barky", 30, 4, 2);
    barky->setActOptions({ "Check", "Cringe Check", "Offer Mountain Dew" });
    barky->setDialogues({ "Awoo! You can't out-fur me!", "Is that... Mountain Dew?!", "Notice me senpai!", "*Aggressive tail wagging*" });
    m_battleSystem = std::make_unique<BattleSystem>(std::move(barky));
}

void BattleScene::onEnter(GameContext& context) {
    context.audio.playMusic("assets/music/epic_background_loop.ogg", true);
}

void BattleScene::update(float dt, GameContext& context) {
    if (m_battleSystem) {
        m_battleSystem->update(dt, context);

        // BND-010 & BUG-03: Transition directly to authentic GameOverScene
        if (m_battleSystem->getState() == BattleState::Defeat) {
            Vector2 deathPos = m_battleSystem->getSoulPosition();
            context.scenes.changeScene(std::make_unique<GameOverScene>(deathPos), 0.0f);
            return;
        }

        if (m_battleSystem->isFinished()) {
            if (context.input.isConfirmPressed()) {
                context.scenes.popScene(context);
            }
        }
    }
}

void BattleScene::render(GameContext& context) {
    ClearBackground(BLACK);

    if (m_battleSystem) {
        m_battleSystem->render(context);

        if (m_battleSystem->isFinished()) {
            Font font8Bit = context.resources.getFont8Bit(20);
            context.resources.drawCenteredText(font8Bit, "[Press Z or ENTER to continue]", 400, 20, COLOR_UNDERTALE_YELLOW, 1.0f);
        }
    }
}

void BattleScene::onExit(GameContext& context) {
}

} // namespace nundertale
