#pragma once

#include <algorithm>
#include <cmath>

namespace nundertale {

constexpr float ATTACK_BAR_CROSS_SECONDS = 1.6f;   // time to cross the target width
constexpr float ATTACK_HIT_HOLD_SECONDS  = 0.55f;  // target stays on screen after Z (bar blinks, slash plays)
constexpr float ATTACK_TARGET_FADE_SECONDS = 0.30f; // target squishes/fades out after the hold
constexpr float ATTACK_CRIT_RADIUS = 12.0f;        // keep the current crit threshold

enum class FightPhase {
    Moving,
    HitHold,
    Fade
};

inline float attackBarSpeed(float targetWidth) {
    return targetWidth / ATTACK_BAR_CROSS_SECONDS;
}

inline bool attackBarBlinkWhite(float sinceHit) {
    if (sinceHit < 0.0f) {
        sinceHit = 0.0f;
    }
    return (static_cast<int>(sinceHit * 15.0f) % 2) == 0;
}

inline float attackTargetSquish(float sinceFadeStart) {
    if (sinceFadeStart <= 0.0f) {
        return 1.0f;
    }
    if (sinceFadeStart >= ATTACK_TARGET_FADE_SECONDS) {
        return 0.0f;
    }
    return std::clamp(1.0f - (sinceFadeStart / ATTACK_TARGET_FADE_SECONDS), 0.0f, 1.0f);
}

inline bool attackBarMissed(float barX, float speed, float left, float right) {
    if (speed > 0.0f) {
        return barX >= right;
    }
    if (speed < 0.0f) {
        return barX <= left;
    }
    return false;
}

} // namespace nundertale
