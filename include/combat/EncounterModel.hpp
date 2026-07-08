#pragma once

// BFL-004 / ACT-004 / C1-024: pure multi-enemy encounter model ("X and co.").
// No Raylib, no BattleSystem. Wiring is a later task (call sites in the header footer).

#include "core/TiledMap.hpp"

#include <cstddef>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace nundertale {

constexpr std::size_t kMaxEncounterEnemies = 3;

// Route-specific replacement of any encounter field (empty / unset = keep the base value).
struct EncounterRouteOverride {
    std::vector<std::string> enemies;
    std::string introText;
    std::string background;
    std::string music;
    bool hasCanFlee = false;
    bool canFlee = false;
};

struct Encounter {
    std::string id;
    std::vector<std::string> enemies; // enemy ids (data/enemies/<id>.json), 1..3
    std::string introText;            // "" = generated from the enemy names
    std::string background;
    std::string music;
    bool canFlee = false;
    std::map<std::string, EncounterRouteOverride> routeOverrides; // "gamer" / "toxic" / "furry"
};

namespace encounter_detail {

using Json = tiled_detail::JsonValue;

inline bool validEnemyId(const std::string& id) {
    if (id.empty()) return false;
    for (char ch : id) {
        const bool ok = (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
                        (ch >= '0' && ch <= '9') || ch == '_';
        if (!ok) return false;
    }
    return true;
}

inline bool readEnemies(const Json& owner, std::vector<std::string>& out) {
    const Json* value = owner.field("enemies");
    if (value == nullptr || value->kind != Json::Kind::Array) return false;
    if (value->array.empty() || value->array.size() > kMaxEncounterEnemies) return false;
    std::vector<std::string> parsed;
    for (const Json& item : value->array) {
        if (item.kind != Json::Kind::String || !validEnemyId(item.string)) return false;
        parsed.push_back(item.string);
    }
    out = std::move(parsed);
    return true;
}

// Optional string field: absent is fine, wrong type is an error.
inline bool readOptString(const Json& owner, const char* key, std::string& out) {
    const Json* value = owner.field(key);
    if (value == nullptr) return true;
    if (value->kind != Json::Kind::String) return false;
    out = value->string;
    return true;
}

} // namespace encounter_detail

// Parses data/encounters/<id>.json. The legacy single-enemy shape parses unchanged.
inline bool parseEncounter(const std::string& jsonText, Encounter& out) {
    using encounter_detail::Json;
    Json root;
    if (!tiled_detail::JsonParser(jsonText).parse(root) || root.kind != Json::Kind::Object) return false;

    Encounter parsed;
    if (!encounter_detail::readEnemies(root, parsed.enemies)) return false;
    if (!encounter_detail::readOptString(root, "id", parsed.id) ||
        !encounter_detail::readOptString(root, "introText", parsed.introText) ||
        !encounter_detail::readOptString(root, "background", parsed.background) ||
        !encounter_detail::readOptString(root, "music", parsed.music)) {
        return false;
    }
    if (const Json* flee = root.field("canFlee")) {
        if (flee->kind != Json::Kind::Boolean) return false;
        parsed.canFlee = flee->boolean;
    }

    if (const Json* overrides = root.field("routeOverrides")) {
        if (overrides->kind != Json::Kind::Object) return false;
        for (const auto& [route, node] : overrides->object) {
            if (node.kind != Json::Kind::Object || route.empty()) return false;
            EncounterRouteOverride entry;
            if (node.field("enemies") != nullptr && !encounter_detail::readEnemies(node, entry.enemies)) {
                return false;
            }
            if (!encounter_detail::readOptString(node, "introText", entry.introText) ||
                !encounter_detail::readOptString(node, "background", entry.background) ||
                !encounter_detail::readOptString(node, "music", entry.music)) {
                return false;
            }
            if (const Json* flee = node.field("canFlee")) {
                if (flee->kind != Json::Kind::Boolean) return false;
                entry.hasCanFlee = true;
                entry.canFlee = flee->boolean;
            }
            parsed.routeOverrides[route] = std::move(entry);
        }
    }

    out = std::move(parsed);
    return true;
}

inline bool loadEncounterFile(const std::string& path, Encounter& out) {
    std::ifstream file(path);
    if (!file) return false;
    std::stringstream ss;
    ss << file.rdbuf();
    return parseEncounter(ss.str(), out);
}

// The encounter as seen on `route` (routeOverrides applied, then cleared). Unknown route = unchanged.
inline Encounter resolveEncounterForRoute(const Encounter& base, const std::string& route) {
    Encounter result = base;
    result.routeOverrides.clear();
    const auto found = base.routeOverrides.find(route);
    if (found == base.routeOverrides.end()) return result;
    const EncounterRouteOverride& o = found->second;
    if (!o.enemies.empty()) result.enemies = o.enemies;
    if (!o.introText.empty()) result.introText = o.introText;
    if (!o.background.empty()) result.background = o.background;
    if (!o.music.empty()) result.music = o.music;
    if (o.hasCanFlee) result.canFlee = o.canFlee;
    return result;
}

// "Barky", "Barky and co." (2+ names). Empty list = "".
inline std::string encounterGroupName(const std::vector<std::string>& names) {
    if (names.empty()) return "";
    return names.size() == 1U ? names.front() : names.front() + " and co.";
}

// Intro line: the data text if any, otherwise generated from the names ("* X and co. blocked your path!").
inline std::string encounterIntroText(const Encounter& encounter, const std::vector<std::string>& names) {
    if (!encounter.introText.empty()) return encounter.introText;
    const std::string group = encounterGroupName(names);
    return group.empty() ? std::string("* Something blocked your path!") : "* " + group + " blocked your path!";
}

enum class EnemyStatus { Alive, Dead, Spared, Fled };

struct TurnAttack {
    std::size_t enemyIndex = 0;
    std::string patternId;
};

// Runtime state of one multi-enemy fight. Indices match Encounter::enemies.
class EncounterState {
public:
    EncounterState() = default;
    EncounterState(Encounter encounter, std::vector<std::string> names)
        : m_encounter(std::move(encounter)), m_names(std::move(names)) {
        m_status.assign(m_encounter.enemies.size(), EnemyStatus::Alive);
        m_names.resize(m_encounter.enemies.size());
    }

