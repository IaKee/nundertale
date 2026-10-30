#pragma once

#include "scenes/LoreScroll.hpp"

#include <cassert>
#include <cmath>
#include <iostream>
#include <string_view>

inline void suite_ru56() {
    using namespace nundertale;

    // BUG-55 / FRT-013: card 6 is typed with a dotted fill, dots ~100x faster than letters
    const std::string_view line1 = LORE_CARD_6_LINE_1;
    const std::string_view line2 = LORE_CARD_6_LINE_2;

    // Nothing is typed at t = 0, so the card never appears all at once
    assert(loreCharsTyped(line1, 0.0f) == 0);
    assert(loreCharsTyped(line2, 0.0f) == 0);

    // "<<" takes two letter delays, then the leading dots fly by in a fraction of one letter delay
    assert(loreCharsTyped(line1, LORE_CHARACTER_DELAY * 0.5f) == 0);
    assert(loreCharsTyped(line1, LORE_CHARACTER_DELAY * 2.0f + LORE_CHARACTER_DELAY * 0.5f) > 3);
    // 16 leading dots cost ~16 / 100 of a letter delay: one letter delay past "<<" reaches the first letter
    assert(loreCharsTyped(line1, LORE_CHARACTER_DELAY * 2.0f + LORE_CHARACTER_DELAY * 0.2f) >= 18);

    // The words keep the normal pace: "MTn. DEW" is not instant
    const float dotsOnly = loreTypingTime(line1, 18);
    assert(dotsOnly < LORE_CHARACTER_DELAY * 2.5f);
    assert(loreTypingTime(line1, line1.size()) > LORE_CHARACTER_DELAY * 8.0f);

    // Whole card takes real time, but far less than if the dots were typed as letters
    const float total = loreTypingTime(line1, line1.size()) + loreTypingTime(line2, line2.size());
    assert(total > 0.5f);
    assert(total < LORE_CHARACTER_DELAY * static_cast<float>(line1.size() + line2.size()) / 2.0f);

    // Full duration types everything; one tick shy of it doesn't
    assert(loreCharsTyped(line1, total + 1.0f) == line1.size());
    assert(loreCharsTyped(line1, loreTypingTime(line1, line1.size()) - 0.001f) < line1.size());

    // Plain text (other cards) is one character per letter delay, unchanged
    const std::string_view plain = "Long ago, two races ruled";
    assert(loreCharsTyped(plain, LORE_CHARACTER_DELAY * 5.5f) == 5);
    assert(std::fabs(loreTypingTime(plain, 4) - LORE_CHARACTER_DELAY * 4.0f) < 1e-5f);

    std::cout << "[PASS] BUG-55: card 6 is typed with the dotted fill (dots 100x faster, not instant)."
              << std::endl;
}
