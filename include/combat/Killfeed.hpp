#pragma once

#include "core/GameContext.hpp"
#include "core/ItemDatabase.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <deque>
#include <string>
#include <string_view>

namespace nundertale {

// CS 1.6 Killfeed parameters
inline constexpr std::size_t KILLFEED_MAX_LINES = 4;
inline constexpr float KILLFEED_DEFAULT_LIFETIME = 5.0f; // ~5 s fade each

// CS 1.6 Killfeed colors (RGBA uint8)
struct KillfeedColor {
    unsigned char r = 255;
    unsigned char g = 255;
    unsigned char b = 255;
    unsigned char a = 255;
};

// Canonical CS 1.6 killfeed colors
inline constexpr KillfeedColor KILLFEED_COLOR_ATTACKER_RED{255, 64, 64, 255};
inline constexpr KillfeedColor KILLFEED_COLOR_VICTIM_BLUE{64, 128, 255, 255};
inline constexpr KillfeedColor KILLFEED_COLOR_WEAPON_ORANGE{255, 178, 32, 255};
inline constexpr KillfeedColor KILLFEED_COLOR_HEADSHOT_YELLOW{255, 215, 0, 255};

// Gamertag mapping entry for ex-gamers (LORE §5, §8.9, §30.4)
struct ExGamerEntry {
    std::string_view enemyId;
    std::string_view characterName;
    std::string_view gamertag;
    int year;
};

// Canonical ex-gamers from LORE §5 table & §8.9:
// 1. xX_AFK_Xx (2015) -> Napstablurk (Ruins optional)
// 2. BrbJerry (2016) -> Jerry (Snowdinha)
// 3. GymRat_Gaming (2017) -> Musk-ron (Wetfall)
// 4. quiet_healer (2018) -> Vtuber-en (Wetfall)
// 5. ClipFarmer_99 (2019) -> Mettapaw (Hotland)
// 6. xX_BarkSc0p3_Xx (2021) -> Barky (Ruins mini-boss)
inline constexpr std::array<ExGamerEntry, 6> EX_GAMER_TABLE = {{
    {"napstablurk", "Napstablurk", "xX_AFK_Xx", 2015},
    {"jerry", "Jerry", "BrbJerry", 2016},
    {"musk_ron", "Musk-ron", "GymRat_Gaming", 2017},
    {"vtuber_en", "Vtuber-en", "quiet_healer", 2018},
    {"mettapaw", "Mettapaw", "ClipFarmer_99", 2019},
    {"barky", "Barky", "xX_BarkSc0p3_Xx", 2021}
}};

// Look up former gamertag for an enemy id or character name.
// Returns gamertag if found, or empty string_view if not an ex-gamer.
inline std::string_view lookupExGamerGamertag(std::string_view enemyOrName) {
    auto equalIgnoreCase = [](std::string_view a, std::string_view b) {
        if (a.size() != b.size()) return false;
        for (std::size_t i = 0; i < a.size(); ++i) {
            char ca = a[i];
            char cb = b[i];
            if (ca >= 'A' && ca <= 'Z') ca = static_cast<char>(ca + ('a' - 'A'));
            if (cb >= 'A' && cb <= 'Z') cb = static_cast<char>(cb + ('a' - 'A'));
            if (ca != cb) return false;
        }
        return true;
    };

    for (const auto& entry : EX_GAMER_TABLE) {
        if (equalIgnoreCase(entry.enemyId, enemyOrName) ||
            equalIgnoreCase(entry.characterName, enemyOrName) ||
            equalIgnoreCase(entry.gamertag, enemyOrName)) {
            return entry.gamertag;
        }
    }
    return "";
}

// Resolves victim display name: returns gamertag if ex-gamer, else fallback name
inline std::string resolveVictimName(std::string_view victimIdOrName) {
    std::string_view tag = lookupExGamerGamertag(victimIdOrName);
    if (!tag.empty()) {
        return std::string(tag);
    }
    return std::string(victimIdOrName);
}

// A single line / entry in the killfeed
struct KillfeedEntry {
    std::string attacker;    // Attacker name (red), e.g. "GRUB"
    std::string weapon;      // Equipped weapon or icon identifier (yellow-orange), e.g. "crowbar"
    std::string victim;      // Victim name (blue) or ex-gamer gamertag
    bool headshot = false;   // Crit / headshot icon
    float remainingTime = KILLFEED_DEFAULT_LIFETIME;
    float maxLifetime = KILLFEED_DEFAULT_LIFETIME;

