#pragma once

#include <algorithm>
#include <cmath>

namespace nundertale {

enum class FurStage {
    Frog = 0,
    Ears,
    Paws,
    Tail,
    Snout,
    Mirror
};

inline constexpr int FUR_MAX = 100;
inline constexpr int FUR_TOXIC_MAX = 99;
inline constexpr int DUCT_TAPE_FUR_LOSS = -15;

inline constexpr const char* DUCT_TAPE_EMPTY_TEXT = "* You tape... nothing. There's no fluff to remove.";
inline constexpr const char* DUCT_TAPE_DEFAULT_TEXT = "* You aggressively taped down your sprouting fur.\n* Ouch! FUR reduced by 15!";
inline constexpr const char* TOXIC_CAP_FLAVOR_MSG = "* Your vitriol burns the fluff right off.";

inline FurStage furStageFor(int value) {
    const int clamped = std::clamp(value, 0, FUR_MAX);
    if (clamped == FUR_MAX) {
        return FurStage::Mirror;
    }
    return static_cast<FurStage>(clamped / 20);
}

inline const char* furStageName(FurStage stage) {
    switch (stage) {
        case FurStage::Frog: return "Frog";
        case FurStage::Ears: return "Ears";
        case FurStage::Paws: return "Paws";
        case FurStage::Tail: return "Tail";
        case FurStage::Snout: return "Snout";
        case FurStage::Mirror: return "Mirror";
    }
    return "Frog";
}

struct FurChange {
    int before = 0;
    int after = 0;
    FurStage oldStage{};
    FurStage newStage{};
    bool stageUp = false;
    bool reachedMirror = false;
    bool capped = false;
};

inline FurChange applyFurCapped(int& value, int& stageMax, int delta, bool toxicRoute, float mult = 1.0f) {
    value = std::clamp(value, 0, FUR_MAX);
    stageMax = std::clamp(stageMax, 0, static_cast<int>(FurStage::Mirror));

    FurChange change;
    change.before = value;
    change.oldStage = furStageFor(change.before);

    int appliedDelta = delta;
    if (delta > 0) {
        appliedDelta = static_cast<int>(std::lround(static_cast<float>(delta) * mult));
    }

    const int maxVal = toxicRoute ? FUR_TOXIC_MAX : FUR_MAX;
    const int target = change.before + appliedDelta;
    if (toxicRoute && delta > 0 && target > FUR_TOXIC_MAX) {
        change.capped = true;
    }

    value = std::clamp(target, 0, maxVal);
    change.after = value;
    change.newStage = furStageFor(change.after);
    change.stageUp = change.newStage > change.oldStage;
    change.reachedMirror = (!toxicRoute) && (change.after == FUR_MAX && change.before < FUR_MAX);
    stageMax = std::max(stageMax, static_cast<int>(change.newStage));
    return change;
}

inline FurChange applyFur(int& value, int& stageMax, int delta, float armorMult = 1.0f) {
    return applyFurCapped(value, stageMax, delta, false, armorMult);
}

} // namespace nundertale
