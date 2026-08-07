#pragma once

#include "scenes/LoreScroll.hpp"

#include <cassert>
#include <iostream>
#include <string_view>

inline void suite_ru30() {
    using namespace nundertale;

    assert(std::string_view(LORE_CARD_1_LAST_LINE) == "FURRIES and GAMERS.");
    assert(std::string_view(LORE_CARD_6_LINE_1) ==
           "<<................MTn. DEW................>>");
    assert(std::string_view(LORE_CARD_6_LINE_2) ==
           "<<...................202X....................>>");

    assert(loreCharacterDelay('A') == LORE_CHARACTER_DELAY);
    assert(loreCharacterDelay(' ') == LORE_CHARACTER_DELAY);
    assert(loreCharacterDelay('.') * LORE_DOT_SPEED_MULTIPLIER ==
           loreCharacterDelay('A'));

    std::cout << "[PASS] FRT-013: intro card strings and 100x dot typewriter speed verified."
              << std::endl;
}
