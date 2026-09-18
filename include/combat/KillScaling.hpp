#pragma once

#include "combat/Enemy.hpp"
#include "combat/EnemyPatterns.hpp"
#include "core/TiledMap.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace nundertale {

// TOX-009 (LORE §8.11): Kill-scaled difficulty.
// Enemies can declare an optional "killScaling" block in data/enemies/*.json.
// On the toxic track or when kills.ruins > 0, enemy ATK and bullet speed scale
// with the number of kills in Ruins. When the block is absent or kills <= 0,
// stats remain unchanged.

struct KillScalingDef {
    bool present = false;

    // Additive scaling per kill in Ruins (kills.ruins)
    int atkPerKill = 0;
    float speedPerKill = 0.0f;

    // Multiplicative scaling per kill in Ruins (e.g. 0.05 = +5% per kill)
    float atkMultiplierPerKill = 0.0f;
    float speedMultiplierPerKill = 0.0f;

    // Optional direct multipliers (applied once when scaled, default 1.0)
    float atkMultiplier = 1.0f;
    float speedMultiplier = 1.0f;

    // Optional HP / damage scaling per kill (LORE §8.11)
    int hpPerKill = 0;
    float hpMultiplierPerKill = 0.0f;
    int damagePerKill = 0;
    float damageMultiplierPerKill = 0.0f;
    int extraActsPerKill = 0;
};

// Pure calculation: scales ATK given base ATK, definition, and kills.ruins count.
inline int scaleEnemyAtk(int baseAtk, const KillScalingDef& def, int killsRuins) {
    if (!def.present || killsRuins <= 0) return baseAtk;

    float atk = static_cast<float>(baseAtk);
    if (def.atkPerKill != 0) {
        atk += static_cast<float>(def.atkPerKill * killsRuins);
    }
    if (def.atkMultiplierPerKill != 0.0f) {
        atk *= (1.0f + def.atkMultiplierPerKill * static_cast<float>(killsRuins));
    }
    if (def.atkMultiplier != 1.0f) {
        atk *= def.atkMultiplier;
    }
    return std::max(0, static_cast<int>(std::round(atk)));
}

inline int scaleAtk(int baseAtk, const KillScalingDef& def, int killsRuins) {
    return scaleEnemyAtk(baseAtk, def, killsRuins);
}

// Pure calculation: scales bullet speed given base speed, definition, and kills.ruins count.
inline float scaleBulletSpeed(float baseSpeed, const KillScalingDef& def, int killsRuins) {
    if (!def.present || killsRuins <= 0) return baseSpeed;

    float speed = baseSpeed;
    if (def.speedPerKill != 0.0f) {
        speed += def.speedPerKill * static_cast<float>(killsRuins);
    }
    if (def.speedMultiplierPerKill != 0.0f) {
        speed *= (1.0f + def.speedMultiplierPerKill * static_cast<float>(killsRuins));
    }
    if (def.speedMultiplier != 1.0f) {
        speed *= def.speedMultiplier;
    }
    return std::max(0.0f, speed);
}

inline float scaleSpeed(float baseSpeed, const KillScalingDef& def, int killsRuins) {
    return scaleBulletSpeed(baseSpeed, def, killsRuins);
}

// Optional HP scaling
inline int scaleEnemyHp(int baseHp, const KillScalingDef& def, int killsRuins) {
    if (!def.present || killsRuins <= 0) return baseHp;

    float hp = static_cast<float>(baseHp);
    if (def.hpPerKill != 0) {
        hp += static_cast<float>(def.hpPerKill * killsRuins);
    }
    if (def.hpMultiplierPerKill != 0.0f) {
        hp *= (1.0f + def.hpMultiplierPerKill * static_cast<float>(killsRuins));
    }
    return std::max(1, static_cast<int>(std::round(hp)));
}

// Optional Damage scaling
inline int scaleDamage(int baseDamage, const KillScalingDef& def, int killsRuins) {
    if (!def.present || killsRuins <= 0) return baseDamage;

    float dmg = static_cast<float>(baseDamage);
    if (def.damagePerKill != 0) {
        dmg += static_cast<float>(def.damagePerKill * killsRuins);
    }
    if (def.damageMultiplierPerKill != 0.0f) {
        dmg *= (1.0f + def.damageMultiplierPerKill * static_cast<float>(killsRuins));
    }
    return std::max(1, static_cast<int>(std::round(dmg)));
}

