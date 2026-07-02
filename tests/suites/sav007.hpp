#pragma once

#include <cassert>
#include <iostream>
#include <string>

#include "core/GameContext.hpp"
#include "core/SaveText.hpp"

inline void suite_sav007()
{
    using namespace nundertale;

    const std::string suffix = "fills you with DETERMINATION.";
    const std::string suffixPlural = "fill you with DETERMINATION.";
    // Every save room from LORE §8.8 (R1, R3, R13, Snowdinha, S6, W1, W6, H3, N3, X2).
    const char* rooms[] = { "ruins_fall", "ruins_entrance", "ruins_hall", "ruins_exit", "snowdin_entrance",
                            "snowdin_town", "snowdin_fog", "waterfall_entrance", "waterfall_arena",
                            "hotland_quiz", "newhome_throne", "con_center_hall" };
    for (const char* room : rooms) {
        const std::string line = savePlaceLine(room);
        assert(line.rfind("* The ", 0) == 0);
        const bool ok = line.size() > suffix.size() &&
            (line.compare(line.size() - suffix.size(), suffix.size(), suffix) == 0 ||
             line.compare(line.size() - suffixPlural.size(), suffixPlural.size(), suffixPlural) == 0);
        assert(ok);
    }
    // Exact LORE wording for a couple of them.
    assert(savePlaceLine("ruins_exit") == "* The cold air and the faint smell of ozone fill you with DETERMINATION.");
    assert(savePlaceLine("con_center_hall") == "* The terrified hugging of a hundred furries fills you with DETERMINATION.");
    // Unknown and empty rooms fall back to R1's line.
    assert(savePlaceLine("nowhere") == savePlaceLine("ruins_fall"));
    assert(savePlaceLine("") == savePlaceLine("ruins_fall"));

    // Normal route: heals HP, never touches FUR, FUR 0 -> only the place line.
    GameState s;
    s.currentRoom = "snowdin_entrance";
    s.player.hp = 3;
    s.player.maxHp = 20;
    SaveHealResult r = saveHealResult(s);
    assert(!r.furryRoute && r.hpAfter == 20 && r.furAfter == 0);
    assert(r.lines.size() == 1 && r.lines[0] == savePlaceLine("snowdin_entrance"));

    // FUR > 0: same heal, FUR unchanged, extra line after the place line.
    s.furry.value = 57;
    r = saveHealResult(s);
    assert(r.hpAfter == 20 && r.furAfter == 57 && s.furry.value == 57);
    assert(r.lines.size() == 2 && r.lines[1] == "* DETERMINATION can't fix fluff.");
    s.furry.value = 1;
    assert(saveHealResult(s).lines.size() == 2);
    assert(saveHealResult(s).furAfter == 1);
    s.furry.value = 100;
    assert(saveHealResult(s).furAfter == 100);

    // Override place line (Tiled `savePlace`) replaces only the first line.
    r = saveHealResult(s, "* Custom.");
    assert(r.lines[0] == "* Custom." && r.lines.size() == 2);
    assert(saveLines("ruins_fall", 0, true).size() == 1);

    // Furry route: FLUFF restored to max HP + 10, HP and FUR untouched.
    GameState f;
    f.player.hp = 5;
    f.player.maxHp = 20;
    f.furry.value = 40;
    f.setFlag("route.furry", 1);
    f.setFlag("fluff.value", 4);
    r = saveHealResult(f);
    assert(r.furryRoute && r.fluffAfter == 30 && fluffMax(f) == 30);
    assert(r.hpAfter == 5 && r.furAfter == 40);
    assert(r.lines.size() == 1 && r.lines[0] == savePlaceLine(f.currentRoom));
    f.setFlag("fluff.value", 30);
    assert(saveHealResult(f).fluffAfter == 30);
    f.furry.value = 0;
    assert(saveHealResult(f).furAfter == 0);

    // Toxic route: bare line, FUR never touched.
    GameState t;
    t.killsTotal = 20;
    t.killsByArea["ruins"] = 20;
    t.player.hp = 1;
    t.furry.value = 10;
    r = saveHealResult(t);
    assert(r.lines.size() == 1 && r.lines[0] == "* Determination.");
    assert(r.hpAfter == t.player.maxHp && r.furAfter == 10);

    std::cout << "[PASS] SAV-007: LORE save lines, FUR line, Toxic line, HP/FLUFF heal never touches FUR." << std::endl;
}
