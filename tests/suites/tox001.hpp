#pragma once

#include "core/ToxicTrack.hpp"

#include <cassert>
#include <iostream>

inline void suite_tox001() {
    using namespace nundertale;

    assert(toxicCorruptionStage(-1) == 0);
    assert(toxicCorruptionStage(0) == 0);
    assert(toxicCorruptionStage(1) == 1);
    assert(toxicCorruptionStage(8) == 1);
    assert(toxicCorruptionStage(9) == 2);
    assert(toxicCorruptionStage(15) == 2);
    assert(toxicCorruptionStage(16) == 3);
    assert(toxicCorruptionStage(21) == 3);
    assert(toxicCorruptionStage(22) == 4);
    assert(toxicCorruptionStage(25) == 4);

    GameState state;
    assert(toxicKills(state) == 0);
    assert(toxicStreak(state) == 0);
    assert(!toxicTrackOnKill(state, "fursuit_dummy"));
    assert(!toxicTrackOnKill(state, "gladowos"));
    assert(toxicTrack(state) == 0);
    assert(toxicKills(state) == 0);
    assert(toxicStreak(state) == 0);

    assert(toxicTrackOnKill(state, "froggsona"));
    assert(toxicTrackOnKill(state, "shysona"));
    assert(toxicTrack(state) == 1);
    assert(toxicKills(state) == 2);
    assert(toxicStreak(state) == 2);

    toxicOnSave(state);
    assert(toxicStreak(state) == 0);
    assert(toxicKills(state) == 2);
    assert(toxicTrackOnKill(state, "modsmal"));
    assert(toxicKills(state) == 3);
    assert(toxicStreak(state) == 1);

    const ToxicHudState activeHud = toxicHudState(state);
    assert(activeHud.kills == toxicKills(state));
    assert(activeHud.streak == toxicStreak(state));
    assert(activeHud.stage == toxicCorruptionStage(state));
    assert(activeHud.active);

    assert(toxicTrackOnSpare(state, "froggsona"));
    const int frozenKills = toxicKills(state);
    const int frozenStreak = toxicStreak(state);
    assert(!toxicTrackOnKill(state, "lurkeye"));
    assert(!toxicTrackOnKill(state, "fursuit_dummy"));
    assert(toxicKills(state) == frozenKills);
    assert(toxicStreak(state) == frozenStreak);

    const ToxicHudState endedHud = toxicHudState(state);
    assert(endedHud.kills == frozenKills);
    assert(endedHud.streak == frozenStreak);
    assert(endedHud.stage == toxicCorruptionStage(frozenKills));
    assert(!endedHud.active);

    std::cout << "[PASS] TOX-001: kill count, streak, save reset, and corruption stages verified."
              << std::endl;
}
