#pragma once

#include "combat/Enemy.hpp"
#include "combat/GladosPatterns.hpp"

#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
#include <set>
#include <string>

inline void suite_ru12() {
    using namespace nundertale;

    auto gladowos = Enemy::loadFromJson("data/enemies/gladowos.json");
    assert(gladowos != nullptr);
    assert(gladowos->getId() == "gladowos");
    assert(gladowos->getMaxHp() == 9999 && gladowos->getAtk() == 9 && gladowos->getDef() == 99);
    assert(gladowos->getSurviveTurns() == 10);
    assert(gladowos->isUnkillable());
    assert(!gladowos->countsAsKill());
    assert(!gladowos->canBeSpared());
    assert(gladowos->getSurvivalEndText() == "* Test complete. You get no cake.");

    gladowos->takeDamage(20000);
    assert(gladowos->getHp() == 1 && !gladowos->isDead());
    assert(!gladosSurvivalComplete(9, gladowos->getSurviveTurns()));
    assert(gladosSurvivalComplete(10, gladowos->getSurviveTurns()));
    assert(gladosSurvivalComplete(11, gladowos->getSurviveTurns()));

    const auto& phases = gladowos->getSurvivalPhases();
    assert(phases.size() == 3);
    assert(phases[0].id == "calibration" && phases[0].startTurn == 1 && phases[0].endTurn == 3);
    assert(phases[1].id == "exposure" && phases[1].startTurn == 4 && phases[1].endTurn == 7);
    assert(phases[2].id == "incineration" && phases[2].startTurn == 8 && phases[2].endTurn == 10);
    for (int turn = 1; turn <= 10; ++turn) {
        assert(gladosPhaseForTurn(phases, turn) != nullptr);
        assert(!gladosPatternForTurn(phases, turn).empty());
    }
    assert(gladosPatternForTurn(phases, 1) == "turret_sweep");
    assert(gladosPatternForTurn(phases, 2) == "cube_drop");
    assert(gladosPatternForTurn(phases, 4) == "musk_neurotoxin");
    assert(gladosPatternForTurn(phases, 5) == "portal_bullets");
    assert(gladosPatternForTurn(phases, 10) == "incinerator_floor");

    std::set<std::string> patternIds;
    for (const EnemyPatternDef& pattern : gladowos->getPatterns()) patternIds.insert(pattern.id);
    assert(patternIds == std::set<std::string>({
        "cube_drop", "incinerator_floor", "musk_neurotoxin", "portal_bullets", "turret_sweep"
    }));
    for (int turn = 1; turn <= 10; ++turn) {
        assert(patternIds.count(gladosPatternForTurn(phases, turn)) == 1);
    }

    const GladosPortalTransit throughBlue = gladosPortalTransit(394.0f, 68.0f, 237.0f, 402.0f);
    assert(throughBlue.teleported && throughBlue.x == 245.0f);
    const GladosPortalTransit throughOrange = gladosPortalTransit(245.0f, -68.0f, 237.0f, 402.0f);
    assert(throughOrange.teleported && throughOrange.x == 394.0f);
    assert(!gladosPortalTransit(320.0f, 68.0f, 237.0f, 402.0f).teleported);

    assert(gladowos->getActs().size() == 4);
    assert(gladowos->getActs()[1].label == "Ask About the Cake");
    assert(gladowos->getActs()[2].label == "Compliment the Booth");
    assert(gladowos->getActs()[3].label == "Hug Barky");
    assert(gladowos->getActHeal("hug_barky") == 5);

    std::ifstream encounterFile("data/encounters/gladowos.json");
    assert(encounterFile.good());
    const std::string encounter((std::istreambuf_iterator<char>(encounterFile)),
                                std::istreambuf_iterator<char>());
    assert(encounter.find("\"gladowos\"") != std::string::npos);
    assert(encounter.find("\"music\": \"battle.default\"") != std::string::npos);

    std::cout << "[PASS] C1-025 (ru12): GLaDOwOS data, 3-phase/10-turn schedule, five patterns, "
                 "portal transit, unkillable/no-kill semantics and ACTs verified." << std::endl;
}
