#pragma once

#include <cmath>
#include <algorithm>

namespace nundertale {

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
