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
};

inline FurChange applyFur(int& value, int& stageMax, int delta, float armorMult = 1.0f) {
    value = std::clamp(value, 0, FUR_MAX);
    stageMax = std::clamp(stageMax, 0, static_cast<int>(FurStage::Mirror));

    FurChange change;
    change.before = value;
    change.oldStage = furStageFor(change.before);

    int appliedDelta = delta;
    if (delta > 0) {
        appliedDelta = static_cast<int>(std::lround(static_cast<float>(delta) * armorMult));
    }

    value = std::clamp(change.before + appliedDelta, 0, FUR_MAX);
    change.after = value;
    change.newStage = furStageFor(change.after);
    change.stageUp = change.newStage > change.oldStage;
    change.reachedMirror = change.after == FUR_MAX && change.before < FUR_MAX;
    stageMax = std::max(stageMax, static_cast<int>(change.newStage));
    return change;
}

} // namespace nundertale
