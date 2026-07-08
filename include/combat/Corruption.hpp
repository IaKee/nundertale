#pragma once

#include "core/ToxicTrack.hpp"

#include <algorithm>

namespace nundertale {

// LORE §8.9 "Escalation of the look": pure mapping kills -> battle-screen
// corruption. Stage numbers come from toxicCorruptionStage (ToxicTrack.hpp).
inline constexpr int CORRUPTION_MAX_KILLS = 25;

// Readability guarantee: no overlay may ever exceed this alpha, text keeps at
// least this contrast ratio against its backdrop, and the HUD is never hidden.
inline constexpr float CORRUPTION_MAX_OVERLAY_ALPHA = 0.45f;
inline constexpr float CORRUPTION_MIN_TEXT_CONTRAST = 4.5f;

struct CorruptionLook {
    int stage = 0;            // 0 plain .. 4 full
    float gridFlicker = 0.0f; // 0..1 red flicker at the grid edges
    float gridDarkness = 0.0f; // 0 = green/plain, 1 = dark red grid
    int scanlines = 0;        // glitch scanline count
    float glitch = 0.0f;      // 0..1 general glitch amount
    bool mercyGray = false;
    bool mercyCracked = false;
    bool fightGlow = false;   // FIGHT button glows red
    bool full = false;        // stage 4: full corruption
    float overlayAlpha = 0.0f;
    float textContrast = 21.0f; // guaranteed minimum for any text
    bool hudHidden = false;     // never true
    bool mercyUsable = true;    // gray/cracked MERCY still works
};

inline CorruptionLook corruptionLook(int kills) {
    CorruptionLook look;
    look.stage = toxicCorruptionStage(std::clamp(kills, 0, CORRUPTION_MAX_KILLS));
    switch (look.stage) {
    case 1:
        look.gridFlicker = 0.25f;
        look.overlayAlpha = 0.10f;
        break;
    case 2:
        look.gridFlicker = 0.4f;
        look.gridDarkness = 0.6f;
        look.scanlines = 3;
        look.glitch = 0.15f;
        look.mercyGray = true;
        look.overlayAlpha = 0.20f;
        break;
    case 3:
        look.gridFlicker = 0.6f;
        look.gridDarkness = 0.8f;
        look.scanlines = 8;
        look.glitch = 0.5f;
        look.mercyGray = true;
        look.mercyCracked = true;
        look.fightGlow = true;
        look.overlayAlpha = 0.30f;
        break;
    case 4:
        look.gridFlicker = 1.0f;
        look.gridDarkness = 1.0f;
        look.scanlines = 14;
        look.glitch = 0.8f;
        look.mercyGray = true;
        look.mercyCracked = true;
        look.fightGlow = true;
        look.full = true;
        look.overlayAlpha = CORRUPTION_MAX_OVERLAY_ALPHA;
        break;
    default:
        break;
    }
    look.overlayAlpha = std::min(look.overlayAlpha, CORRUPTION_MAX_OVERLAY_ALPHA);
    look.textContrast = std::max(CORRUPTION_MIN_TEXT_CONTRAST, 21.0f - 16.0f * look.glitch);
    return look;
}

inline CorruptionLook corruptionLook(const GameState& state) {
    return corruptionLook(toxicKills(state));
}

} // namespace nundertale
