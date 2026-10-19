#pragma once

#include <algorithm>

namespace nundertale {

// OVW-022 (LORE §30.4): examining a bloodstain replays a translucent ghost, an ex-gamer, walking up to a
// furry, accepting a hug, and the red soul leaving the chest. No text, then `* (They said yes.)`.
// The replay itself is data (data/cutscenes/r8_bloodstain.json); these constants are its timeline.
constexpr float kBloodstainLeadSeconds = 0.5f;     // the ghost fades in (flag on) and stands
constexpr float kBloodstainWalkDistance = 120.0f;  // pixels walked toward the furry
constexpr float kBloodstainWalkSpeed = 60.0f;      // pixels per second
constexpr float kBloodstainEmbraceSeconds = 1.5f;  // the hug (the furry steps in, then holds)
constexpr float kBloodstainSoulDistance = 45.0f;   // pixels the soul rises
constexpr float kBloodstainSoulSpeed = 30.0f;      // pixels per second
constexpr float kBloodstainGhostAlpha = 0.45f;     // translucent: never fully opaque

enum class BloodstainPhase { Lead, Walk, Embrace, Soul, Done };

constexpr float bloodstainWalkSeconds() { return kBloodstainWalkDistance / kBloodstainWalkSpeed; }
constexpr float bloodstainSoulSeconds() { return kBloodstainSoulDistance / kBloodstainSoulSpeed; }

constexpr float bloodstainReplaySeconds() {
    return kBloodstainLeadSeconds + bloodstainWalkSeconds() + kBloodstainEmbraceSeconds +
           bloodstainSoulSeconds();
}

inline BloodstainPhase bloodstainPhaseAt(float seconds) {
    float t = seconds;
    if (t < kBloodstainLeadSeconds) return BloodstainPhase::Lead;
    t -= kBloodstainLeadSeconds;
    if (t < bloodstainWalkSeconds()) return BloodstainPhase::Walk;
    t -= bloodstainWalkSeconds();
    if (t < kBloodstainEmbraceSeconds) return BloodstainPhase::Embrace;
    t -= kBloodstainEmbraceSeconds;
    if (t < bloodstainSoulSeconds()) return BloodstainPhase::Soul;
    return BloodstainPhase::Done;
}

// Distance walked from the start (0 before the walk, full distance from the embrace on).
inline float bloodstainGhostWalked(float seconds) {
    const float walking = seconds - kBloodstainLeadSeconds;
    return std::clamp(walking * kBloodstainWalkSpeed, 0.0f, kBloodstainWalkDistance);
}

// 0 until the soul leaves the chest, then 0..1 up to the end of the replay.
inline float bloodstainSoulProgress(float seconds) {
    const float start = kBloodstainLeadSeconds + bloodstainWalkSeconds() + kBloodstainEmbraceSeconds;
    return std::clamp((seconds - start) / bloodstainSoulSeconds(), 0.0f, 1.0f);
}

inline bool bloodstainGhostVisible(float seconds) {
    return seconds >= 0.0f && bloodstainPhaseAt(seconds) != BloodstainPhase::Done;
}

}  // namespace nundertale
