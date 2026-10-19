#pragma once

#include "core/GameContext.hpp"
#include <array>
#include <cstddef>
#include <string>
#include <string_view>

namespace nundertale {

inline constexpr std::string_view TOXIC_TRACK_FLAG = "toxic.track";
inline constexpr std::string_view TOXIC_KILLS_FLAG = "toxic.kills";

inline bool toxicTrackExcluded(std::string_view enemyId) {
    return enemyId == "fursuit_dummy" || enemyId == "gladowos";
}

inline int toxicTrack(const GameState& state) {
    return state.getFlag(std::string(TOXIC_TRACK_FLAG), 0);
}

// Returns true when this kill counted toward the toxic track.
inline bool toxicTrackOnKill(GameState& state, std::string_view enemyId) {
    if (toxicTrackExcluded(enemyId)) return false;

    const int track = toxicTrack(state);
    if (track == 2 || (track != 0 && track != 1)) return false;
    if (track == 0) state.setFlag(std::string(TOXIC_TRACK_FLAG), 1);
    state.incFlag(std::string(TOXIC_KILLS_FLAG));
    return true;
}

// Returns true when this spare permanently ended (or pre-empted) the track.
inline bool toxicTrackOnSpare(GameState& state, std::string_view enemyId) {
    if (toxicTrackExcluded(enemyId) || toxicTrack(state) == 2) return false;
    state.setFlag(std::string(TOXIC_TRACK_FLAG), 2);
    return true;
}

inline std::string_view mercyLabel(int track) {
    return track == 1 ? "Spare" : "Yiff";
}

inline std::string_view mercyLabel(const GameState& state) {
    return mercyLabel(toxicTrack(state));
}

// AUD-004 (LORE C7-002): while the toxic track runs, the Ruins music plays in slow motion
inline constexpr float TOXIC_RUINS_MUSIC_PITCH = 0.75f;

inline float overworldMusicPitch(int track, std::string_view roomId) {
    return (track == 1 && roomId.substr(0, 6) == "ruins_") ? TOXIC_RUINS_MUSIC_PITCH : 1.0f;
}

inline bool usesRegularYiffNarration(std::string_view enemyId) {
    return !toxicTrackExcluded(enemyId) && enemyId != "barky" && enemyId != "furiell";
}

struct YiffNarration {
    std::size_t index = 0;
    std::string text;
};

// Pure selection helper: a repeated candidate advances to the next variation.
inline YiffNarration yiffNarration(std::string_view enemyName,
                                   std::size_t candidate,
                                   int previousIndex = -1) {
    constexpr std::size_t variationCount = 3;
    std::size_t index = candidate % variationCount;
    if (previousIndex >= 0 && index == static_cast<std::size_t>(previousIndex)) {
        index = (index + 1) % variationCount;
    }

    const std::string name(enemyName);
    const std::array<std::string, variationCount> lines{
        "* You yiffed " + name + ". The narrator is not elaborating.",
        "* " + name + " leaves, flustered.",
        "* You yiffed " + name + ". Nobody will speak of this."
    };
    return { index, lines[index] };
}

} // namespace nundertale
