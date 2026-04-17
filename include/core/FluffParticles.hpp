#pragma once

#include <vector>
#include <cstdint>
#include <cmath>
#include <algorithm>

namespace nundertale {

inline int fluffTargetCount(int fur, bool toxicRoute) {
    if (toxicRoute) {
        return 0;
    }
    int target = 2 + (fur * 2) / 100;
    if (target < 2) {
        target = 2;
    }
    if (target > 4) {
        target = 4;
    }
    return target;
}

struct FluffParticle {
    float x = 0.0f;
    float y = 0.0f;
    float vx = 0.0f;
    float vy = 0.0f;
    float phase = 0.0f;
    unsigned char colorIndex = 0;
};

// Tiny deterministic LCG for fluff simulation (no raylib RNG, pure and testable)
inline std::uint32_t nextFluffRng(std::uint32_t& state) {
    state = state * 1664525u + 1013904223u;
    return state;
}

inline float fluffRandomFloat(std::uint32_t& state, float minVal, float maxVal) {
    float norm = static_cast<float>(nextFluffRng(state) & 0x00FFFFFFu) / 16777216.0f;
    return minVal + norm * (maxVal - minVal);
}

inline float fluffDrawX(const FluffParticle& p) {
    float drawX = p.x + 6.0f * std::sin(p.phase);
    while (drawX < 0.0f) {
        drawX += 640.0f;
    }
    while (drawX >= 640.0f) {
        drawX -= 640.0f;
    }
    return drawX;
}

inline void updateFluff(std::vector<FluffParticle>& particles, float dt, int target, std::uint32_t& rngState) {
    const size_t targetCount = (target > 0) ? static_cast<size_t>(target) : 0u;

    // Add / remove at most one particle per update to reach target
    if (particles.size() < targetCount) {
        FluffParticle p;
        p.x = fluffRandomFloat(rngState, 0.0f, 640.0f);
        p.y = fluffRandomFloat(rngState, 0.0f, 480.0f);
        p.vx = fluffRandomFloat(rngState, -6.0f, 6.0f);
        p.vy = fluffRandomFloat(rngState, 4.0f, 10.0f);
        p.phase = fluffRandomFloat(rngState, 0.0f, 6.2831853f);
        p.colorIndex = static_cast<unsigned char>(nextFluffRng(rngState) % 2u);
        particles.push_back(p);
    } else if (particles.size() > targetCount) {
        particles.pop_back();
    }

    // Update particle positions, sway, and wrapping
    for (auto& p : particles) {
        p.phase += dt * 2.0f;
        if (p.phase > 6.2831853f * 100.0f) {
            p.phase = std::fmod(p.phase, 6.2831853f);
        }

        p.x += p.vx * dt;
        while (p.x < 0.0f) {
            p.x += 640.0f;
        }
        while (p.x >= 640.0f) {
            p.x -= 640.0f;
        }

        p.y += p.vy * dt;
        if (p.y >= 480.0f) {
            p.y = 0.0f;
            p.x = fluffRandomFloat(rngState, 0.0f, 640.0f);
        } else if (p.y < 0.0f) {
            p.y += 480.0f;
        }
    }
}

} // namespace nundertale