    const Encounter& encounter() const { return m_encounter; }
    std::size_t size() const { return m_status.size(); }
    EnemyStatus status(std::size_t i) const { return i < m_status.size() ? m_status[i] : EnemyStatus::Dead; }
    bool isActive(std::size_t i) const { return status(i) == EnemyStatus::Alive; }
    const std::string& enemyId(std::size_t i) const { return m_encounter.enemies.at(i); }
    const std::string& name(std::size_t i) const { return m_names.at(i); }

    // Alive = not dead, not spared, not fled. Out-of-range indices are ignored (return false).
    bool kill(std::size_t i) { return setStatus(i, EnemyStatus::Dead); }
    bool spare(std::size_t i) { return setStatus(i, EnemyStatus::Spared); }
    bool flee(std::size_t i) { return setStatus(i, EnemyStatus::Fled); }

    std::vector<std::size_t> activeIndices() const {
        std::vector<std::size_t> result;
        for (std::size_t i = 0; i < m_status.size(); ++i) {
            if (m_status[i] == EnemyStatus::Alive) result.push_back(i);
        }
        return result;
    }
    std::size_t activeCount() const { return activeIndices().size(); }

    // Victory: nobody left fighting (every enemy dead, spared or fled). An empty encounter is not a victory.
    bool isVictory() const { return !m_status.empty() && activeCount() == 0U; }
    std::size_t countWith(EnemyStatus wanted) const {
        std::size_t n = 0;
        for (EnemyStatus s : m_status) n += (s == wanted) ? 1U : 0U;
        return n;
    }
    bool anyKilled() const { return countWith(EnemyStatus::Dead) > 0U; }