    // Alpha ratio [0.0f, 1.0f] for rendering fade
    [[nodiscard]] float alphaRatio() const {
        if (maxLifetime <= 0.0f) return 0.0f;
        float r = remainingTime / maxLifetime;
        if (r < 0.0f) return 0.0f;
        if (r > 1.0f) return 1.0f;
        return r;
    }

    [[nodiscard]] bool isExpired() const {
        return remainingTime <= 0.0f;
    }

    // Format as textual representation: e.g. "GRUB [crowbar] (HS) xX_BarkSc0p3_Xx"
    [[nodiscard]] std::string formatText() const {
        std::string s;
        s.reserve(attacker.size() + weapon.size() + victim.size() + 16);
        s += attacker;
        s += " [";
        s += weapon;
        s += "]";
        if (headshot) {
            s += " (HS)";
        }
        s += " ";
        s += victim;
        return s;
    }
};

class Killfeed {
public:
    explicit Killfeed(float defaultLifetime = KILLFEED_DEFAULT_LIFETIME)
        : m_defaultLifetime(defaultLifetime) {}

    // Push a new kill entry:
    // Adds to the bottom (newest line at bottom).
    // Caps at KILLFEED_MAX_LINES (max 4 lines; if full, the oldest line at index 0 is dropped).
    void push(std::string_view attacker, std::string_view weapon,
              std::string_view victim, bool headshot = false,
              float lifetime = -1.0f) {
        float life = (lifetime > 0.0f) ? lifetime : m_defaultLifetime;

        KillfeedEntry entry;
        entry.attacker = std::string(attacker);
        entry.weapon = std::string(weapon);
        entry.victim = resolveVictimName(victim);
        entry.headshot = headshot;
        entry.remainingTime = life;
        entry.maxLifetime = life;

        if (m_entries.size() >= KILLFEED_MAX_LINES) {
            m_entries.pop_front();
        }
        m_entries.push_back(std::move(entry));
    }

    // Push using GameState: retrieves equipped weapon or uses fallback, resolves victim gamertag
    void pushKill(const GameState& state, std::string_view victimIdOrName,
                  bool headshot = false, std::string_view attackerName = "") {
        std::string_view attacker = attackerName;
        if (attacker.empty()) {
            attacker = state.player.name.empty() ? "GRUB" : state.player.name;
        }
        std::string_view weapon = state.player.weapon.empty() ? "crowbar" : state.player.weapon;
        push(attacker, weapon, victimIdOrName, headshot);
    }

    // Updates timer on all active entries and removes expired ones.
    // Preserves ordering: oldest remains at front, newest at back.
    void update(float dt) {
        if (dt <= 0.0f) return;

        for (auto& entry : m_entries) {
            entry.remainingTime -= dt;
        }

        while (!m_entries.empty() && m_entries.front().isExpired()) {
            m_entries.pop_front();
        }
    }

    // Lines count
    [[nodiscard]] std::size_t size() const {
        return m_entries.size();
    }

    [[nodiscard]] bool empty() const {
        return m_entries.empty();
    }

    // Access entry by line index (0 is oldest, size() - 1 is newest at bottom)
    [[nodiscard]] const KillfeedEntry& operator[](std::size_t index) const {
        return m_entries[index];
    }

    [[nodiscard]] const KillfeedEntry& get(std::size_t index) const {
        return m_entries[index];
    }

    // Newest entry (at bottom)
    [[nodiscard]] const KillfeedEntry& newest() const {
        return m_entries.back();
    }

    // Oldest entry (at top)
    [[nodiscard]] const KillfeedEntry& oldest() const {
        return m_entries.front();
    }

    // Clear all entries
    void clear() {
        m_entries.clear();
    }

    // Expose entry collection
    [[nodiscard]] const std::deque<KillfeedEntry>& entries() const {
        return m_entries;
    }

private:
    float m_defaultLifetime = KILLFEED_DEFAULT_LIFETIME;
    std::deque<KillfeedEntry> m_entries;
};

} // namespace nundertale
