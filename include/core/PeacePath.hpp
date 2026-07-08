#pragma once

#include <algorithm>
#include <cstddef>
#include <string>
#include <string_view>
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
};

struct PeacePathDef {
    int goal = PEACE_GOAL_MIN;
    std::vector<PeaceActRule> rules;
    std::vector<std::string> stepLines;  // escalating lines, indexed by the step just reached (1-based)
    bool furrierPath = false;            // "Accept Hug" also gives FUR
    int furPerHug = 0;
    // LORE §8.11: on the toxic track enemies need more ACTs before they can be spared (killfeed).
    int toxicExtraSteps = 0;
};

struct PeacePathState {
    int progress = 0;
    int hugs = 0;
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
    const int wanted = std::max(0, rule->progress);
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

} // namespace nundertale
