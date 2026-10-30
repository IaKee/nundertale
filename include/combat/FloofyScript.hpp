#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <string>
#include <vector>

namespace nundertale {

struct ScriptedBattleEnd {
    int afterEnemyTurns = 0;
    bool interruptOnTurnStart = false;
    std::string endCutscene;

    bool enabled() const { return afterEnemyTurns > 0; }
};

enum class MercyOption {
    Spare,
    Flee
};

// Scripted tutorial battles use their patterns in declaration order instead of
// weighted random order. Turn numbers are one-based, matching BattleSystem.
inline std::size_t scriptedPatternIndex(int turnNumber, std::size_t patternCount) {
    if (patternCount == 0U) return 0U;
    if (turnNumber <= 1) return 0U;
    const std::size_t index = static_cast<std::size_t>(turnNumber - 1);
    return index < patternCount ? index : patternCount - 1U;
}

inline bool scriptedBattleEndsAfterTurn(int completedTurns, int scriptedEndAfterTurns) {
    return scriptedEndAfterTurns > 0 && completedTurns >= scriptedEndAfterTurns;
}

// The retry counter records every loss. The first two retries keep the authored
// turn count; the third and later losses bring the interruption one turn closer.
inline int effectiveScriptedEndTurn(int afterEnemyTurns, int retryCount, int minimumTurn = 3) {
    if (afterEnemyTurns <= 0) return 0;
    const int floor = std::max(1, std::min(minimumTurn, afterEnemyTurns));
    const int turnOffset = std::max(0, retryCount - 2);
    return std::max(floor, afterEnemyTurns - turnOffset);
}

inline bool scriptedBattleEndsOnTurnStart(int turnNumber, int endTurn, bool interruptOnTurnStart) {
    return interruptOnTurnStart && endTurn > 0 && turnNumber >= endTurn;
}

inline bool scriptedBattleEndsAfterTurn(int completedTurns, int endTurn, bool interruptOnTurnStart) {
    return !interruptOnTurnStart && scriptedBattleEndsAfterTurn(completedTurns, endTurn);
}

inline std::vector<MercyOption> battleMercyOptions(bool canSpare,
                                                    bool canFlee,
                                                    bool hasBlockedFleeText) {
    std::vector<MercyOption> options;
    if (canSpare) options.push_back(MercyOption::Spare);
    if (canFlee || hasBlockedFleeText) options.push_back(MercyOption::Flee);
    return options;
}

inline std::string joinBattleText(const std::vector<std::string>& lines) {
    std::string joined;
    for (const std::string& line : lines) {
        if (line.empty()) continue;
        if (!joined.empty()) joined += '\n';
        joined += line;
    }
    return joined;
}

inline int battleFurDelta(int currentFur,
                          int requestedDelta,
                          int furCap,
                          float multiplier = 1.0f) {
    if (requestedDelta <= 0) return requestedDelta;
    const int scaled = std::max(0, static_cast<int>(std::lround(
        static_cast<float>(requestedDelta) * multiplier)));
    if (furCap < 0) return scaled;
    return std::min(scaled, std::max(0, furCap - currentFur));
}

// C1-002 v3.5: the fake tutorial escalates by attempts, not by menu turns.
// Keeping these rules pure makes the authored timings/data independently testable.
inline int r2TutorialWaveSize(int waveIndex) {
    if (waveIndex <= 0) return 1;
    if (waveIndex == 1) return 3;
    return 6;
}

inline int r2TutorialHpAfterCatch(int hp, int damage) {
    return std::max(1, hp - std::max(0, damage));
}

struct GiantPetalSweep {
    float arenaLeft = 0.0f;
    float arenaWidth = 0.0f;
    float spriteWidth = 0.0f;
    float speed = 20.0f;
    float safeWallWidth = 12.0f;
    float rescueProgress = 0.75f;
};

inline float giantPetalXAt(const GiantPetalSweep& sweep, float elapsedSeconds) {
    return sweep.arenaLeft + std::max(0.0f, elapsedSeconds) * std::max(0.0f, sweep.speed);
}

inline float giantPetalProgress(const GiantPetalSweep& sweep, float centerX) {
    if (sweep.arenaWidth <= 0.0f) return 1.0f;
    return std::clamp((centerX - sweep.arenaLeft) / sweep.arenaWidth, 0.0f, 1.0f);
}

inline float giantPetalSafeZoneStart(const GiantPetalSweep& sweep) {
    return sweep.arenaLeft + std::max(0.0f, sweep.arenaWidth - sweep.safeWallWidth);
}

inline bool giantPetalHitsSoul(const GiantPetalSweep& sweep, float petalCenterX,
                               float soulCenterX, float soulRadius = 4.0f) {
    if (soulCenterX >= giantPetalSafeZoneStart(sweep)) return false;
    const float halfWidth = std::max(0.0f, sweep.spriteWidth * 0.5f);
    return soulCenterX + soulRadius >= petalCenterX - halfWidth &&
           soulCenterX - soulRadius <= petalCenterX + halfWidth;
}

inline bool giantPetalRescueTriggered(const GiantPetalSweep& sweep, float petalCenterX) {
    return giantPetalProgress(sweep, petalCenterX) >=
           std::clamp(sweep.rescueProgress, 0.0f, 1.0f);
}

inline bool giantPetalWouldFillBox(const GiantPetalSweep& sweep, float petalCenterX) {
    // The sweep leaves danger behind its moving centre line; the box is only
    // fully occupied when that line reaches the right edge.
    return petalCenterX >= sweep.arenaLeft + sweep.arenaWidth;
}

} // namespace nundertale
