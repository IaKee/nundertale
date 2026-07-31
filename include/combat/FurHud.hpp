#pragma once

#include <cstddef>

namespace nundertale {

inline bool furHudVisible(int fur) {
    return fur > 0;
}

inline bool furHudShouldTremble(float sinceAppear) {
    return sinceAppear >= 0.0f && sinceAppear < 1.5f;
}

inline int furHudJitter(float t) {
    if (!furHudShouldTremble(t)) {
        return 0;
    }

    // Undertale runs at 30 fps (LOGIC_TIMESTEP = 1.0f / 30.0f).
    // Alternating every ~2 frames means a step period of ~2/30s = 1/15s (~0.0667s).
    // Deterministic cycle: {-1, 1, 0}
    constexpr int kJitterCycle[] = { -1, 1, 0 };
    int step = static_cast<int>(t * 15.0f);
    if (step < 0) {
        step = 0;
    }
    return kJitterCycle[static_cast<size_t>(step % 3)];
}

// FUR-003 for scenes that draw their own HUD (the battle intro): a clock that starts the tremble when the
// bar first appears. update() returns true on the exact call where the 1.5 s tremble ends, which is when
// the caller stores `fur_hud_seen` so the battle proper does not tremble it a second time.
struct FurHudTrembleClock {
    float t = -1.0f;

    void start() { t = 0.0f; }
    bool running() const { return furHudShouldTremble(t); }
    int jitter() const { return furHudJitter(t); }

    bool update(float dt) {
        if (!running()) return false;
        t += dt;
        return !furHudShouldTremble(t);
    }
};

} // namespace nundertale
