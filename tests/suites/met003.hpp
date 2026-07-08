#pragma once

#include "core/MetaLines.hpp"
#include "core/SessionMeta.hpp"

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

inline void suite_met003() {
    static bool alreadyRan = false;
    if (alreadyRan) {
        return;
    }
    alreadyRan = true;

    using namespace nundertale;

    // --- MET-002: reloadLines tests ---
    {
        // 1. Fresh session: no reload lines
        SessionMeta fresh{};
        assert(reloadLines(fresh, false).empty());
        assert(reloadLines(fresh, true).empty());

        // 2. Floofy not present: no reload lines regardless of flags
        SessionMeta furryReload{};
        furryReload.reloadedAfterFurryOver = true;
        assert(reloadLines(furryReload, false).empty());

        SessionMeta killReload{};
        killReload.reloadedAfterKill = true;
        assert(reloadLines(killReload, false).empty());

        // 3. Post WELCOME HOME (FURRY OVER) reload with Floofy present
        const std::vector<std::string> furryLines = reloadLines(furryReload, true);
        assert(furryLines.size() == 1);
        assert(furryLines[0] == "Did you just reload? I saw that. No shame, though. I'd do it too.");

        // 4. Kill and reload with Floofy present
        const std::vector<std::string> killLines = reloadLines(killReload, true);
        assert(killLines.size() == 1);
        assert(killLines[0] == "You killed them and loaded your save. Deleting the evidence? Gamer behavior.");

        // 5. Precedence: WELCOME HOME reload takes precedence over kill-reload
        SessionMeta bothReload{};
        bothReload.reloadedAfterFurryOver = true;
        bothReload.reloadedAfterKill = true;
        const std::vector<std::string> precedenceLines = reloadLines(bothReload, true);
        assert(precedenceLines.size() == 1);
        assert(precedenceLines[0] == "Did you just reload? I saw that. No shame, though. I'd do it too.");

        // 6. SessionMeta state transitions via helper functions
        SessionMeta sessionFlow{};
        sessionOnKill(sessionFlow);
        assert(sessionFlow.killsSinceSave == 1);
        // Load without furry over -> reloadedAfterKill = true
        sessionOnLoad(sessionFlow, false);
        assert(sessionFlow.reloadedAfterKill);
        assert(!sessionFlow.reloadedAfterFurryOver);
        assert(reloadLines(sessionFlow, true) == killLines);

        // Save resets kill count
        sessionOnKill(sessionFlow);
        sessionOnSave(sessionFlow);
        assert(sessionFlow.killsSinceSave == 0);
        sessionOnLoad(sessionFlow, true);
        assert(!sessionFlow.reloadedAfterKill);
        assert(sessionFlow.reloadedAfterFurryOver);
        assert(reloadLines(sessionFlow, true) == furryLines);
    }

    // --- MET-003: resetLines tests ---
    {
        // 1. Fresh session (ending 0, resets 0): no speaker delivers a reset line
        assert(resetLines(Speaker::SansS1, 0, 0).empty());
        assert(resetLines(Speaker::FloofyR2, 0, 0).empty());
        assert(resetLines(Speaker::FuriellR3, 0, 0).empty());
        assert(resetLines(Speaker::SansGrillbys, 0, 0).empty());

        // 2. Sans S1: fires after any ending (1: Gamer, 2: Clean Run, 3: Toxic, 4: Furry)
        const std::string sansS1Line = "heh. you look like you've seen this episode before.";
        for (int ending = 1; ending <= 4; ++ending) {
            const std::vector<std::string> lines = resetLines(Speaker::SansS1, ending, 0);
            assert(lines.size() == 1);
            assert(lines[0] == sansS1Line);
        }
        // Boundaries and unknown endings for Sans S1
        assert(resetLines(Speaker::SansS1, 0, 0).empty());
        assert(resetLines(Speaker::SansS1, -1, 0).empty());
        assert(resetLines(Speaker::SansS1, 5, 0).empty());
        assert(resetLines(Speaker::SansS1, 99, 0).empty());

        // 3. Floofy R2: fires only after Toxic ending (3)
        const std::string floofyR2Line =
            "You again? You know what you did. I have screenshots. I have NO shame about showing them.";
        assert(resetLines(Speaker::FloofyR2, 0, 0).empty());
        assert(resetLines(Speaker::FloofyR2, 1, 0).empty());
        assert(resetLines(Speaker::FloofyR2, 2, 0).empty());
        assert(resetLines(Speaker::FloofyR2, 4, 0).empty());
        assert(resetLines(Speaker::FloofyR2, -1, 0).empty());
        assert(resetLines(Speaker::FloofyR2, 5, 0).empty());
        const std::vector<std::string> floofyToxic = resetLines(Speaker::FloofyR2, 3, 0);
        assert(floofyToxic.size() == 1);
        assert(floofyToxic[0] == floofyR2Line);

        // 4. Furiell R3: fires only after Furry ending (4)
        const std::string furiellR3Line = "...Have we met? You smell familiar.";
        assert(resetLines(Speaker::FuriellR3, 0, 0).empty());
        assert(resetLines(Speaker::FuriellR3, 1, 0).empty());
        assert(resetLines(Speaker::FuriellR3, 2, 0).empty());
        assert(resetLines(Speaker::FuriellR3, 3, 0).empty());
        assert(resetLines(Speaker::FuriellR3, -1, 0).empty());
        assert(resetLines(Speaker::FuriellR3, 5, 0).empty());
        const std::vector<std::string> furiellFurry = resetLines(Speaker::FuriellR3, 4, 0);
        assert(furiellFurry.size() == 1);
        assert(furiellFurry[0] == furiellR3Line);

        // 5. Sans at Grillby's: boundary resets 2 vs 3 (resets >= 3)
        const std::string sansGrillbysLine =
            "you ever feel stuck in a loop? like a gacha banner that never ends?";
        assert(resetLines(Speaker::SansGrillbys, 0, -1).empty());
        assert(resetLines(Speaker::SansGrillbys, 0, 0).empty());
        assert(resetLines(Speaker::SansGrillbys, 0, 1).empty());
        assert(resetLines(Speaker::SansGrillbys, 0, 2).empty());

        const std::vector<std::string> grillbysAt3 = resetLines(Speaker::SansGrillbys, 0, 3);
        assert(grillbysAt3.size() == 1);
        assert(grillbysAt3[0] == sansGrillbysLine);

        const std::vector<std::string> grillbysAt4 = resetLines(Speaker::SansGrillbys, 0, 4);
        assert(grillbysAt4.size() == 1);
        assert(grillbysAt4[0] == sansGrillbysLine);

        const std::vector<std::string> grillbysAt10 = resetLines(Speaker::SansGrillbys, 3, 10);
        assert(grillbysAt10.size() == 1);
        assert(grillbysAt10[0] == sansGrillbysLine);

        // 6. MetaSpeaker alias compatibility
        assert(resetLines(MetaSpeaker::SansS1, 1, 0).size() == 1);
        assert(resetLines(MetaSpeaker::SansS1, 1, 0)[0] == sansS1Line);
    }

    std::cout << "[PASS] MET-003: reload lines and reset lines by last ending verified." << std::endl;
}

namespace {
struct Met003Runner {
    Met003Runner() {
        suite_met003();
    }
};
[[maybe_unused]] const Met003Runner s_met003Runner;
}