    // Whole-party flee (the FLEE command). Fails if the encounter forbids it or nobody is active.
    bool fleeAll() {
        if (!m_encounter.canFlee || activeCount() == 0U) return false;
        for (EnemyStatus& s : m_status) {
            if (s == EnemyStatus::Alive) s = EnemyStatus::Fled;
        }
        return true;
    }

    // 'Alone' condition (Reply Guy `Talk`, GLaDOwOS): `i` is active and no other enemy is.
    bool isAlone(std::size_t i) const { return isActive(i) && activeCount() == 1U; }
    // Same, by enemy id (true if any active copy of `id` is the only one left).
    bool isAloneById(const std::string& id) const {
        const auto active = activeIndices();
        return active.size() == 1U && m_encounter.enemies[active.front()] == id;
    }
    // Company of `i`: other active enemies still in the fight.
    std::size_t companionCount(std::size_t i) const {
        const std::size_t active = activeCount();
        return isActive(i) ? active - 1U : active;
    }

    // Jerry `Ditch`: Jerry leaves, the others become spareable (ACT-004).
    bool ditch(std::size_t i) {
        if (!flee(i)) return false;
        m_companionsSpareable = true;
        return true;
    }
    bool companionsSpareable() const { return m_companionsSpareable; }

    // "X and co." for whoever is still fighting; "" when nobody is.
    std::string activeGroupName() const {
        std::vector<std::string> names;
        for (std::size_t i : activeIndices()) names.push_back(m_names[i]);
        return encounterGroupName(names);
    }
    // Intro line for the full roster.
    std::string introText() const { return encounterIntroText(m_encounter, m_names); }

    // Turn aggregation: every active enemy attacks in the same turn, in roster order, each with its own
    // pattern. `patternIds[i]` is enemy i's pattern for this turn ("" = no attack); `skip[i]` (optional,
    // missing = false) drops enemy i's attack (skipAttack / freezeTurns / Hold Breath style effects).
    std::vector<TurnAttack> aggregateTurn(const std::vector<std::string>& patternIds,
                                          const std::vector<bool>& skip = {}) const {
        std::vector<TurnAttack> attacks;
        for (std::size_t i = 0; i < m_status.size(); ++i) {
            if (m_status[i] != EnemyStatus::Alive) continue;
            if (i >= patternIds.size() || patternIds[i].empty()) continue;
            if (i < skip.size() && skip[i]) continue;
            attacks.push_back({ i, patternIds[i] });
        }
        return attacks;
    }

private:
    bool setStatus(std::size_t i, EnemyStatus next) {
        if (i >= m_status.size() || m_status[i] != EnemyStatus::Alive) return false;
        m_status[i] = next;
        return true;
    }

    Encounter m_encounter;
    std::vector<std::string> m_names;
    std::vector<EnemyStatus> m_status;
    bool m_companionsSpareable = false;
};

// Later wiring (NOT done here), the places that assume one enemy today:
//  - src/scenes/BattleScene.cpp ctor: loads only enemies[0]; use loadEncounterFile + resolveEncounterForRoute
//    and build one Enemy per id.
//  - include/combat/BattleSystem.hpp / src/combat/BattleSystem.cpp: single Enemy, isVictory/isSpared/isFled
//    flags, target select (FIGHT/ACT pick an enemy), enemy turn loop (use aggregateTurn), spare/flee/kill hooks
//    (EncounterState::spare/kill/flee/fleeAll), HP bars and "X and co." intro/idle text.
//  - ActGate (ActGate.hpp): feed EncounterState::isAlone / companionCount into Reply Guy `Talk` and
//    GLaDOwOS; Jerry `Ditch` -> EncounterState::ditch + companionsSpareable.
//  - Enemy kill/spare outcome flags (Enemy.hpp outcome flags) and KillAnnouncer streaks per killed enemy.

} // namespace nundertale
