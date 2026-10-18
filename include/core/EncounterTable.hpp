#pragma once

#include "core/TiledMap.hpp"
#include <algorithm>
#include <cstdint>
#include <fstream>
#include <limits>
#include <random>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace nundertale {

struct EncounterEntry {
    std::string encounter;
    int weight = 0;
};

struct EncounterTableData {
    int stepsMin = 0;
    int stepsMax = 0;
    int budget = 0;
    std::vector<EncounterEntry> entries;
};

inline bool parseEncounterTableJson(const std::string& text, EncounterTableData& output) {
    tiled_detail::JsonValue root;
    if (!tiled_detail::JsonParser(text).parse(root) ||
        root.kind != tiled_detail::JsonValue::Kind::Object) {
        return false;
    }

    EncounterTableData parsed;
    if (!tiled_detail::intField(root, "stepsMin", parsed.stepsMin) ||
        !tiled_detail::intField(root, "stepsMax", parsed.stepsMax) ||
        !tiled_detail::intField(root, "budget", parsed.budget) ||
        parsed.stepsMin <= 0 || parsed.stepsMax < parsed.stepsMin || parsed.budget < 0) {
        return false;
    }

    const tiled_detail::JsonValue* entries = root.field("entries");
    if (entries == nullptr || entries->kind != tiled_detail::JsonValue::Kind::Array ||
        entries->array.empty()) {
        return false;
    }

    long long totalWeight = 0;
    for (const tiled_detail::JsonValue& rawEntry : entries->array) {
        EncounterEntry entry;
        if (rawEntry.kind != tiled_detail::JsonValue::Kind::Object ||
            !tiled_detail::stringField(rawEntry, "encounter", entry.encounter) ||
            !tiled_detail::intField(rawEntry, "weight", entry.weight) ||
            entry.encounter.empty() || entry.weight <= 0) {
            return false;
        }
        totalWeight += entry.weight;
        if (totalWeight > std::numeric_limits<int>::max()) return false;
        parsed.entries.push_back(std::move(entry));
    }

    output = std::move(parsed);
    return true;
}

inline bool loadEncounterTableFromFile(const std::string& path, EncounterTableData& output) {
    std::ifstream file(path);
    if (!file) return false;
    std::ostringstream json;
    json << file.rdbuf();
    return parseEncounterTableJson(json.str(), output);
}

inline bool encounterBudgetAllows(int encounterCount, int budget) {
    return encounterCount >= 0 && budget > 0 && encounterCount < budget;
}

class EncounterTable {
public:
    explicit EncounterTable(EncounterTableData data)
        : EncounterTable(std::move(data), std::random_device{}()) {}

    EncounterTable(EncounterTableData data, std::uint32_t seed)
        : m_data(std::move(data)), m_rng(seed) {
        rollThreshold();
    }

    bool addDistance(float pixels) {
        if (pixels <= 0.0f || m_threshold <= 0) return false;
        m_distance += pixels;
        if (m_distance < static_cast<float>(m_threshold)) return false;
        m_distance = 0.0f;
        rollThreshold();
        return true;
    }

    std::string pickEncounter() {
        int totalWeight = 0;
        for (const EncounterEntry& entry : m_data.entries) totalWeight += entry.weight;
        if (totalWeight <= 0) return {};

        std::uniform_int_distribution<int> distribution(1, totalWeight);
        int roll = distribution(m_rng);
        for (const EncounterEntry& entry : m_data.entries) {
            roll -= entry.weight;
            if (roll <= 0) return entry.encounter;
        }
        return m_data.entries.back().encounter;
    }

    int budget() const { return m_data.budget; }
    int threshold() const { return m_threshold; }
    float distance() const { return m_distance; }
    const std::vector<EncounterEntry>& entries() const { return m_data.entries; }

private:
    void rollThreshold() {
        if (m_data.stepsMin <= 0 || m_data.stepsMax < m_data.stepsMin) {
            m_threshold = 0;
            return;
        }
        std::uniform_int_distribution<int> distribution(m_data.stepsMin, m_data.stepsMax);
        m_threshold = distribution(m_rng);
    }

    EncounterTableData m_data;
    std::mt19937 m_rng;
    float m_distance = 0.0f;
    int m_threshold = 0;
};

} // namespace nundertale
