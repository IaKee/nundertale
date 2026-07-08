#pragma once

#include <cstddef>

namespace nundertale {

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

} // namespace nundertale
