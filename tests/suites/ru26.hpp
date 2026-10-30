#pragma once

#include "core/SaveText.hpp"

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

inline void suite_ru26() {
    using namespace nundertale;

    assert(isFloofyFight("floofy"));
    assert(isFloofyFight("floofy_bass"));
    assert(isFloofyFight("floofy_lair"));
    assert(!isFloofyFight("barky"));
    assert(floofyWelcomeHomeLine("GRUB") ==
           "Floofy? ...Oh dear. Stay determined, GRUB. And stay away from his petals.");

    GameState defeated;
    defeated.player.name = "CHAD";
    defeated.player.isChad = true;
    defeated.player.hp = 0;
    defeated.furry.value = 77;
    defeated.currentRoom = "ruins_floofy";
    defeated.playerPos = { 411.0f, 222.0f };
    defeated.pendingCutscene = "r2_floofy_end";
    defeated.setFlag("ruins.floofy.scene", 1);
    defeated.setFlag("ruins.floofy.choice1", 2);
    defeated.setFlag("ruins.floofy.deaths", 2);

    GameState saved;
    saved.player.name = "CHAD";
    saved.player.isChad = true;
    saved.player.hp = 13;
    saved.player.maxHp = 24;
    saved.player.gold = 91;
    saved.furry.value = 25;
    saved.currentRoom = "ruins_panelhall";
    saved.playerPos = { 180.0f, 320.0f };
    saved.playerFacing = 2;
    saved.playTime = 123.5f;
    saved.setFlag("ruins.panelhall.saved", 1);
    saved.setFlag("ruins.floofy.scene", 1);
    saved.setFlag("ruins.floofy.deaths", 1);

    const std::vector<std::string> reArm = {
        "ruins.floofy.scene",
        ""
    };
    const GameState fromSave = buildWelcomeHomeRetryState(defeated, &saved, reArm, true);

    // The checkpoint wins exactly for room, spawn, stats, flags and FUR.
    assert(fromSave.currentRoom == "ruins_panelhall");
    assert(fromSave.playerPos.x == 180.0f && fromSave.playerPos.y == 320.0f);
    assert(fromSave.playerFacing == 2);
    assert(fromSave.player.hp == 13 && fromSave.player.maxHp == 24);
    assert(fromSave.player.gold == 91);
    assert(fromSave.furry.value == 25);
    assert(fromSave.playTime == 123.5f);
    assert(fromSave.getFlag("ruins.panelhall.saved") == 1);
    assert(!fromSave.hasFlag("ruins.floofy.scene"));
    assert(fromSave.getFlag("ruins.floofy.choice1", -1) == -1);
    assert(fromSave.getFlag("ruins.floofy.deaths") == 3);
    assert(saved.getFlag("ruins.floofy.scene") == 1); // pure helper: input unchanged

    // Applying the prepared retry metadata during Continue cannot double-count a death.
    GameState idempotent = fromSave;
    applyWelcomeHomeRetryState(idempotent, reArm, 3);
    assert(idempotent.getFlag("ruins.floofy.deaths") == 3);
    assert(!idempotent.hasFlag("ruins.floofy.scene"));

    // With no save, restart at a clean R1 while retaining the selected identity.
    const GameState clean = buildWelcomeHomeRetryState(defeated, nullptr, reArm, true);
    assert(clean.currentRoom == "ruins_fall");
    assert(clean.playerPos.x == 0.0f && clean.playerPos.y == 0.0f);
    assert(clean.player.name == "CHAD" && clean.player.isChad);
    assert(clean.player.hp == 20 && clean.player.maxHp == 20);
    assert(clean.furry.value == 0);
    assert(clean.pendingCutscene.empty());
    assert(!clean.hasFlag("ruins.floofy.scene"));
    assert(!clean.hasFlag("ruins.floofy.choice1"));
    assert(clean.getFlag("ruins.floofy.deaths") == 3);
    assert(clean.flags.size() == 1U);

    // A non-Floofy scripted retry can re-arm its once flag without inventing a death count.
    const GameState otherFight = buildWelcomeHomeRetryState(
        defeated, &saved, { "ruins.floofy.scene" }, false);
    assert(!otherFight.hasFlag("ruins.floofy.scene"));
    assert(otherFight.getFlag("ruins.floofy.deaths") == 1);

    std::cout << "[PASS] BUG-43: WELCOME HOME exact save reload, clean R1 restart, "
                 "script re-arm, Floofy death count and canonical line verified."
              << std::endl;
}
