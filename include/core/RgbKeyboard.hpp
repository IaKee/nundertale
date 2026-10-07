#pragma once

#include <array>

namespace nundertale {

inline constexpr int RGB_KEY_COUNT = 5;
inline constexpr int RGB_KEY_START_MASK = 0b00010;
inline constexpr int RGB_KEY_TARGET_MASK = 0b11001;

struct RgbKeyboardColor {
    int r = 0;
    int g = 0;
    int b = 0;
};

inline constexpr std::array<RgbKeyboardColor, RGB_KEY_COUNT> RGB_KEY_COLORS = {{
    {255, 48, 72},    // red
    {255, 210, 48},   // yellow
    {64, 232, 112},   // green
    {72, 104, 255},   // blue
    {255, 56, 224},   // magenta
}};

inline bool rgbKeyboardKeyLit(int mask, int key) {
    return key >= 0 && key < RGB_KEY_COUNT && (mask & (1 << key)) != 0;
}

inline int rgbKeyboardPressMask(int key) {
    if (key < 0 || key >= RGB_KEY_COUNT) return 0;
    int toggles = 1 << key;
    if (key > 0) toggles |= 1 << (key - 1);
    if (key + 1 < RGB_KEY_COUNT) toggles |= 1 << (key + 1);
    return toggles;
}

inline int pressRgbKeyboardKey(int mask, int key) {
    return (mask ^ rgbKeyboardPressMask(key)) & ((1 << RGB_KEY_COUNT) - 1);
}

inline bool rgbKeyboardSolved(int mask) {
    return mask == RGB_KEY_TARGET_MASK;
}

inline RgbKeyboardColor rgbKeyboardTint(int mask) {
    RgbKeyboardColor mixed;
    int lit = 0;
    for (int key = 0; key < RGB_KEY_COUNT; ++key) {
        if (!rgbKeyboardKeyLit(mask, key)) continue;
        mixed.r += RGB_KEY_COLORS[static_cast<std::size_t>(key)].r;
        mixed.g += RGB_KEY_COLORS[static_cast<std::size_t>(key)].g;
        mixed.b += RGB_KEY_COLORS[static_cast<std::size_t>(key)].b;
        ++lit;
    }
    if (lit == 0) return {24, 20, 32};
    mixed.r /= lit;
    mixed.g /= lit;
    mixed.b /= lit;
    return mixed;
}

inline bool rgbKeyboardTintIsPink(const RgbKeyboardColor& color) {
    return color.r >= 180 && color.b >= 140 && color.g < 100;
}

inline int rgbKeyboardMinimumPresses(int start, int target) {
    constexpr int STATE_COUNT = 1 << RGB_KEY_COUNT;
    if (start < 0 || start >= STATE_COUNT || target < 0 || target >= STATE_COUNT) return -1;
    std::array<int, STATE_COUNT> distance{};
    distance.fill(-1);
    std::array<int, STATE_COUNT> queue{};
    int head = 0;
    int tail = 0;
    queue[static_cast<std::size_t>(tail++)] = start;
    distance[static_cast<std::size_t>(start)] = 0;
    while (head < tail) {
        const int state = queue[static_cast<std::size_t>(head++)];
        if (state == target) return distance[static_cast<std::size_t>(state)];
        for (int key = 0; key < RGB_KEY_COUNT; ++key) {
            const int next = pressRgbKeyboardKey(state, key);
            if (distance[static_cast<std::size_t>(next)] >= 0) continue;
            distance[static_cast<std::size_t>(next)] = distance[static_cast<std::size_t>(state)] + 1;
            queue[static_cast<std::size_t>(tail++)] = next;
        }
    }
    return -1;
}

} // namespace nundertale