// Parses a KillScalingDef from a JsonValue (either the root enemy JSON object or the killScaling block itself)
inline bool parseKillScaling(const tiled_detail::JsonValue& root, KillScalingDef& out) {
    out = KillScalingDef{};
    const tiled_detail::JsonValue* block = nullptr;

    if (root.kind == tiled_detail::JsonValue::Kind::Object) {
        const tiled_detail::JsonValue* sub = root.field("killScaling");
        if (sub != nullptr) {
            block = sub;
        } else if (root.field("atkPerKill") != nullptr || root.field("speedPerKill") != nullptr ||
                   root.field("atkMultiplier") != nullptr || root.field("speedMultiplier") != nullptr ||
                   root.field("atkMultiplierPerKill") != nullptr || root.field("speedMultiplierPerKill") != nullptr ||
                   root.field("atkBonus") != nullptr || root.field("speedBonus") != nullptr) {
            block = &root;
        }
    }

    if (block == nullptr || block->kind != tiled_detail::JsonValue::Kind::Object) {
        out.present = false;
        return false;
    }

    out.present = true;

    auto readNumber = [](const tiled_detail::JsonValue* obj, const std::string& key, double& val) -> bool {
        if (obj == nullptr) return false;
        const auto* f = obj->field(key);
        if (f != nullptr && f->kind == tiled_detail::JsonValue::Kind::Number) {
            val = f->number;
            return true;
        }
        return false;
    };

    double num = 0.0;
    if (readNumber(block, "atkPerKill", num) || readNumber(block, "atkBonus", num) ||
        readNumber(block, "atkBonusPerKill", num) || readNumber(block, "atk", num)) {
        out.atkPerKill = static_cast<int>(num);
    }
    if (readNumber(block, "atkMultiplierPerKill", num) || readNumber(block, "atkScalePerKill", num)) {
        out.atkMultiplierPerKill = static_cast<float>(num);
    }
    if (readNumber(block, "atkMultiplier", num) || readNumber(block, "atkMult", num)) {
        out.atkMultiplier = static_cast<float>(num);
    }

    if (readNumber(block, "speedPerKill", num) || readNumber(block, "bulletSpeedPerKill", num) ||
        readNumber(block, "speedBonus", num) || readNumber(block, "bulletSpeed", num) ||
        readNumber(block, "speed", num)) {
        out.speedPerKill = static_cast<float>(num);
    }
    if (readNumber(block, "speedMultiplierPerKill", num) || readNumber(block, "speedScalePerKill", num)) {
        out.speedMultiplierPerKill = static_cast<float>(num);
    }
    if (readNumber(block, "speedMultiplier", num) || readNumber(block, "speedMult", num)) {
        out.speedMultiplier = static_cast<float>(num);
    }

    if (readNumber(block, "hpPerKill", num) || readNumber(block, "hpBonus", num)) {
        out.hpPerKill = static_cast<int>(num);
    }
    if (readNumber(block, "hpMultiplierPerKill", num) || readNumber(block, "hpMultiplier", num)) {
        out.hpMultiplierPerKill = static_cast<float>(num);
    }
    if (readNumber(block, "damagePerKill", num) || readNumber(block, "damageBonus", num)) {
        out.damagePerKill = static_cast<int>(num);
    }
    if (readNumber(block, "damageMultiplierPerKill", num) || readNumber(block, "damageMultiplier", num)) {
        out.damageMultiplierPerKill = static_cast<float>(num);
    }
    if (readNumber(block, "extraActsPerKill", num) || readNumber(block, "extraActs", num)) {
        out.extraActsPerKill = static_cast<int>(num);
    }

    return true;
}

inline bool parseKillScalingFromJson(const std::string& jsonText, KillScalingDef& out) {
    tiled_detail::JsonValue root;
    if (!tiled_detail::JsonParser(jsonText).parse(root)) {
        out = KillScalingDef{};
        return false;
    }
    return parseKillScaling(root, out);
}

inline bool loadKillScalingFromFile(const std::string& filepath, KillScalingDef& out) {
    out = KillScalingDef{};
    std::ifstream file(filepath);
    if (!file.is_open()) return false;
    std::stringstream buffer;
    buffer << file.rdbuf();
    return parseKillScalingFromJson(buffer.str(), out);
}

namespace killscaling_detail {

template <typename Tag, typename Tag::type Member>
struct PrivateAccess {
    friend inline typename Tag::type accessMember(Tag) { return Member; }
};

struct EnemyPatternsTag {
    using type = std::vector<EnemyPatternDef> Enemy::*;
    friend inline type accessMember(EnemyPatternsTag);
};
template struct PrivateAccess<EnemyPatternsTag, &Enemy::m_patterns>;

} // namespace killscaling_detail

inline void applyKillScalingToPattern(EnemyPatternDef& pattern, const KillScalingDef& def, int killsRuins) {
    if (!def.present || killsRuins <= 0) return;
    pattern.speed = scaleBulletSpeed(pattern.speed, def, killsRuins);
    if (def.damagePerKill != 0 || def.damageMultiplierPerKill != 0.0f) {
        pattern.damage = scaleDamage(pattern.damage, def, killsRuins);
    }
}

inline void applyKillScalingToPatterns(std::vector<EnemyPatternDef>& patterns, const KillScalingDef& def, int killsRuins) {
    if (!def.present || killsRuins <= 0) return;
    for (auto& pattern : patterns) {
        applyKillScalingToPattern(pattern, def, killsRuins);
    }
}

inline void applyKillScaling(Enemy& enemy, const KillScalingDef& def, int killsRuins) {
    if (!def.present || killsRuins <= 0) return;

    const int baseAtk = enemy.getAtk();
    const int scaledAtk = scaleEnemyAtk(baseAtk, def, killsRuins);
    const int deltaAtk = scaledAtk - baseAtk;
    if (deltaAtk != 0) {
        enemy.modifyAtk(deltaAtk);
    }

    using namespace killscaling_detail;
    auto& patterns = enemy.*accessMember(EnemyPatternsTag{});
    applyKillScalingToPatterns(patterns, def, killsRuins);
}

} // namespace nundertale
