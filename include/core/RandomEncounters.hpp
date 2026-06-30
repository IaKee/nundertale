#pragma once

#include <cctype>
#include <cstdint>
#include <array>
#include <string>
#include <string_view>
#include <vector>

namespace nundertale {

struct EncounterEntry {
    std::string id;
    int weight = 0;
};

inline std::string trimEncounterToken(const std::string& value) {
    std::size_t begin = 0;
    while (begin < value.size() && std::isspace(static_cast<unsigned char>(value[begin])) != 0) ++begin;
    std::size_t end = value.size();
    while (end > begin && std::isspace(static_cast<unsigned char>(value[end - 1])) != 0) --end;
    return value.substr(begin, end - begin);
}

inline std::vector<EncounterEntry> parseEncounterSpec(const std::string& spec) {
    std::vector<EncounterEntry> entries;
    std::size_t begin = 0;
    while (begin <= spec.size()) {
        const std::size_t comma = spec.find(',', begin);
        const std::string token = trimEncounterToken(spec.substr(
            begin, comma == std::string::npos ? std::string::npos : comma - begin));
        const std::size_t colon = token.rfind(':');
        if (colon != std::string::npos) {
            const std::string id = trimEncounterToken(token.substr(0, colon));
            const std::string weightText = trimEncounterToken(token.substr(colon + 1));
            int weight = 0;
            bool valid = !id.empty() && !weightText.empty();
            for (char ch : weightText) {
                if (std::isdigit(static_cast<unsigned char>(ch)) == 0) {
                    valid = false;
                    break;
                }
                weight = weight * 10 + (ch - '0');
            }
            if (valid && weight > 0) entries.push_back({ id, weight });
        }
        if (comma == std::string::npos) break;
        begin = comma + 1;
    }
    return entries;
}

inline std::uint32_t nextEncounterRandom(std::uint32_t& state) {
    if (state == 0u) state = 0xA341316Cu;
    state ^= state << 13u;
    state ^= state >> 17u;
    state ^= state << 5u;
    return state;
}

inline std::string pickWeightedEncounter(const std::vector<EncounterEntry>& entries,
                                         std::uint32_t& rngState) {
    int total = 0;
    for (const EncounterEntry& entry : entries) {
        if (entry.weight > 0) total += entry.weight;
    }
    if (total <= 0) return {};

    int selected = static_cast<int>(nextEncounterRandom(rngState) % static_cast<std::uint32_t>(total));
    for (const EncounterEntry& entry : entries) {
        if (entry.weight <= 0) continue;
        if (selected < entry.weight) return entry.id;
        selected -= entry.weight;
    }
    return {};
}

inline int randomEncounterStepCountdown(std::uint32_t& rngState) {
    return 90 + static_cast<int>(nextEncounterRandom(rngState) % 111u);
}

inline bool encounterBudgetAllows(int encounterCount, int encounterBudget) {
    return encounterBudget > 0 && encounterCount < encounterBudget;
}

struct EncounterAreaCap {
    std::string_view area;
    int kills = 0;
};

inline constexpr std::array<EncounterAreaCap, 4> TOXIC_ENCOUNTER_CAPS{{
    { "ruins", 6 },
    { "snowdinha", 6 },
    { "wetfall", 5 },
    { "hotland", 3 },
}};

inline std::string_view encounterAreaForRoom(std::string_view roomId) {
    for (const EncounterAreaCap& entry : TOXIC_ENCOUNTER_CAPS) {
        if (roomId == entry.area ||
            (roomId.size() > entry.area.size() &&
             roomId.substr(0, entry.area.size()) == entry.area &&
             roomId[entry.area.size()] == '_')) {
            return entry.area;
        }
    }
    return {};
}

inline int toxicEncounterCap(std::string_view area) {
    for (const EncounterAreaCap& entry : TOXIC_ENCOUNTER_CAPS) {
        if (entry.area == area) return entry.kills;
    }
    return 0;
}

// On the active toxic track, the area's kill cap replaces the per-room budget.
// Unknown areas retain the regular budget until their cap is defined.
inline bool encounterAllowed(int encounterCount,
                             int encounterBudget,
                             int toxicTrack,
                             int areaKills,
                             std::string_view area) {
    const int areaCap = toxicEncounterCap(area);
    if (toxicTrack == 1 && areaCap > 0) return areaKills < areaCap;
    return encounterBudgetAllows(encounterCount, encounterBudget);
}

inline bool advanceEncounterStep(int& stepsRemaining, bool walked) {
    if (!walked || stepsRemaining <= 0) return false;
    --stepsRemaining;
    return stepsRemaining == 0;
}

} // namespace nundertale
