#pragma once

#include <algorithm>
#include <cstddef>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace nundertale {

// ACT-008 (LORE §8.12): mini-bosses and bosses take 4-8 peaceful ACTs toward a goal before MERCY works.
// Regular enemies keep their 1-3 ACTs and do not use this model.
inline constexpr int PEACE_GOAL_MIN = 4;
inline constexpr int PEACE_GOAL_MAX = 8;

struct PeaceActRule {
    std::string act;
    int progress = 0;           // steps added (0 = the ACT is known but gives no progress, e.g. Insult)
    std::string line;           // shown when the ACT is used; empty = use PeacePathDef::stepLines
    int requiresProgress = 0;   // the ACT only counts once this many steps are done (Furiell: Promise to Visit)
    bool skipsAttack = false;   // the enemy skips its next attack (Insult, Debate Lore, Laser)
    int maxUses = 0;            // the ACT gives progress at most this many times (Comment on Tail x4); 0 = unlimited
};

struct PeacePathDef {
    int goal = PEACE_GOAL_MIN;
    std::vector<PeaceActRule> rules;
    std::vector<std::string> stepLines;  // escalating lines, indexed by the step just reached (1-based)
    // Flavor lead-ins for the next turn, indexed by the step just reached (1-based). The remaining count
    // is appended by peaceFlavorLine(), so the counting stays visible on the toxic track too.
    std::vector<std::string> stepFlavor;
    bool furrierPath = false;            // "Accept Hug" also gives FUR
    int furPerHug = 0;
    // LORE §8.11: on the toxic track enemies need more ACTs before they can be spared (killfeed).
    int toxicExtraSteps = 0;
};

struct PeacePathState {
    int progress = 0;
    int hugs = 0;
    std::vector<std::pair<std::string, int>> counted;  // ACT -> times it gave progress (for maxUses)
};

struct PeaceStepResult {
    bool known = false;          // the ACT is part of the peace path and its requirement was met
    int progressAdded = 0;
    std::string line;
    int furGained = 0;
    bool skipsAttack = false;
    bool goalReached = false;    // true only on the step that first reaches the goal
};

inline constexpr std::string_view PEACE_HUG_ACT = "Accept Hug";

// The authored goal is clamped to 4..8 first; on the toxic track the def's toxicExtraSteps are added
// afterwards (never negative), so the toxic goal may exceed 8.
inline int peaceGoal(const PeacePathDef& def, bool toxicTrack) {
    const int base = std::clamp(def.goal, PEACE_GOAL_MIN, PEACE_GOAL_MAX);
    return toxicTrack ? base + std::max(0, def.toxicExtraSteps) : base;
}

inline bool peaceGoalMet(const PeacePathDef& def, const PeacePathState& state, bool toxicTrack) {
    return state.progress >= peaceGoal(def, toxicTrack);
}

// MERCY (Yiff / Spare) only works once the goal has been reached.
inline bool peaceMercyAllowed(const PeacePathDef& def, const PeacePathState& state, bool toxicTrack) {
    return peaceGoalMet(def, state, toxicTrack);
}

inline PeaceStepResult peaceApplyAct(const PeacePathDef& def, PeacePathState& state,
                                     std::string_view act, bool toxicTrack) {
    PeaceStepResult result;
    const PeaceActRule* rule = nullptr;
    for (const PeaceActRule& candidate : def.rules) {
        if (candidate.act == act) {
            rule = &candidate;
            break;
        }
    }
    if (!rule || state.progress < rule->requiresProgress) return result;

    const int goal = peaceGoal(def, toxicTrack);
    const int before = state.progress;
    int wanted = std::max(0, rule->progress);
    if (wanted > 0 && rule->maxUses > 0) {
        auto it = std::find_if(state.counted.begin(), state.counted.end(),
                               [&](const auto& entry) { return entry.first == act; });
        if (it == state.counted.end()) {
            state.counted.emplace_back(std::string(act), 0);
            it = state.counted.end() - 1;
        }
        if (it->second >= rule->maxUses) wanted = 0;
        else ++it->second;
    }
    state.progress = std::min(goal, before + wanted);  // a progress already past the goal is left alone
    if (state.progress < before) state.progress = before;

    result.known = true;
    result.progressAdded = state.progress - before;
    result.skipsAttack = rule->skipsAttack;
    result.goalReached = before < goal && state.progress >= goal;

    if (!rule->line.empty()) {
        result.line = rule->line;
    } else if (result.progressAdded > 0 && !def.stepLines.empty()) {
        const std::size_t step = static_cast<std::size_t>(state.progress - 1);
        result.line = def.stepLines[std::min(step, def.stepLines.size() - 1)];
    }

    if (def.furrierPath && act == PEACE_HUG_ACT) {
        result.furGained = std::max(0, def.furPerHug);
        ++state.hugs;
    }
    return result;
}

inline std::string peaceNumberWord(int n) {
    static const char* const words[] = { "No", "One", "Two", "Three", "Four", "Five", "Six", "Seven", "Eight",
                                         "Nine", "Ten", "Eleven", "Twelve" };
    return (n >= 0 && n <= 12) ? words[n] : std::to_string(n);
}

// "<lead-in> Four to go." for the turn after a step; empty when no step was taken, no lead-in exists or the
// goal is met (the spareable flavor pool takes over then).
inline std::string peaceFlavorLine(const PeacePathDef& def, const PeacePathState& state, bool toxicTrack) {
    if (state.progress <= 0 || def.stepFlavor.empty() || peaceGoalMet(def, state, toxicTrack)) return {};
    const std::size_t step = static_cast<std::size_t>(state.progress - 1);
    const std::string& lead = def.stepFlavor[std::min(step, def.stepFlavor.size() - 1)];
    return lead + " " + peaceNumberWord(peaceGoal(def, toxicTrack) - state.progress) + " to go.";
}

} // namespace nundertale
