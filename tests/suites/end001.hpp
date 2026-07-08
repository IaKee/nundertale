#pragma once

#include "core/Endings.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>

inline void suite_end001() {
    using namespace nundertale;

    // Enum values = LORE 28.
    assert(endingTypeValue(EndingType::None) == 0);
    assert(endingTypeValue(EndingType::Gamer) == 1);
    assert(endingTypeValue(EndingType::CleanRun) == 2);
    assert(endingTypeValue(EndingType::Toxic) == 3);
    assert(endingTypeValue(EndingType::Furry) == 4);
    assert(endingTypeFromValue(3) == EndingType::Toxic);
    assert(endingTypeFromValue(9) == EndingType::None);

    // Clean Run vs Gamer boundary.
    GameState clean;
    assert(decideEnding(clean) == EndingType::CleanRun);
    GameState oneKill;
    oneKill.killsTotal = 1;
    assert(decideEnding(oneKill) == EndingType::Gamer);
    GameState flagKill;
    flagKill.setFlag("kills.total", 1);
    assert(decideEnding(flagKill) == EndingType::Gamer);

    // Toxic: track running with kills.
    GameState toxic;
    toxic.killsTotal = 25;
    toxic.setFlag("toxic.track", 1);
    toxic.setFlag("toxic.kills", 25);
    assert(decideEnding(toxic) == EndingType::Toxic);

    // Track ended or never started: not Toxic.
    GameState ended = toxic;
    ended.setFlag("toxic.track", 2);
    assert(decideEnding(ended) == EndingType::Gamer);
    GameState notStarted = toxic;
    notStarted.setFlag("toxic.track", 0);
    assert(decideEnding(notStarted) == EndingType::Gamer);
    GameState noTrackKills = toxic;
    noTrackKills.setFlag("toxic.kills", 0);
    assert(decideEnding(noTrackKills) == EndingType::Gamer);

    // Furry beats Toxic, Gamer and Clean Run.
    GameState furry = toxic;
    furry.setFlag("route.furry", 1);
    assert(decideEnding(furry) == EndingType::Furry);
    GameState furryClean;
    furryClean.setFlag("route.furry", 1);
    assert(decideEnding(furryClean) == EndingType::Furry);
    GameState furryGamer = oneKill;
    furryGamer.setFlag("route.furry", 1);
    assert(decideEnding(furryGamer) == EndingType::Furry);

    // recordEnding.
    GameState rec;
    assert(recordedEnding(rec) == EndingType::None);
    recordEnding(rec, EndingType::CleanRun);
    assert(rec.getFlag("ending.type", -1) == 2);
    assert(recordedEnding(rec) == EndingType::CleanRun);
    recordEnding(rec, decideEnding(furry));
    assert(rec.getFlag("ending.type", -1) == 4);

    // Squad clock lines.
    const EndingType all[] = { EndingType::Gamer, EndingType::CleanRun, EndingType::Toxic, EndingType::Furry };
    const std::string speedA = "* Your squad asks how you got back so fast.";
    const std::string speedB = "* (Skill issue.)";
    for (EndingType type : all) {
        for (bool kicked : { false, true }) {
            const auto base = squadClockEndingLines(type, kicked, false);
            const auto fast = squadClockEndingLines(type, kicked, true);
            assert(!base.empty());
            assert(std::count(base.begin(), base.end(), speedA) == 0);
            if (kicked) {
                assert(fast == base);
            } else {
                assert(fast.size() == base.size() + 2);
                assert(std::count(fast.begin(), fast.end(), speedA) == 1);
                assert(std::count(fast.begin(), fast.end(), speedB) == 1);
                assert(fast[fast.size() - 2] == speedA && fast.back() == speedB);
            }
        }
    }
    assert(squadClockEndingLines(EndingType::None, false, true).empty());

    const auto gamerKicked = squadClockEndingLines(EndingType::Gamer, true, false);
    assert(gamerKicked.back() == "* (Whatever.)");
    const auto cleanKicked = squadClockEndingLines(EndingType::CleanRun, true, false);
    assert(cleanKicked.size() == 3);
    assert(cleanKicked[1] == "BARKY: new squad. two members. we're called the Shadow Fang Pack.");
    assert(cleanKicked[2] == "* (You're in a wolf squad now.)");
    const auto cleanOk = squadClockEndingLines(EndingType::CleanRun, false, false);
    assert(cleanOk.front() == "SQUAD: who's the wolf");
    assert(cleanOk.back() == "* (He gets 14 eliminations. You don't mention the zipper.)");

    // RaidClock overload: >10:00 left = speedrun; exactly 10:00 is not; expired = kicked.
    RaidClock fastClock;
    raidStart(fastClock, 601.0f);
    assert(squadClockEndingLines(EndingType::Gamer, fastClock).back() == speedB);
    RaidClock edgeClock;
    raidStart(edgeClock, 600.0f);
    assert(squadClockEndingLines(EndingType::Gamer, edgeClock) ==
           squadClockEndingLines(EndingType::Gamer, false, false));
    RaidClock deadClock;
    raidStart(deadClock, 10.0f);
    raidTick(deadClock, 11.0f, false);
    assert(squadClockEndingLines(EndingType::Gamer, deadClock) ==
           squadClockEndingLines(EndingType::Gamer, true, false));

    std::cout << "[PASS] END-001/END-006: ending router precedence, recordEnding and squad clock lines." << std::endl;
}
