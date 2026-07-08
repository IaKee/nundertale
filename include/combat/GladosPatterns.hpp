#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace nundertale {

// C1-025: authored, 1-based turn ranges for GLaDOwOS's survival test.
struct GladosPhaseDef {
    std::string id;
    int startTurn = 1;
    int endTurn = 1;
    std::vector<std::string> patterns;
};

inline const GladosPhaseDef* gladosPhaseForTurn(const std::vector<GladosPhaseDef>& phases,
                                                int turn) {
    for (const GladosPhaseDef& phase : phases) {
        if (turn >= phase.startTurn && turn <= phase.endTurn && !phase.patterns.empty()) {
            return &phase;
        }
    }
    return nullptr;
}

inline std::string gladosPatternForTurn(const std::vector<GladosPhaseDef>& phases, int turn) {
    const GladosPhaseDef* phase = gladosPhaseForTurn(phases, turn);
    if (phase == nullptr) return {};
    const int offset = turn - phase->startTurn;
    const std::size_t index = static_cast<std::size_t>(offset) % phase->patterns.size();
    return phase->patterns[index];
}

inline bool gladosSurvivalComplete(int completedTurns, int surviveTurns) {
    return surviveTurns > 0 && completedTurns >= surviveTurns;
}

struct GladosPortalTransit {
    float x = 0.0f;
    bool teleported = false;
};

// A bullet keeps its velocity: entering the wall it is travelling toward makes it
// leave the opposite portal and continue across the box.
inline GladosPortalTransit gladosPortalTransit(float x, float velocityX,
                                                float leftWall, float rightWall,
                                                float inset = 8.0f) {
    if (velocityX > 0.0f && x >= rightWall - inset) {
        return { leftWall + inset, true };
    }
    if (velocityX < 0.0f && x <= leftWall + inset) {
        return { rightWall - inset, true };
    }
    return { x, false };
}

} // namespace nundertale
