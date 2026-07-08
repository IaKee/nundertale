#pragma once

#include "core/ToxicTrack.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace nundertale {

// LORE section 8.9 defines these as cumulative toxic kill-count rewards.
// They deliberately do not use toxic.streak, which resets at save points.
inline constexpr int CARE_PACKAGE_THRESHOLD = 4;
inline constexpr int PREDATOR_MISSILE_THRESHOLD = 5;
inline constexpr int TACTICAL_NUKE_THRESHOLD = 25;

inline constexpr const char* CARE_EARNED_FLAG = "toxic.ks.care.earned";
inline constexpr const char* CARE_DROPPED_FLAG = "toxic.ks.care.dropped";
inline constexpr const char* PREDATOR_EARNED_FLAG = "toxic.ks.predator.earned";
inline constexpr const char* PREDATOR_USED_FLAG = "toxic.ks.predator.used";
inline constexpr const char* NUKE_EARNED_FLAG = "toxic.ks.nuke.earned";

// Awards every newly reached reward once. Calling this more than once for the
// same kill count is harmless, and rewards freeze as soon as the track ends.
inline void killstreaksOnKill(GameState& state) {
    if (toxicTrack(state) != 1) return;

    const int kills = toxicKills(state);
    if (kills >= CARE_PACKAGE_THRESHOLD && !state.hasFlag(CARE_EARNED_FLAG)) {
        state.setFlag(CARE_EARNED_FLAG, 1);
    }
    if (kills >= PREDATOR_MISSILE_THRESHOLD && !state.hasFlag(PREDATOR_EARNED_FLAG)) {
        state.setFlag(PREDATOR_EARNED_FLAG, 1);
    }
    if (kills >= TACTICAL_NUKE_THRESHOLD && !state.hasFlag(NUKE_EARNED_FLAG)) {
        state.setFlag(NUKE_EARNED_FLAG, 1);
    }
}

inline bool careDropPending(const GameState& state) {
    return state.hasFlag(CARE_EARNED_FLAG) && !state.hasFlag(CARE_DROPPED_FLAG);
}

inline void markCareDropped(GameState& state) {
    if (careDropPending(state)) state.setFlag(CARE_DROPPED_FLAG, 1);
}

// Returns an item ID from the canonical LORE section 20.1 Care Package pool.
inline std::string carePackageFood(std::uint32_t seed) {
    constexpr std::array<const char*, 4> foods{
        "shark", "lobster", "crayons", "white_monster"
    };
    return foods[seed % static_cast<std::uint32_t>(foods.size())];
}

inline bool predatorAvailable(const GameState& state) {
    return state.hasFlag(PREDATOR_EARNED_FLAG) && !state.hasFlag(PREDATOR_USED_FLAG);
}

inline bool usePredator(GameState& state) {
    if (!predatorAvailable(state)) return false;
    state.setFlag(PREDATOR_USED_FLAG, 1);
    return true;
}

// Heavy damage is 75% of maximum HP, capped by current HP - 1. Consequently a
// valid living target always keeps at least 1 HP; a 1-HP target takes no damage.
inline int predatorDamage(int targetHp, int targetMaxHp) {
    if (targetHp <= 1 || targetMaxHp <= 0) return 0;
    const std::int64_t heavyDamage =
        (static_cast<std::int64_t>(targetMaxHp) * 3) / 4;
    const std::int64_t cappedDamage =
        std::min(heavyDamage, static_cast<std::int64_t>(targetHp - 1));
    return static_cast<int>(std::max<std::int64_t>(1, cappedDamage));
}

struct KillstreakIcon {
    const char* name;
    int threshold;
    bool earned;
    bool used;
    bool locked;
};

inline std::vector<KillstreakIcon> killstreakColumn(const GameState& state) {
    const bool careEarned = state.hasFlag(CARE_EARNED_FLAG);
    const bool predatorEarned = state.hasFlag(PREDATOR_EARNED_FLAG);
    const bool nukeEarned = state.hasFlag(NUKE_EARNED_FLAG);
    return {
        { "Care Package", CARE_PACKAGE_THRESHOLD, careEarned,
          state.hasFlag(CARE_DROPPED_FLAG), !careEarned },
        { "Predator Missile", PREDATOR_MISSILE_THRESHOLD, predatorEarned,
          state.hasFlag(PREDATOR_USED_FLAG), !predatorEarned },
        { "Tactical Nuke", TACTICAL_NUKE_THRESHOLD, nukeEarned, false, !nukeEarned }
    };
}

} // namespace nundertale
