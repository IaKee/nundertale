#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <cstddef>
#include "ActUses.hpp"

namespace nundertale {

struct ActDef {
    std::string id;
    std::string label;
    std::string text;
    std::string response;
    bool spareable = false;
    bool skipAttack = false;
    bool stopsBeat = false; // BSK: no more enemy attack turns this battle + music ducked
    bool flees = false;
    int mercy = 0;
    std::string requiresItem;
    int requiresFurMin = 0;
    std::string requiresFlag;
    int minUses = 1;
    int atkDelta = 0;
    bool atkDeltaOnce = false;
    int defDelta = 0;
    int freezeTurns = 0;
    std::string repeatText;     // shown from the 2nd use on (e.g. Cringe Check), "" = reuse text
    std::string repeatResponse; // enemy line from the 2nd use on, "" = reuse response
    std::vector<std::string> useTexts;     // per-use text (use N -> [N-1], last one repeats); wins over text/repeatText
    std::vector<std::string> useResponses; // per-use enemy line, same rule
    int fur = 0;                // FUR added when the effect applies (ACT-005 Accept Hug)
    bool spareNow = false;      // spares on the spot, without MERCY (ACT-005)
    std::string setFlag;
    bool healNextAttack = false;
};

// Text and enemy line for the Nth use of an ACT (1-based).
inline const std::string& actTextForUse(const ActDef& act, int useCount) {
    if (!act.useTexts.empty()) {
        return act.useTexts[actUseIndex(useCount, act.useTexts.size())];
    }
    return (useCount >= 2 && !act.repeatText.empty()) ? act.repeatText : act.text;
}

inline const std::string& actResponseForUse(const ActDef& act, int useCount) {
    if (!act.useResponses.empty()) {
        return act.useResponses[actUseIndex(useCount, act.useResponses.size())];
    }
    return (useCount >= 2 && !act.repeatResponse.empty()) ? act.repeatResponse : act.response;
}

inline bool actEffectApplies(int useCount, int minUses) {
    return minUses <= 1 || useCount >= minUses;
}

inline int clampStat(int value, int delta) {
    return std::max(0, value + delta);
}

// Pure gate helper in ActGate.hpp (item present/absent, flag 0/1, both).
// Checks whether an item gate requirement is satisfied by the player's inventory
// or associated pickup flag (e.g. Toy Laser Pointer / ruins.laser.taken).
inline bool actItemGatePassed(const std::string& requiresItem,
                             const std::vector<std::string>& inventory,
                             const std::function<bool(const std::string&)>& hasFlag = nullptr) {
    if (requiresItem.empty()) {
        return true;
    }

    if (std::find(inventory.begin(), inventory.end(), requiresItem) != inventory.end()) {
        return true;
    }

    // Toy Laser Pointer (laser_pointer in items.json, flag ruins.laser.taken in R8)
    if (requiresItem == "Toy Laser Pointer" || requiresItem == "laser_pointer" || requiresItem == "ToyLaser") {
        for (const auto& item : inventory) {
            if (item == "Toy Laser Pointer" || item == "laser_pointer" || item == "ToyLaser") {
                return true;
            }
        }
        if (hasFlag && (hasFlag("ruins.laser.taken") || hasFlag("laser_pointer"))) {
            return true;
        }
    }

    return false;
}

// Checks whether a flag gate requirement is satisfied via predicate
inline bool actFlagGatePassed(const std::string& requiresFlag,
                             const std::function<bool(const std::string&)>& hasFlag) {
    if (requiresFlag.empty()) {
        return true;
    }
    return hasFlag && hasFlag(requiresFlag);
}

// Checks whether a flag gate requirement is satisfied via integer flag value (0/1)
inline bool actFlagGatePassed(const std::string& requiresFlag, int flagValue) {
    if (requiresFlag.empty()) {
        return true;
    }
    return flagValue != 0;
}

// Checks both item requirement and flag requirement via predicate
inline bool actGatePassed(const std::string& requiresItem,
                          const std::string& requiresFlag,
                          const std::vector<std::string>& inventory,
                          const std::function<bool(const std::string&)>& hasFlag = nullptr) {
    if (!actItemGatePassed(requiresItem, inventory, hasFlag)) {
        return false;
    }
    if (!actFlagGatePassed(requiresFlag, hasFlag)) {
        return false;
    }
    return true;
}

// Checks both item requirement and flag requirement via integer flag value (0/1)
inline bool actGatePassed(const std::string& requiresItem,
                          const std::string& requiresFlag,
                          const std::vector<std::string>& inventory,
                          int flagValue) {
    auto flagCheck = [flagValue, &requiresFlag](const std::string& f) {
        if (f == requiresFlag) {
            return flagValue != 0;
        }
        return false;
    };
    if (!actItemGatePassed(requiresItem, inventory, flagCheck)) {
        return false;
    }
    return actFlagGatePassed(requiresFlag, flagValue);
}

// Overload for ActDef
inline bool actGatePassed(const ActDef& act,
                          const std::vector<std::string>& inventory,
                          const std::function<bool(const std::string&)>& hasFlag = nullptr) {
    return actGatePassed(act.requiresItem, act.requiresFlag, inventory, hasFlag);
}

inline bool actGatePassed(const ActDef& act,
                          const std::vector<std::string>& inventory,
                          int flagValue) {
    return actGatePassed(act.requiresItem, act.requiresFlag, inventory, flagValue);
}

// Availability helper incorporating FUR requirement and item/flag gates
inline bool actAvailable(const ActDef& act,
                         const std::vector<std::string>& inventory,
                         int fur,
                         const std::function<bool(const std::string&)>& hasFlag) {
    if (fur < act.requiresFurMin) {
        return false;
    }
    return actGatePassed(act, inventory, hasFlag);
}

inline bool isActAvailable(const ActDef& act,
                           const std::vector<std::string>& inventory,
                           int fur,
                           const std::function<bool(const std::string&)>& hasFlag) {
    return actAvailable(act, inventory, fur, hasFlag);
}

} // namespace nundertale
