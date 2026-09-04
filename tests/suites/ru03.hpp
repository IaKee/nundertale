#pragma once

#include "combat/FloofyScript.hpp"
#include "core/Cutscene.hpp"
#include "core/TiledMap.hpp"

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace ru03_detail {

inline std::string readFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

inline bool hasFlagStep(const nundertale::Cutscene& cutscene,
                        const std::string& key,
                        int value) {
    for (const nundertale::CutsceneStep& step : cutscene.steps) {
        if (step.type == nundertale::CutsceneStepType::SetFlag &&
            step.key == key && step.value == value) return true;
    }
    return false;
}

} // namespace ru03_detail

inline void suite_ru03() {
    using namespace nundertale;

    const auto floofy = Enemy::loadFromJson("data/enemies/floofy.json");
    assert(floofy != nullptr);
    assert(floofy->getId() == "floofy");
    assert(floofy->isInvulnerable());
    assert(!floofy->canBeSpared());
    assert(floofy->getScriptedEndAfterTurns() == 6);
    assert(!floofy->getScriptedEnd().interruptOnTurnStart);
    assert(floofy->getScriptedEnd().endCutscene == "r2_floofy_end");
    assert(floofy->getFurCap() == 99);
    assert(floofy->suppressesFurMirror());

    const int startingHp = floofy->getHp();
    floofy->takeDamage(999);
    floofy->addMercy(100);
    assert(floofy->getHp() == startingHp && !floofy->isDead());
    assert(!floofy->isSpareable());

    const auto& patterns = floofy->getPatterns();
    assert(patterns.size() == 6U);
    assert(patterns[0].id == "friendship_petals");
    assert(patterns[0].kind == EnemyPatternKind::FriendshipPetals);
    assert(patterns[0].furry == 8);
    assert(floofy->getPatternFur(patterns[0].id, -1) == 8);
    assert(floofy->getPatternSprite(patterns[0].id) == "assets/sprites/bullets/petal.png");
    assert(patterns[5].id == "pounce_ring");
    assert(patterns[5].kind == EnemyPatternKind::PounceRing);
    assert(patterns[5].furry == 8);
    assert(floofy->getPatternSpeech(patterns[5].id) == "Hold STILL~");
    for (std::size_t i = 0; i < patterns.size(); ++i) {
        assert(patterns[i].furry == 8);
        assert(patterns[i].damage == (i < 4U ? 3 : 4));
        assert(scriptedPatternIndex(static_cast<int>(i + 1U), patterns.size()) == i);
    }

    assert(scriptedPatternIndex(0, patterns.size()) == 0U);
    assert(scriptedPatternIndex(1, patterns.size()) == 0U);
    assert(scriptedPatternIndex(2, patterns.size()) == 1U);
    assert(scriptedPatternIndex(6, patterns.size()) == 5U);
    assert(scriptedPatternIndex(99, patterns.size()) == 5U);
    assert(!scriptedBattleEndsAfterTurn(1, floofy->getScriptedEndAfterTurns()));
    assert(!scriptedBattleEndsAfterTurn(5, floofy->getScriptedEndAfterTurns()));
    assert(scriptedBattleEndsAfterTurn(6, floofy->getScriptedEndAfterTurns()));
    assert(!scriptedBattleEndsAfterTurn(100, 0));

    tiled_detail::JsonValue encounter;
    const std::string encounterText = ru03_detail::readFile("data/encounters/floofy.json");
    assert(!encounterText.empty());
    assert(tiled_detail::JsonParser(encounterText).parse(encounter));
    const tiled_detail::JsonValue* enemies = encounter.field("enemies");
    assert(enemies != nullptr && enemies->kind == tiled_detail::JsonValue::Kind::Array);
    assert(enemies->array.size() == 1U && enemies->array[0].string == "floofy");
    const tiled_detail::JsonValue* canFlee = encounter.field("canFlee");
    assert(canFlee != nullptr && canFlee->kind == tiled_detail::JsonValue::Kind::Boolean);
    assert(!canFlee->boolean);

    Cutscene battleIntro;
    assert(parseCutsceneJson(ru03_detail::readFile("data/cutscenes/r2_floofy_battle.json"), battleIntro));
    assert(battleIntro.steps.size() == 2U);
    assert(battleIntro.steps.back().type == CutsceneStepType::StartBattle);
    assert(battleIntro.steps.back().encounter == "floofy");

    Cutscene rescue;
    assert(parseCutsceneJson(ru03_detail::readFile("data/cutscenes/r2_floofy_rescue.json"), rescue));
    bool glitter = false;
    bool heal = false;
    bool furReset = false;
    for (const CutsceneStep& step : rescue.steps) {
        if (step.type == CutsceneStepType::Sound &&
            step.path == "assets/sfx/glitter_spray.ogg") glitter = true;
        if (step.type == CutsceneStepType::HealFull) heal = true;
        if (step.type == CutsceneStepType::SetFur && step.value == 0) furReset = true;
    }
    assert(glitter && heal && furReset);
    assert(ru03_detail::hasFlagStep(rescue, "ruins.floofy.met", 1));
    assert(ru03_detail::hasFlagStep(rescue, "fur_mirror_pending", 0));
    assert(!ru03_detail::hasFlagStep(rescue, "battle.floofy.killed", 1));
    assert(!ru03_detail::hasFlagStep(rescue, "battle.floofy.spared", 1));
    assert(!ru03_detail::hasFlagStep(rescue, "battle.floofy.fled", 1));

    std::cout << "[PASS] C1-027: Floofy six-turn petal fight, +8 FUR hits, scripted pounce end, invulnerability and encounter verified."
              << std::endl;
}
