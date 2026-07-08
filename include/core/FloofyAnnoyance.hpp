#pragma once

#include <algorithm>
#include <string>
#include <unordered_map>

namespace nundertale {

// LORE §19.0 "Which fight" / "Retry after a WELCOME HOME": pure helpers, no engine calls.
// Flags: ruins.floofy.pokes / choice1 / choice2 / dodges / annoyed / fight / deaths.
inline constexpr int FLOOFY_POKES_ANNOY = 3;      // pokes in R1 that count as annoying
inline constexpr int FLOOFY_DODGES_ANNOY = 2;     // dodged tutorial petals that count as annoying
inline constexpr int FLOOFY_ANNOYED_BASS = 2;     // annoyance needed for the Bass Shake variant
inline constexpr int FLOOFY_FIGHT_PETALS = 1;
inline constexpr int FLOOFY_FIGHT_BASS = 2;
inline constexpr int FLOOFY_FURIELL_TURN_DEFAULT = 6;
inline constexpr int FLOOFY_FURIELL_TURN_MIN = 3;
inline constexpr int FLOOFY_FURIELL_FREE_DEATHS = 2;  // deaths 0..2 keep the default turn

struct FloofyAnnoyanceInput {
    int pokes = 0;
    bool choice1Rude = false;
    bool choice2Rude = false;
    int dodges = 0;
};

// Each source adds at most +1, so the total is 0..4.
inline int floofyAnnoyance(const FloofyAnnoyanceInput& in) {
    int n = 0;
    if (in.pokes >= FLOOFY_POKES_ANNOY) ++n;
    if (in.choice1Rude) ++n;
    if (in.choice2Rude) ++n;
    if (in.dodges >= FLOOFY_DODGES_ANNOY) ++n;
    return n;
}

// 2 (Bass Shake) when annoyed >= 2, otherwise 1 (petals).
inline int floofyFightVariant(int annoyed) {
    return annoyed >= FLOOFY_ANNOYED_BASS ? FLOOFY_FIGHT_BASS : FLOOFY_FIGHT_PETALS;
}

// Locks the variant into ruins.floofy.fight on the first pick; later calls (retries) reuse it.
inline int floofyLockFight(std::unordered_map<std::string, int>& flags, int annoyed) {
    auto it = flags.find("ruins.floofy.fight");
    if (it != flags.end() && it->second != 0) return it->second;
    const int v = floofyFightVariant(annoyed);
    flags["ruins.floofy.fight"] = v;
    return v;
}

// The turn on which Furiell's spray rescue arrives: 6 for deaths < 3, then one turn earlier per death, min 3.
inline int floofyFuriellTurn(int deaths) {
    if (deaths <= FLOOFY_FURIELL_FREE_DEATHS) return FLOOFY_FURIELL_TURN_DEFAULT;
    return std::max(FLOOFY_FURIELL_TURN_MIN,
                    FLOOFY_FURIELL_TURN_DEFAULT - (deaths - FLOOFY_FURIELL_FREE_DEATHS));
}

}  // namespace nundertale
