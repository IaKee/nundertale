#include "scenes/BattleScene.hpp"
#include "scenes/OverworldScene.hpp"
#include "scenes/GameOverScene.hpp"
#include "core/GameContext.hpp"
#include "core/Constants.hpp"
#include "core/SceneManager.hpp"
#include "services/MusicTracks.hpp"
#include <raylib.h>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace nundertale {

static std::string extractJsonString(const std::string& json, const std::string& key) {
    std::string needle = "\"" + key + "\":";
    size_t pos = json.find(needle);
    if (pos == std::string::npos) return "";
    pos += needle.size();
    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t' || json[pos] == '\n' || json[pos] == '\r')) pos++;
    if (pos >= json.size() || json[pos] != '\"') return "";
    pos++;
    size_t end = json.find('\"', pos);
    if (end == std::string::npos) return "";
    return json.substr(pos, end - pos);
}

BattleScene::BattleScene() {
    // BFL-005 & BFL-006: Data-driven enemy loading with fallback
    auto enemy = Enemy::loadFromJson("data/enemies/barky.json");
    m_battleSystem = std::make_unique<BattleSystem>(std::move(enemy));
}

void BattleScene::onEnter(GameContext& context) {
    std::string musicId;

    // BUI-013 & BFL-007: Load encounter presentation settings in one read.
    std::ifstream encFile("data/encounters/barky.json");
    if (encFile.is_open()) {
        std::stringstream ss;
        ss << encFile.rdbuf();
        const std::string encounterJson = ss.str();
        const std::string bgId = extractJsonString(encounterJson, "background");
        musicId = extractJsonString(encounterJson, "music");
        if (!bgId.empty()) {
            m_battleSystem->setBackground(backgroundFromId(bgId));
        }
    }

    const std::string musicPath = resolveMusicPath(
        musicId,
        [](const std::string& path) { return std::filesystem::exists(path); });
    context.audio.playMusic(musicPath, true);
}

void BattleScene::update(float dt, GameContext& context) {
    if (m_battleSystem) {
        m_battleSystem->update(dt, context);

        // BND-010 & BUG-03: Transition directly to authentic GameOverScene
        if (m_battleSystem->getState() == BattleState::Defeat) {
            Vector2 deathPos = m_battleSystem->getSoulPosition();
            context.scenes.changeScene(std::make_unique<GameOverScene>(deathPos, "barky"), 0.0f);
            return;
        }

        if (m_battleSystem->isFinished()) {
            if (m_battleSystem->isFled()) {
                context.scenes.popScene(context);
                return;
            }
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

        if (m_battleSystem->isFinished() && !m_battleSystem->isFled()) {
            Font font8Bit = context.resources.getFont8Bit(20);
            context.resources.drawCenteredText(font8Bit, "[Press Z or ENTER to continue]", 400, 20, COLOR_UNDERTALE_YELLOW, 1.0f);
        }
    }
}

void BattleScene::onExit(GameContext& /*context*/) {
}

} // namespace nundertale
