#pragma once

#include "combat/Corruption.hpp"

#include <cassert>
#include <iostream>

inline void suite_ru29() {
    using namespace nundertale;

    const int boundaries[][2] = {{0, 0}, {1, 1}, {8, 1}, {9, 2}, {15, 2},
                                 {16, 3}, {21, 3}, {22, 4}, {25, 4}, {26, 4}};
    for (const auto& b : boundaries) {
        assert(corruptionLook(b[0]).stage == b[1]);
        assert(corruptionLook(b[0]).stage == toxicCorruptionStage(b[0]));
    }
    assert(corruptionLook(-5).stage == 0);
    assert(corruptionLook(1000).stage == 4);

    const CorruptionLook plain = corruptionLook(0);
    assert(plain.gridFlicker == 0.0f && plain.scanlines == 0 && !plain.mercyGray);
    assert(plain.overlayAlpha == 0.0f);

    const CorruptionLook s1 = corruptionLook(8);
    assert(s1.gridFlicker > 0.0f && s1.gridDarkness == 0.0f && s1.scanlines == 0);
    assert(!s1.mercyGray && !s1.fightGlow);

    const CorruptionLook s2 = corruptionLook(9);
    assert(s2.gridDarkness > 0.0f && s2.scanlines > 0 && s2.mercyGray);
    assert(!s2.mercyCracked && !s2.fightGlow);

    const CorruptionLook s3 = corruptionLook(16);
    assert(s3.mercyGray && s3.mercyCracked && s3.fightGlow && !s3.full);
    assert(s3.glitch > s2.glitch);

    const CorruptionLook s4 = corruptionLook(22);
    assert(s4.full && s4.fightGlow && s4.mercyCracked);

    // Monotonic escalation and readability guarantee at every kill count.
    CorruptionLook prev = corruptionLook(0);
    for (int k = 0; k <= 30; ++k) {
        const CorruptionLook l = corruptionLook(k);
        assert(l.stage >= prev.stage);
        assert(l.glitch >= prev.glitch && l.scanlines >= prev.scanlines);
        assert(l.overlayAlpha <= CORRUPTION_MAX_OVERLAY_ALPHA);
        assert(l.textContrast >= CORRUPTION_MIN_TEXT_CONTRAST);
        assert(!l.hudHidden && l.mercyUsable);
        prev = l;
    }

    GameState state;
    state.setFlag("toxic.kills", 16);
    assert(corruptionLook(state).stage == 3);

    std::cout << "[PASS] TOX-008: corruption ladder stages, visual params and readability at boundaries."
              << std::endl;
}
