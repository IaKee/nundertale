#pragma once

#include "core/GameContext.hpp"
#include "core/ToxicTrack.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <deque>
#include <string>
#include <string_view>
#include <vector>

namespace nundertale {

// Constants for TOX-003
inline constexpr std::string_view OSRS_LEVEL_UP_LINE =
    "* Congratulations, you just advanced a Vitriol level.";
inline constexpr std::string_view DEFAULT_LEVEL_UP_LINE =
    "* Your LEVEL OF VITRIOL increased.";
inline constexpr std::string_view OSRS_LEVEL_UP_JINGLE_SFX =
    "assets/sfx/osrs_level_up.ogg";
inline constexpr std::string_view DEFAULT_LEVEL_UP_SFX =
    "assets/sfx/level_up.ogg";

inline constexpr int OSRS_LEVEL_UP_KILL_THRESHOLD = 9;

// Hitsplat kinds per OSRS / TOX-003
enum class HitsplatKind {
    None = 0,
    Damage, // Red splat with white number (normal hit)
    Miss,   // Blue splat with 0 (miss)
    MaxHit  // Max-hit splat / critical hit splat
};

// Hit result descriptor (input to determine splat or regular text)
enum class HitResultKind {
    Miss = 0,
    Hit,
    Crit
};

// Determines the hitsplat kind based on hit result
[[nodiscard]] constexpr HitsplatKind hitsplatKindFor(HitResultKind hit) noexcept {
    switch (hit) {
        case HitResultKind::Miss: return HitsplatKind::Miss;
        case HitResultKind::Hit:  return HitsplatKind::Damage;
        case HitResultKind::Crit: return HitsplatKind::MaxHit;
    }
    return HitsplatKind::None;
}

// Convenience overload taking damage amount and crit flag
[[nodiscard]] constexpr HitsplatKind hitsplatKindFor(int damage, bool isCrit, bool isMiss = false) noexcept {
    if (isMiss || damage <= 0) return HitsplatKind::Miss;
    if (isCrit) return HitsplatKind::MaxHit;
    return HitsplatKind::Damage;
}

// Check if OSRS hitsplats should be active (from the first kill on toxic track, i.e., toxic.kills >= 1 and toxic.track == 1)
[[nodiscard]] inline bool hitsplatsActive(const GameState& state) noexcept {
    return toxicTrack(state) == 1 && toxicKills(state) >= 1;
}

[[nodiscard]] constexpr bool hitsplatsActive(int kills, int track) noexcept {
    return track == 1 && kills >= 1;
}

// Pure data model for a single hitsplat
struct Hitsplat {
    HitsplatKind kind = HitsplatKind::None;
    int amount = 0;              // 0 for Miss, positive damage for Damage/MaxHit
    float x = 0.0f;
    float y = 0.0f;
    float timer = 0.0f;
    float lifetime = 1.2f;

    [[nodiscard]] bool active() const noexcept {
        return timer > 0.0f && kind != HitsplatKind::None;
    }

    [[nodiscard]] float progress() const noexcept {
        if (lifetime <= 0.0f) return 1.0f;
        return std::clamp(1.0f - (timer / lifetime), 0.0f, 1.0f);
    }

    [[nodiscard]] float alpha() const noexcept {
        if (!active()) return 0.0f;
        // Fade out in the last 25% of lifetime
        const float p = progress();
        if (p < 0.75f) return 1.0f;
        return std::clamp((1.0f - p) / 0.25f, 0.0f, 1.0f);
    }

    void update(float dt) noexcept {
        if (timer > 0.0f) {
            timer -= dt;
            if (timer <= 0.0f) {
                timer = 0.0f;
            }
        }
    }
};

// Pure factory for creating hitsplats
inline Hitsplat makeHitsplat(HitsplatKind kind, int amount, float x, float y, float lifetime = 1.2f) noexcept {
    Hitsplat s;
    s.kind = kind;
    s.amount = (kind == HitsplatKind::Miss) ? 0 : std::max(0, amount);
    s.x = x;
    s.y = y;
    s.lifetime = lifetime > 0.0f ? lifetime : 1.2f;
    s.timer = s.lifetime;
    return s;
}

// XP drop model: '+N' floating up the right edge on every EXP gain
struct XpDrop {
    int amount = 0;
    float x = 600.0f;           // Default near right edge (e.g. 640 virtual width)
    float startY = 320.0f;
    float currentY = 320.0f;
    float floatSpeed = 40.0f;   // pixels per second upward
    float timer = 0.0f;
    float lifetime = 1.5f;

    [[nodiscard]] bool active() const noexcept {
        return timer > 0.0f && amount > 0;
    }

    [[nodiscard]] float progress() const noexcept {
        if (lifetime <= 0.0f) return 1.0f;
        return std::clamp(1.0f - (timer / lifetime), 0.0f, 1.0f);
    }

    [[nodiscard]] float alpha() const noexcept {
        if (!active()) return 0.0f;
        const float p = progress();
        // Fully visible for first 60%, then fades out to 0
        if (p <= 0.6f) return 1.0f;
        return std::clamp((1.0f - p) / 0.4f, 0.0f, 1.0f);
    }

