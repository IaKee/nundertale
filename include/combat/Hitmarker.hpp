#pragma once

#include "core/ToxicTrack.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <string_view>

namespace nundertale {

inline constexpr std::string_view HITMARKER_SFX = "assets/sfx/hitmarker.ogg";
inline constexpr float HITMARKER_DEFAULT_LIFETIME = 0.20f;   // ~6 frames at 30 fps (0.2s)
inline constexpr float HITMARKER_DEFAULT_INNER_RADIUS = 3.0f; // Gap around center
inline constexpr float HITMARKER_DEFAULT_LENGTH = 6.0f;       // Length of each tick

struct HitmarkerTick {
    float x1 = 0.0f;
    float y1 = 0.0f;
    float x2 = 0.0f;
    float y2 = 0.0f;
};

// Pure state for CoD-style 4-tick hitmarker on the Toxic Track (TOX-005).
struct HitmarkerState {
    bool active = false;
    float timer = 0.0f;
    float lifetime = HITMARKER_DEFAULT_LIFETIME;
    float centerX = 0.0f;
    float centerY = 0.0f;
    float innerRadius = HITMARKER_DEFAULT_INNER_RADIUS;
    float tickLength = HITMARKER_DEFAULT_LENGTH;

    void spawn(float cx, float cy, float dur = HITMARKER_DEFAULT_LIFETIME,
               float innerR = HITMARKER_DEFAULT_INNER_RADIUS,
               float tickLen = HITMARKER_DEFAULT_LENGTH) {
        active = true;
        timer = dur;
        lifetime = dur > 0.0f ? dur : HITMARKER_DEFAULT_LIFETIME;
        centerX = cx;
        centerY = cy;
        innerRadius = innerR;
        tickLength = tickLen;
    }

    void reset() {
        active = false;
        timer = 0.0f;
    }

    void update(float dt) {
        if (!active) return;
        timer -= dt;
        if (timer <= 0.0f) {
            timer = 0.0f;
            active = false;
        }
    }

    // Alpha / opacity in [0.0f, 1.0f], fading linearly from 1.0f down to 0.0f over lifetime.
    [[nodiscard]] float alpha() const {
        if (!active || lifetime <= 0.0f) return 0.0f;
        return std::clamp(timer / lifetime, 0.0f, 1.0f);
    }

    // Four diagonal ticks forming an 'X' around (centerX, centerY):
    // Index 0: Top-Left     (-dx, -dy)
    // Index 1: Top-Right    (+dx, -dy)
    // Index 2: Bottom-Left  (-dx, +dy)
    // Index 3: Bottom-Right (+dx, +dy)
    // where dx = dy = offset * 1/sqrt(2) (normalized 45-degree diagonal)
    [[nodiscard]] std::array<HitmarkerTick, 4> ticks() const {
        // sqrt(0.5) approx 0.70710678f
        constexpr float kDiag = 0.7071067811865475f;
        const float r1 = innerRadius;
        const float r2 = innerRadius + tickLength;

        const float inOff = r1 * kDiag;
        const float outOff = r2 * kDiag;

        std::array<HitmarkerTick, 4> result{};

        // 0: Top-Left (negative X, negative Y)
        result[0] = {
            centerX - inOff,  centerY - inOff,
            centerX - outOff, centerY - outOff
        };

        // 1: Top-Right (positive X, negative Y)
        result[1] = {
            centerX + inOff,  centerY - inOff,
            centerX + outOff, centerY - outOff
        };

        // 2: Bottom-Left (negative X, positive Y)
        result[2] = {
            centerX - inOff,  centerY + inOff,
            centerX - outOff, centerY + outOff
        };

        // 3: Bottom-Right (positive X, positive Y)
        result[3] = {
            centerX + inOff,  centerY + inOff,
            centerX + outOff, centerY + outOff
        };

        return result;
    }
};

// Spawns a hitmarker only when the toxic track is currently running (toxic.track == 1).
// Returns true if spawned, false otherwise.
inline bool spawnHitmarkerIfToxic(HitmarkerState& hm, const GameState& state,
                                  float cx, float cy,
                                  float dur = HITMARKER_DEFAULT_LIFETIME) {
    if (toxicTrack(state) == 1) {
        hm.spawn(cx, cy, dur);
        return true;
    }
    return false;
}

// Check whether hitmarker sound should play on every hit (toxic.track == 1).
inline bool shouldPlayToxicHitmarker(const GameState& state) {
    return toxicTrack(state) == 1;
}

inline std::string_view hitmarkerSoundId() {
    return HITMARKER_SFX;
}

} // namespace nundertale
