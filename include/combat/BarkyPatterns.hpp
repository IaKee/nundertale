#pragma once

namespace nundertale {

enum class BarkyPattern {
    UwuRain,
    DewBounce,
    HowlWave
};

inline BarkyPattern barkyPatternForTurn(int turnIndex, bool howlUnlocked, bool howlJustUnlocked) {
    if (howlJustUnlocked) {
        return BarkyPattern::HowlWave;
    }
    if (!howlUnlocked) {
        return (turnIndex % 2 == 0) ? BarkyPattern::UwuRain : BarkyPattern::DewBounce;
    }

    switch (turnIndex % 3) {
        case 0:
            return BarkyPattern::UwuRain;
        case 1:
            return BarkyPattern::DewBounce;
        default:
            return BarkyPattern::HowlWave;
    }
}

inline bool howlGapContains(float gapTop, float gapHeight, float y) {
    return y >= gapTop && y <= gapTop + gapHeight;
}

inline bool howlNextWallReady(float travelled, float boxW, float sinceLast, float minTravelRatio = 0.60f, float minInterval = 1.4f) {
    constexpr float EPSILON = 1e-4f;
    return sinceLast >= minInterval && (travelled + EPSILON) >= (boxW * minTravelRatio);
}

} // namespace nundertale
