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

} // namespace nundertale
