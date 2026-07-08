#pragma once

#include "combat/Hitmarker.hpp"
#include "core/ToxicTrack.hpp"

#include <cassert>
#include <cmath>
#include <iostream>

inline void suite_ru23() {
    using namespace nundertale;

    // 1. Initial / idle state
    HitmarkerState hm;
    assert(!hm.active);
    assert(hm.timer == 0.0f);
    assert(hm.alpha() == 0.0f);

    // 2. Spawn condition: only while track == 1
    GameState state;
    // Track == 0 (unstarted / neutral)
    assert(toxicTrack(state) == 0);
    assert(!shouldPlayToxicHitmarker(state));
    assert(!spawnHitmarkerIfToxic(hm, state, 320.0f, 150.0f));
    assert(!hm.active);

    // Track == 2 (ended via spare)
    state.setFlag("toxic.track", 2);
    assert(toxicTrack(state) == 2);
    assert(!shouldPlayToxicHitmarker(state));
    assert(!spawnHitmarkerIfToxic(hm, state, 320.0f, 150.0f));
    assert(!hm.active);

    // Track == 1 (active toxic track)
    state.setFlag("toxic.track", 1);
    assert(toxicTrack(state) == 1);
    assert(shouldPlayToxicHitmarker(state));
    assert(spawnHitmarkerIfToxic(hm, state, 320.0f, 150.0f, 0.20f));
    assert(hm.active);
    assert(hm.timer == 0.20f);
    assert(hm.lifetime == 0.20f);
    assert(hm.centerX == 320.0f);
    assert(hm.centerY == 150.0f);

    // 3. Sound ID
    assert(hitmarkerSoundId() == "assets/sfx/hitmarker.ogg");
    assert(HITMARKER_SFX == "assets/sfx/hitmarker.ogg");

    // 4. Geometry verification (4 ticks around center forming an X)
    const auto ticks = hm.ticks();
    constexpr float kDiag = 0.7071067811865475f;
    const float inOff = hm.innerRadius * kDiag;
    const float outOff = (hm.innerRadius + hm.tickLength) * kDiag;

    // Tick 0: Top-Left (-dx, -dy)
    assert(std::abs(ticks[0].x1 - (320.0f - inOff)) < 1e-4f);
    assert(std::abs(ticks[0].y1 - (150.0f - inOff)) < 1e-4f);
    assert(std::abs(ticks[0].x2 - (320.0f - outOff)) < 1e-4f);
    assert(std::abs(ticks[0].y2 - (150.0f - outOff)) < 1e-4f);

    // Tick 1: Top-Right (+dx, -dy)
    assert(std::abs(ticks[1].x1 - (320.0f + inOff)) < 1e-4f);
    assert(std::abs(ticks[1].y1 - (150.0f - inOff)) < 1e-4f);
    assert(std::abs(ticks[1].x2 - (320.0f + outOff)) < 1e-4f);
    assert(std::abs(ticks[1].y2 - (150.0f - outOff)) < 1e-4f);

    // Tick 2: Bottom-Left (-dx, +dy)
    assert(std::abs(ticks[2].x1 - (320.0f - inOff)) < 1e-4f);
    assert(std::abs(ticks[2].y1 - (150.0f + inOff)) < 1e-4f);
    assert(std::abs(ticks[2].x2 - (320.0f - outOff)) < 1e-4f);
    assert(std::abs(ticks[2].y2 - (150.0f + outOff)) < 1e-4f);

    // Tick 3: Bottom-Right (+dx, +dy)
    assert(std::abs(ticks[3].x1 - (320.0f + inOff)) < 1e-4f);
    assert(std::abs(ticks[3].y1 - (150.0f + inOff)) < 1e-4f);
    assert(std::abs(ticks[3].x2 - (320.0f + outOff)) < 1e-4f);
    assert(std::abs(ticks[3].y2 - (150.0f + outOff)) < 1e-4f);

    // Verify lengths of all 4 ticks match tickLength
    for (size_t i = 0; i < 4; ++i) {
        float dx = ticks[i].x2 - ticks[i].x1;
        float dy = ticks[i].y2 - ticks[i].y1;
        float len = std::sqrt(dx * dx + dy * dy);
        assert(std::abs(len - hm.tickLength) < 1e-4f);
    }

    // 5. Fade and expiry timing
    assert(std::abs(hm.alpha() - 1.0f) < 1e-4f);

    // Halfway update
    hm.update(0.10f);
    assert(hm.active);
    assert(std::abs(hm.timer - 0.10f) < 1e-4f);
    assert(std::abs(hm.alpha() - 0.5f) < 1e-4f);

    // Near expiry
    hm.update(0.09f);
    assert(hm.active);
    assert(std::abs(hm.timer - 0.01f) < 1e-4f);
    assert(std::abs(hm.alpha() - 0.05f) < 1e-4f);

    // Expiry
    hm.update(0.02f);
    assert(!hm.active);
    assert(hm.timer == 0.0f);
    assert(hm.alpha() == 0.0f);

    // Updating while inactive is a no-op
    hm.update(0.5f);
    assert(!hm.active);
    assert(hm.timer == 0.0f);

    // Reset clears active and timer
    hm.spawn(100.0f, 100.0f, 0.5f);
    assert(hm.active);
    hm.reset();
    assert(!hm.active);
    assert(hm.timer == 0.0f);
    assert(hm.alpha() == 0.0f);

    std::cout << "[PASS] TOX-005: CoD hitmarker spawn (track==1 only), 4-tick geometry, sound ID, and lifetime/fade verified."
              << std::endl;
}
