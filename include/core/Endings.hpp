#pragma once

#include "core/GameContext.hpp"
#include "core/RaidClock.hpp"
#include "core/ToxicTrack.hpp"

#include <string>
#include <string_view>
#include <vector>

namespace nundertale {

// END-001 / END-006 (LORE 22.0-22.4, 28). Values of EndingType are the `ending.type` flag values.
enum class EndingType : int { None = 0, Gamer = 1, CleanRun = 2, Toxic = 3, Furry = 4 };

inline constexpr std::string_view ENDING_TYPE_FLAG = "ending.type";
inline constexpr std::string_view ROUTE_FURRY_FLAG = "route.furry";
inline constexpr std::string_view KILLS_TOTAL_FLAG = "kills.total";

// Pure route decision (LORE 22.0), by precedence:
//   1. Furry: `route.furry` is set (Mirror accepted). Beats everything.
//   2. Toxic: the toxic track is still running (`toxic.track == 1`: started by a kill, never ended
//      by a spare/yiff; track 2 = ended for good) and at least one track kill is recorded.
//      The per-area caps and the boss kills are not re-checked here: they are the route
//      predicate of PRG-031, which this router does not own.
//   3. Clean Run: Gamer with zero kills (`GameState::killsTotal == 0` and `kills.total` flag == 0).
//   4. Gamer: everything else.
inline EndingType decideEnding(const GameState& state) {
    if (state.getFlag(std::string(ROUTE_FURRY_FLAG), 0) != 0) return EndingType::Furry;
    if (toxicTrack(state) == 1 && toxicKills(state) > 0) return EndingType::Toxic;
    const bool noKills = state.killsTotal <= 0 && state.getFlag(std::string(KILLS_TOTAL_FLAG), 0) <= 0;
    return noKills ? EndingType::CleanRun : EndingType::Gamer;
}

inline int endingTypeValue(EndingType type) {
    return static_cast<int>(type);
}

inline EndingType endingTypeFromValue(int value) {
    switch (value) {
        case 1: return EndingType::Gamer;
        case 2: return EndingType::CleanRun;
        case 3: return EndingType::Toxic;
        case 4: return EndingType::Furry;
        default: return EndingType::None;
    }
}

// Writes `ending.type`. `meta.last_ending` (meta.json) must be fed with endingTypeValue(type)
// by the SaveService/MET-005 caller.
inline void recordEnding(GameState& state, EndingType type) {
    state.setFlag(std::string(ENDING_TYPE_FLAG), endingTypeValue(type));
}

inline EndingType recordedEnding(const GameState& state) {
    return endingTypeFromValue(state.getFlag(std::string(ENDING_TYPE_FLAG), 0));
}

// LORE 22.4. `kicked` = the squad clock ran out; `speedrun` = more than 10:00 left (RaidClock.hpp).
// The speedrun lines are appended exactly once and only when the player was not kicked.
inline std::vector<std::string> squadClockEndingLines(EndingType type, bool kicked, bool speedrun) {
    std::vector<std::string> lines;
    switch (type) {
        case EndingType::Gamer:
            if (kicked) {
                lines = { "* You queued without the squad.", "* (Whatever.)" };
            } else {
                lines = { "* You play with the squad. Nobody says a word." };
            }
            break;
        case EndingType::CleanRun:
            if (kicked) {
                lines = {
                    "* The squad queued without you.",
                    "BARKY: new squad. two members. we're called the Shadow Fang Pack.",
                    "* (You're in a wolf squad now.)"
                };
            } else {
                lines = {
                    "SQUAD: who's the wolf",
                    "SQUAD: bro is running a FURRY skin??",
                    "BARKY: I'm... I'm the carry. I think I always was.",
                    "* (He's actually good.)",
                    "* (He gets 14 eliminations. You don't mention the zipper.)"
                };
            }
            break;
        case EndingType::Toxic:
            lines = { "* VICTORY ROYALE.", "* (Nobody is really there.)" };
            break;
        case EndingType::Furry:
            lines = { "* The squad dropped on you.", "* (Nobody is really there.)" };
            break;
        case EndingType::None:
            return lines;
    }
    if (speedrun && !kicked) {
        lines.push_back("* Your squad asks how you got back so fast.");
        lines.push_back("* (Skill issue.)");
    }
    return lines;
}

inline std::vector<std::string> squadClockEndingLines(EndingType type, const RaidClock& clock) {
    const RaidEndingQuery query = raidEndingQuery(clock);
    return squadClockEndingLines(type, query.expired, query.speedrun);
}

} // namespace nundertale
