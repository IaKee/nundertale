#include "scenes/BattleScene.hpp"
#include "scenes/OverworldScene.hpp"
#include "scenes/GameOverScene.hpp"
#include "core/GameContext.hpp"
#include "core/Constants.hpp"
#include "core/SceneManager.hpp"
#include "services/MusicTracks.hpp"
#include "core/TiledMap.hpp"
#include <raylib.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cctype>

namespace nundertale {

static bool safeEncounterId(const std::string& id) {
    if (id.empty()) return false;
    for (char ch : id) {
        if (std::isalnum(static_cast<unsigned char>(ch)) == 0 && ch != '_') return false;
    }
    return true;
}

BattleScene::BattleScene(std::string encounterId)
    : m_encounterId(std::move(encounterId)) {
    if (!safeEncounterId(m_encounterId)) {
        std::clog << "Invalid encounter id '" << m_encounterId << "'; battle not started.\n";
        return;
    }

    std::ifstream encFile("data/encounters/" + m_encounterId + ".json");
    if (!encFile) {
        std::clog << "Unknown encounter id '" << m_encounterId << "'; battle not started.\n";
        return;
    }

    std::stringstream ss;
    ss << encFile.rdbuf();
    tiled_detail::JsonValue root;
    if (!tiled_detail::JsonParser(ss.str()).parse(root) ||
        root.kind != tiled_detail::JsonValue::Kind::Object) {
        std::clog << "Invalid encounter data for '" << m_encounterId << "'; battle not started.\n";
        return;
    }

    const tiled_detail::JsonValue* enemies = root.field("enemies");
    if (enemies == nullptr || enemies->kind != tiled_detail::JsonValue::Kind::Array ||
        enemies->array.empty() || enemies->array[0].kind != tiled_detail::JsonValue::Kind::String) {
        std::clog << "Encounter '" << m_encounterId << "' has no supported enemy; battle not started.\n";
        return;
    }

    const std::string enemyId = enemies->array[0].string;
    if (!safeEncounterId(enemyId) || !std::filesystem::exists("data/enemies/" + enemyId + ".json")) {
        std::clog << "Encounter '" << m_encounterId << "' references unknown enemy '"
                  << enemyId << "'; battle not started.\n";
        return;
    }

    std::string introText = "* " + enemyId + " blocked your path!";
    if (const auto* value = root.field("introText"); value != nullptr &&
        value->kind == tiled_detail::JsonValue::Kind::String) introText = value->string;
    if (const auto* value = root.field("background"); value != nullptr &&
        value->kind == tiled_detail::JsonValue::Kind::String) m_backgroundId = value->string;
    if (const auto* value = root.field("music"); value != nullptr &&
        value->kind == tiled_detail::JsonValue::Kind::String) m_musicId = value->string;
    bool canFlee = false;
    if (const auto* value = root.field("canFlee"); value != nullptr &&
        value->kind == tiled_detail::JsonValue::Kind::Boolean) canFlee = value->boolean;

    auto enemy = Enemy::loadFromJson("data/enemies/" + enemyId + ".json");
    m_battleSystem = std::make_unique<BattleSystem>(std::move(enemy), introText, canFlee);
    if (!m_backgroundId.empty()) m_battleSystem->setBackground(backgroundFromId(m_backgroundId));
    m_valid = true;
}

void BattleScene::onEnter(GameContext& context) {
    if (!m_valid || !m_battleSystem) return;
    const std::string musicContext = m_musicId.rfind("battle.", 0) == 0
        ? m_musicId
        : "battle." + m_musicId;
    const MusicEntry track = musicFor(
        musicContext,
        [](const std::string& path) { return std::filesystem::exists(path); });
    context.audio.playMusic(track.path, true, track.loopStart, track.loopEnd);
}

void BattleScene::update(float dt, GameContext& context) {
    if (m_battleSystem) {
        m_battleSystem->update(dt, context);

        // BND-010 & BUG-03: Transition directly to authentic GameOverScene
        if (m_battleSystem->getState() == BattleState::Defeat) {
            Vector2 deathPos = m_battleSystem->getSoulPosition();
            context.scenes.changeScene(std::make_unique<GameOverScene>(deathPos, m_encounterId), 0.0f);
            return;
        }

        // AUD-004: the battle music stops the moment YOU WON shows
        if (m_battleSystem->isVictory() && !m_victoryMusicStopped) {
            context.audio.stopMusic();
            m_victoryMusicStopped = true;
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
