#pragma once

#include <cassert>
#include <iostream>
#include <string>
#include <unordered_map>

#include "core/FloofyAnnoyance.hpp"

inline void suite_ru28() {
    using namespace nundertale;

    // Each source counts once, at its boundary.
    assert(floofyAnnoyance({}) == 0);
    assert(floofyAnnoyance({2, false, false, 1}) == 0);
    assert(floofyAnnoyance({3, false, false, 0}) == 1);
    assert(floofyAnnoyance({99, false, false, 0}) == 1);
    assert(floofyAnnoyance({0, true, false, 0}) == 1);
    assert(floofyAnnoyance({0, false, true, 0}) == 1);
    assert(floofyAnnoyance({0, false, false, 2}) == 1);
    assert(floofyAnnoyance({0, false, false, 50}) == 1);
    assert(floofyAnnoyance({3, true, true, 2}) == 4);
    assert(floofyAnnoyance({2, true, false, 1}) == 1);

    // Variant: 0,1 -> petals; 2+ -> bass.
    assert(floofyFightVariant(0) == 1);
    assert(floofyFightVariant(1) == 1);
    assert(floofyFightVariant(2) == 2);
    assert(floofyFightVariant(4) == 2);

    // Lock on first pick, reuse on retry even if annoyance changed.
    std::unordered_map<std::string, int> flags;
    assert(floofyLockFight(flags, 2) == 2);
    assert(flags["ruins.floofy.fight"] == 2);
    assert(floofyLockFight(flags, 0) == 2);
    std::unordered_map<std::string, int> flags2;
    assert(floofyLockFight(flags2, 1) == 1);
    assert(floofyLockFight(flags2, 4) == 1);
    std::unordered_map<std::string, int> flags3{{"ruins.floofy.fight", 0}};
    assert(floofyLockFight(flags3, 3) == 2);

    // Furiell turn: 6 through 2 deaths, then 5, 4, 3, floor at 3.
    assert(floofyFuriellTurn(0) == 6);
    assert(floofyFuriellTurn(1) == 6);
    assert(floofyFuriellTurn(2) == 6);
    assert(floofyFuriellTurn(3) == 5);
    assert(floofyFuriellTurn(4) == 4);
    assert(floofyFuriellTurn(5) == 3);
    assert(floofyFuriellTurn(6) == 3);
    assert(floofyFuriellTurn(1000) == 3);

    std::cout << "[PASS] Floofy annoyance: sources, fight variant, lock on retry, Furiell turn by deaths." << std::endl;
}
