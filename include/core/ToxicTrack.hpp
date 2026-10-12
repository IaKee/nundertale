#pragma once

#include "core/GameContext.hpp"
#include <algorithm>
#include <array>
#include <cstddef>
#include <string>
#include <string_view>

namespace nundertale {

inline constexpr std::string_view TOXIC_TRACK_FLAG = "toxic.track";
inline constexpr std::string_view TOXIC_KILLS_FLAG = "toxic.kills";
inline constexpr std::string_view TOXIC_STREAK_FLAG = "toxic.streak";
inline constexpr std::string_view TOXIC_FADE_ROOMS_FLAG = "toxic.fade_rooms";
inline constexpr int TOXIC_FADE_ROOM_COUNT = 4;

inline bool toxicTrackExcluded(std::string_view enemyId) {
    return enemyId == "fursuit_dummy" || enemyId == "gladowos";
}

inline int toxicTrack(const GameState& state) {
    return state.getFlag(std::string(TOXIC_TRACK_FLAG), 0);
}

// The overworld PK marker identifies the currently active toxic route only.
// It disappears as soon as a spare ends the route (track == 2), along with
// being absent on neutral saves (track == 0).
inline bool toxicPkSkullVisible(const GameState& state) {
    return toxicTrack(state) == 1;
}

inline int toxicKills(const GameState& state) {
    return state.getFlag(std::string(TOXIC_KILLS_FLAG), 0);
}

inline int toxicStreak(const GameState& state) {
    return state.getFlag(std::string(TOXIC_STREAK_FLAG), 0);
}

inline int toxicFadeRoomsRemaining(const GameState& state) {
    return std::clamp(state.getFlag(std::string(TOXIC_FADE_ROOMS_FLAG), 0),
                      0, TOXIC_FADE_ROOM_COUNT);
}

inline int toxicCorruptionStage(int kills) {
    if (kills < 1) return 0;
    if (kills < 9) return 1;
    if (kills < 16) return 2;
    if (kills < 22) return 3;
    return 4;
}

inline int toxicCorruptionStage(const GameState& state) {
    return toxicCorruptionStage(toxicKills(state));
}

// Shared 0..1 weight for music and pacing. The active track is fully applied;
// after it ends the weight loses one equal step on each room entry.
inline float toxicEffectBlend(const GameState& state) {
    if (toxicTrack(state) == 1) return 1.0f;
    if (toxicTrack(state) != 2) return 0.0f;
    return static_cast<float>(toxicFadeRoomsRemaining(state)) /
           static_cast<float>(TOXIC_FADE_ROOM_COUNT);
}

// Fractional corruption stage for visual consumers. This starts at the last
// kill-derived stage when the track ends and reaches exactly zero after the fade.
inline float toxicFadeValue(const GameState& state) {
    return static_cast<float>(toxicCorruptionStage(state)) * toxicEffectBlend(state);
}

struct ToxicFadeState {
    float value = 0.0f;
    float blend = 0.0f;
    int roomsRemaining = 0;
    bool fading = false;
};

inline ToxicFadeState toxicFadeState(const GameState& state) {
    const int roomsRemaining = toxicFadeRoomsRemaining(state);
    return {
        toxicFadeValue(state),
        toxicEffectBlend(state),
        roomsRemaining,
        toxicTrack(state) == 2 && roomsRemaining > 0
    };
}

struct ToxicHudState {
    int kills = 0;
    int streak = 0;
    int stage = 0;
    bool active = false;
    float corruption = 0.0f;
};

inline ToxicHudState toxicHudState(const GameState& state) {
    return {
        toxicKills(state),
        toxicStreak(state),
        toxicCorruptionStage(state),
        toxicTrack(state) == 1,
        toxicFadeValue(state)
    };
}

// Returns true when this kill counted toward the toxic track.
inline bool toxicTrackOnKill(GameState& state, std::string_view enemyId) {
    if (toxicTrackExcluded(enemyId)) return false;

    const int track = toxicTrack(state);
    if (track == 2 || (track != 0 && track != 1)) return false;
    if (track == 0) {
        state.setFlag(std::string(TOXIC_TRACK_FLAG), 1);
        state.setFlag(std::string(TOXIC_FADE_ROOMS_FLAG), 0);
    }
    state.incFlag(std::string(TOXIC_KILLS_FLAG));
    state.incFlag(std::string(TOXIC_STREAK_FLAG));
    return true;
}

inline void toxicOnSave(GameState& state) {
    state.setFlag(std::string(TOXIC_STREAK_FLAG), 0);
}

// Returns true when this spare permanently ended (or pre-empted) the track.
inline bool toxicTrackOnSpare(GameState& state, std::string_view enemyId) {
    if (toxicTrackExcluded(enemyId) || toxicTrack(state) == 2) return false;
    state.setFlag(std::string(TOXIC_TRACK_FLAG), 2);
    const int fadeRooms = toxicCorruptionStage(state) > 0 ? TOXIC_FADE_ROOM_COUNT : 0;
    state.setFlag(std::string(TOXIC_FADE_ROOMS_FLAG), fadeRooms);
    return true;
}

// Call once after entering a different room. Returns true while a fade step was consumed.
inline bool toxicTrackOnRoomEntry(GameState& state) {
    if (toxicTrack(state) != 2) return false;
    const int roomsRemaining = toxicFadeRoomsRemaining(state);
    if (roomsRemaining <= 0) {
        state.setFlag(std::string(TOXIC_FADE_ROOMS_FLAG), 0);
        return false;
    }
    state.setFlag(std::string(TOXIC_FADE_ROOMS_FLAG), roomsRemaining - 1);
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

inline float overworldMusicPitch(const GameState& state, std::string_view roomId) {
    if (roomId.substr(0, 6) != "ruins_") return 1.0f;
    const float blend = toxicEffectBlend(state);
    return 1.0f - (1.0f - TOXIC_RUINS_MUSIC_PITCH) * blend;
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
