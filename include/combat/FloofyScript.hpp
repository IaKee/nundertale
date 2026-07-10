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

} // namespace nundertale
