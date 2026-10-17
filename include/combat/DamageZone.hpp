#pragma once

#include <raylib.h>
#include <algorithm>

namespace nundertale {

struct DamageZone {
    Rectangle area{ 0.0f, 0.0f, 0.0f, 0.0f };
    float lifetime = 1.0f;
    int damagePerTick = 1;
    float tickInterval = 0.33f;
    float furPerTick = 0.0f;
    Color color{ 0, 192, 0, 90 };
    float tickTimer = 0.0f;

    int update(float dt, bool soulInside) {
        lifetime -= dt;
        tickTimer = std::max(0.0f, tickTimer - dt);

        if (soulInside && tickTimer <= 0.0f) {
            tickTimer = tickInterval;
            return damagePerTick;
        }

        return 0;
    }

    bool expired() const { return lifetime <= 0.0f; }
};

} // namespace nundertale
