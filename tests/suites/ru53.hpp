#pragma once

#include "combat/FloofyScript.hpp"

#include <cassert>
#include <cmath>
#include <iostream>

inline void suite_ru53() {
    using namespace nundertale;

    assert(r2TutorialWaveSize(0) == 1);
    assert(r2TutorialWaveSize(1) == 3);
    assert(r2TutorialWaveSize(2) == 6);
    assert(r2TutorialWaveSize(99) == 6);

    assert(r2TutorialHpAfterCatch(20, 2) == 18);
    assert(r2TutorialHpAfterCatch(2, 8) == 1);
    assert(r2TutorialHpAfterCatch(1, 8) == 1);

    const GiantPetalSweep sweep{ 240.0f, 160.0f, 80.0f, 20.0f, 12.0f, 0.75f };
    assert(std::fabs(giantPetalXAt(sweep, 0.0f) - 240.0f) < 0.001f);
    assert(std::fabs(giantPetalXAt(sweep, 2.0f) - 280.0f) < 0.001f);
    assert(std::fabs(giantPetalSafeZoneStart(sweep) - 388.0f) < 0.001f);
    assert(giantPetalHitsSoul(sweep, 360.0f, 370.0f));
    assert(!giantPetalHitsSoul(sweep, 390.0f, 392.0f));

    const float rescueX = sweep.arenaLeft + sweep.arenaWidth * sweep.rescueProgress;
    assert(giantPetalRescueTriggered(sweep, rescueX));
    assert(!giantPetalWouldFillBox(sweep, rescueX));

    std::cout << "[PASS] C1-002: R2 tutorial escalation, HP floor, giant petal sweep, safe wall, and rescue timing verified."
              << std::endl;
}
