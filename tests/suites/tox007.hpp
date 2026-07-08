#pragma once

#include "core/Killstreaks.hpp"

#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <string>

inline void suite_tox007() {
    using namespace nundertale;

    GameState state;
    state.setFlag("toxic.track", 1);

    state.setFlag("toxic.kills", CARE_PACKAGE_THRESHOLD - 1);
    killstreaksOnKill(state);
    assert(!state.hasFlag(CARE_EARNED_FLAG));
    assert(!careDropPending(state));

    state.setFlag("toxic.kills", CARE_PACKAGE_THRESHOLD);
    killstreaksOnKill(state);
    assert(state.hasFlag(CARE_EARNED_FLAG));
    assert(!state.hasFlag(PREDATOR_EARNED_FLAG));
    assert(careDropPending(state));

    // A repeated notification at the same count does not recreate a consumed drop.
    markCareDropped(state);
    assert(!careDropPending(state));
    killstreaksOnKill(state);
    assert(!careDropPending(state));

    state.setFlag("toxic.kills", PREDATOR_MISSILE_THRESHOLD);
    killstreaksOnKill(state);
    assert(state.hasFlag(PREDATOR_EARNED_FLAG));
    assert(predatorAvailable(state));
    assert(usePredator(state));
    assert(!predatorAvailable(state));
    assert(!usePredator(state));

    state.setFlag("toxic.kills", TACTICAL_NUKE_THRESHOLD - 1);
    killstreaksOnKill(state);
    assert(!state.hasFlag(NUKE_EARNED_FLAG));
    state.setFlag("toxic.kills", TACTICAL_NUKE_THRESHOLD);
    killstreaksOnKill(state);
    assert(state.hasFlag(NUKE_EARNED_FLAG));

    // Rewards use cumulative kills rather than the save-reset streak.
    GameState cumulative;
    cumulative.setFlag("toxic.track", 1);
    cumulative.setFlag("toxic.kills", PREDATOR_MISSILE_THRESHOLD);
    cumulative.setFlag("toxic.streak", 1);
    killstreaksOnKill(cumulative);
    assert(cumulative.hasFlag(CARE_EARNED_FLAG));
    assert(cumulative.hasFlag(PREDATOR_EARNED_FLAG));

    // An ended or not-yet-started toxic track cannot award anything.
    GameState inactive;
    inactive.setFlag("toxic.kills", TACTICAL_NUKE_THRESHOLD);
    killstreaksOnKill(inactive);
    assert(!inactive.hasFlag(CARE_EARNED_FLAG));
    inactive.setFlag("toxic.track", 2);
    killstreaksOnKill(inactive);
    assert(!inactive.hasFlag(NUKE_EARNED_FLAG));

    assert(predatorDamage(100, 100) == 75);
    assert(predatorDamage(10, 100) == 9);
    assert(predatorDamage(1, 100) == 0);
    assert(predatorDamage(0, 100) == 0);
    assert(predatorDamage(std::numeric_limits<int>::max(),
                          std::numeric_limits<int>::max()) < std::numeric_limits<int>::max());

    constexpr std::array<const char*, 4> foodPool{
        "shark", "lobster", "crayons", "white_monster"
    };
    const std::string seededFood = carePackageFood(123456U);
    assert(seededFood == carePackageFood(123456U));
    bool foodInPool = false;
    for (const char* food : foodPool) {
        if (seededFood == food) foodInPool = true;
    }
    assert(foodInPool);
    for (std::uint32_t seed = 0;
         seed < static_cast<std::uint32_t>(foodPool.size()); ++seed) {
        assert(carePackageFood(seed) == foodPool[seed]);
    }

    GameState iconsState;
    iconsState.setFlag(CARE_EARNED_FLAG, 1);
    iconsState.setFlag(CARE_DROPPED_FLAG, 1);
    iconsState.setFlag(PREDATOR_EARNED_FLAG, 1);
    const auto icons = killstreakColumn(iconsState);
    assert(icons.size() == 3);
    assert(std::string(icons[0].name) == "Care Package");
    assert(icons[0].threshold == CARE_PACKAGE_THRESHOLD);
    assert(icons[0].earned && icons[0].used && !icons[0].locked);
    assert(std::string(icons[1].name) == "Predator Missile");
    assert(icons[1].threshold == PREDATOR_MISSILE_THRESHOLD);
    assert(icons[1].earned && !icons[1].used && !icons[1].locked);
    assert(std::string(icons[2].name) == "Tactical Nuke");
    assert(icons[2].threshold == TACTICAL_NUKE_THRESHOLD);
    assert(!icons[2].earned && !icons[2].used && icons[2].locked);

    std::cout << "[PASS] TOX-007: killstreak awards, consumables, damage cap, and icons verified."
              << std::endl;
}