    [[nodiscard]] std::string text() const {
        return "+" + std::to_string(amount);
    }

    void update(float dt) noexcept {
        if (timer > 0.0f) {
            currentY -= floatSpeed * dt;
            timer -= dt;
            if (timer <= 0.0f) {
                timer = 0.0f;
            }
        }
    }
};

// Pure factory for creating XP drops
inline XpDrop makeXpDrop(int amount, float x = 600.0f, float startY = 320.0f,
                         float floatSpeed = 40.0f, float lifetime = 1.5f) noexcept {
    XpDrop drop;
    drop.amount = std::max(0, amount);
    drop.x = x;
    drop.startY = startY;
    drop.currentY = startY;
    drop.floatSpeed = floatSpeed;
    drop.lifetime = lifetime > 0.0f ? lifetime : 1.5f;
    drop.timer = drop.amount > 0 ? drop.lifetime : 0.0f;
    return drop;
}

// Check if XP drops should be active (on toxic track from 1st kill, or whenever toxic HUD is active)
[[nodiscard]] inline bool xpDropsActive(const GameState& state) noexcept {
    return toxicTrack(state) == 1 && toxicKills(state) >= 1;
}

[[nodiscard]] constexpr bool xpDropsActive(int kills, int track) noexcept {
    return track == 1 && kills >= 1;
}

// Manages collections of active hitsplats and XP drops
class ToxicCombatFxManager {
public:
    ToxicCombatFxManager() = default;

    void spawnHitsplat(HitsplatKind kind, int amount, float x, float y, float lifetime = 1.2f) {
        if (kind == HitsplatKind::None) return;
        m_hitsplats.push_back(makeHitsplat(kind, amount, x, y, lifetime));
    }

    void spawnHit(HitResultKind hit, int amount, float x, float y, float lifetime = 1.2f) {
        HitsplatKind k = hitsplatKindFor(hit);
        spawnHitsplat(k, amount, x, y, lifetime);
    }

    void spawnXpDrop(int amount, float x = 600.0f, float startY = 320.0f,
                     float floatSpeed = 40.0f, float lifetime = 1.5f) {
        if (amount <= 0) return;
        m_xpDrops.push_back(makeXpDrop(amount, x, startY, floatSpeed, lifetime));
    }

    void update(float dt) {
        for (auto& s : m_hitsplats) {
            s.update(dt);
        }
        for (auto& d : m_xpDrops) {
            d.update(dt);
        }

        // Cleanup expired
        m_hitsplats.erase(
            std::remove_if(m_hitsplats.begin(), m_hitsplats.end(),
                           [](const Hitsplat& s) { return !s.active(); }),
            m_hitsplats.end());

        m_xpDrops.erase(
            std::remove_if(m_xpDrops.begin(), m_xpDrops.end(),
                           [](const XpDrop& d) { return !d.active(); }),
            m_xpDrops.end());
    }

    void clear() {
        m_hitsplats.clear();
        m_xpDrops.clear();
    }

    [[nodiscard]] const std::vector<Hitsplat>& hitsplats() const noexcept { return m_hitsplats; }
    [[nodiscard]] const std::vector<XpDrop>& xpDrops() const noexcept { return m_xpDrops; }
    [[nodiscard]] size_t hitsplatCount() const noexcept { return m_hitsplats.size(); }
    [[nodiscard]] size_t xpDropCount() const noexcept { return m_xpDrops.size(); }

private:
    std::vector<Hitsplat> m_hitsplats;
    std::vector<XpDrop> m_xpDrops;
};

// Level-up line logic:
// "From the 9th kill, level-up shows '* Congratulations, you just advanced a Vitriol level.' with the OSRS jingle."
[[nodiscard]] constexpr bool shouldUseOsrsLevelUp(int kills, int track = 1) noexcept {
    return track == 1 && kills >= OSRS_LEVEL_UP_KILL_THRESHOLD;
}

[[nodiscard]] inline bool shouldUseOsrsLevelUp(const GameState& state) noexcept {
    return shouldUseOsrsLevelUp(toxicKills(state), toxicTrack(state));
}

[[nodiscard]] inline std::string_view levelUpNarratorLine(int kills, int track = 1) noexcept {
    if (shouldUseOsrsLevelUp(kills, track)) {
        return OSRS_LEVEL_UP_LINE;
    }
    return DEFAULT_LEVEL_UP_LINE;
}

[[nodiscard]] inline std::string_view levelUpNarratorLine(const GameState& state) noexcept {
    return levelUpNarratorLine(toxicKills(state), toxicTrack(state));
}

[[nodiscard]] inline std::string_view levelUpSoundSfx(int kills, int track = 1) noexcept {
    if (shouldUseOsrsLevelUp(kills, track)) {
        return OSRS_LEVEL_UP_JINGLE_SFX;
    }
    return DEFAULT_LEVEL_UP_SFX;
}

[[nodiscard]] inline std::string_view levelUpSoundSfx(const GameState& state) noexcept {
    return levelUpSoundSfx(toxicKills(state), toxicTrack(state));
}

} // namespace nundertale
