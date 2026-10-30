#pragma once

#include <cmath>
#include <algorithm>
#include <cstddef>
#include <string_view>

namespace nundertale {

inline constexpr const char* LORE_CARD_1_LAST_LINE = "FURRIES and GAMERS.";
inline constexpr const char* LORE_CARD_6_LINE_1 =
    "<<................MTn. DEW................>>";
inline constexpr const char* LORE_CARD_6_LINE_2 =
    "<<...................202X....................>>";

inline constexpr float LORE_CHARACTER_DELAY = 0.04f;
inline constexpr float LORE_DOT_SPEED_MULTIPLIER = 100.0f;

/** Return the typewriter delay for one character. */
inline constexpr float loreCharacterDelay(char character) {
    return character == '.'
        ? LORE_CHARACTER_DELAY / LORE_DOT_SPEED_MULTIPLIER
        : LORE_CHARACTER_DELAY;
}

/** Seconds the typewriter needs to type the first `count` characters of a line. */
inline float loreTypingTime(std::string_view line, std::size_t count) {
    float total = 0.0f;
    for (std::size_t i = 0; i < count && i < line.size(); ++i) {
        total += loreCharacterDelay(line[i]);
    }
    return total;
}

/**
 * Characters of `line` typed after `elapsed` seconds (one per loreCharacterDelay, so a dotted
 * fill like card 6's flies by while the words keep the normal pace). The caller plays the
 * type sound once for every character this count grows by.
 */
inline std::size_t loreCharsTyped(std::string_view line, float elapsed) {
    std::size_t typed = 0;
    float spent = 0.0f;
    while (typed < line.size()) {
        spent += loreCharacterDelay(line[typed]);
        if (spent > elapsed) {
            break;
        }
        ++typed;
    }
    return typed;
}

/**
 * Pure function calculating the vertical source rect Y offset (in native pixels)
 * for scrolling a tall intro card inside a fixed-height window from bottom to top.
 *
 * Starts at the bottom of the image (y = imageH - windowH) at t <= 0,
 * scrolls up linearly to the top (y = 0) at t = duration, and holds at 0 for t >= duration.
 * Clamps t < 0 to (imageH - windowH), and t >= duration to 0.
 *
 * The progression moves in whole native pixels:
 *   offset = floor(progress * (imageH - windowH))
 *   y = (imageH - windowH) - offset
 *
 * @param t Elapsed scroll time in seconds.
 * @param duration Total scroll duration in seconds (e.g. 6.0f).
 * @param imageH Native height of the image (e.g. 320).
 * @param windowH Native height of the visible window (e.g. 110).
 * @return Integer native pixel Y coordinate for the top edge of the source rectangle.
 */
inline int loreScrollOffset(float t, float duration, int imageH, int windowH) {
    const int maxScroll = imageH - windowH;
    if (maxScroll <= 0) {
        return 0;
    }
    if (duration <= 0.0f || t >= duration) {
        return 0;
    }
    if (t <= 0.0f) {
        return maxScroll;
    }

    float progress = t / duration;
    if (progress < 0.0f) progress = 0.0f;
    if (progress > 1.0f) progress = 1.0f;

    const int moved = static_cast<int>(std::floor(progress * static_cast<float>(maxScroll)));
    const int clampedMoved = std::clamp(moved, 0, maxScroll);

    return maxScroll - clampedMoved;
}

} // namespace nundertale
