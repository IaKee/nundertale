#pragma once

#include "combat/Enemy.hpp"
#include "combat/FloofyScript.hpp"
#include "core/GameContext.hpp"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>

inline void suite_ru18() {
    using namespace nundertale;

    const std::filesystem::path enemyPath =
        std::filesystem::temp_directory_path() / "nundertale_ru18_enemy.json";
    {
        std::ofstream out(enemyPath, std::ios::trunc);
        assert(out.good());
        out << R"({
            "id": "scripted_test",
            "name": "Scripted Test",
            "hp": 20,
            "atk": 1,
            "def": 1,
            "canSpare": false,
            "invulnerable": true,
            "scriptedEnd": {
                "afterEnemyTurns": 6,
                "interruptOnTurnStart": true,
                "endCutscene": "r2_floofy_end"
            },
            "turnOffsetFlag": "ruins.floofy.deaths",
            "fleeBlockedText": "* The petals block the exit.",
            "invulnerableTexts": [
                "* You hit him. Your fist sinks into the fursuit padding.",
                "* It's too soft to hurt."
            ],
            "invulnerableBubble": "Aww~ was that a hug?",
            "furCap": 99
        })";
    }

    const auto scripted = Enemy::loadFromJson(enemyPath.string());
    std::error_code removeError;
    std::filesystem::remove(enemyPath, removeError);
    assert(scripted != nullptr);
    assert(!scripted->canBeSpared());
    assert(scripted->isInvulnerable());
    assert(scripted->getScriptedEndAfterTurns() == 6);
    assert(scripted->getScriptedEnd().interruptOnTurnStart);
    assert(scripted->getScriptedEnd().endCutscene == "r2_floofy_end");
    assert(scripted->getTurnOffsetFlag() == "ruins.floofy.deaths");
    assert(scripted->getFleeBlockedText() == "* The petals block the exit.");
    assert(scripted->getInvulnerableTexts().size() == 2U);
    assert(scripted->getInvulnerableBubble() == "Aww~ was that a hug?");
    assert(scripted->getFurCap() == 99);

    assert(effectiveScriptedEndTurn(6, 0) == 6);
    assert(effectiveScriptedEndTurn(6, 1) == 6);
    assert(effectiveScriptedEndTurn(6, 2) == 6);
    assert(effectiveScriptedEndTurn(6, 3) == 5);
    assert(effectiveScriptedEndTurn(6, 99) == 3);
    assert(scriptedBattleEndsOnTurnStart(6, 6, true));
    assert(!scriptedBattleEndsOnTurnStart(5, 6, true));
    assert(!scriptedBattleEndsAfterTurn(6, 6, true));
    assert(scriptedBattleEndsAfterTurn(6, 6, false));

    const std::vector<MercyOption> noSpare = battleMercyOptions(false, false, true);
    assert(noSpare.size() == 1U && noSpare[0] == MercyOption::Flee);
    const std::vector<MercyOption> normal = battleMercyOptions(true, true, false);
    assert(normal.size() == 2U && normal[0] == MercyOption::Spare && normal[1] == MercyOption::Flee);

    assert(joinBattleText(scripted->getInvulnerableTexts()) ==
           "* You hit him. Your fist sinks into the fursuit padding.\n* It's too soft to hurt.");
    assert(battleFurDelta(90, 20, 99) == 9);
    assert(battleFurDelta(99, 20, 99) == 0);
    assert(battleFurDelta(90, 20, -1) == 20);

    GameState state;
    assert(state.pendingCutscene.empty());
    state.pendingCutscene = scripted->getScriptedEnd().endCutscene;
    assert(state.pendingCutscene == "r2_floofy_end");

    // Floofy's data uses the structured form and lets all six petal turns finish.
    const auto petalFloofy = Enemy::loadFromJson("data/enemies/floofy.json");
    assert(petalFloofy != nullptr);
    assert(petalFloofy->getScriptedEndAfterTurns() == 6);
    assert(!petalFloofy->getScriptedEnd().interruptOnTurnStart);
    assert(petalFloofy->getScriptedEnd().endCutscene == "r2_floofy_end");

    std::cout << "[PASS] RU18: scripted battle end, retry offset, no-Spare MERCY, blocked Flee, invulnerability text and FUR cap verified."
              << std::endl;
}
