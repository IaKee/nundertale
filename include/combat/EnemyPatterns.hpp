#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include "Bullet.hpp"

namespace nundertale {

enum class EnemyPatternKind {
    None,
    Rain,
    Hop,
    Swarm,
    Wall,
    Bounce,
    Ring,
    Beat,
    TextWall
};

inline EnemyPatternKind enemyPatternKindFromString(const std::string& kind) {
    if (kind == "rain") return EnemyPatternKind::Rain;
    if (kind == "hop") return EnemyPatternKind::Hop;
    if (kind == "swarm") return EnemyPatternKind::Swarm;
    if (kind == "wall") return EnemyPatternKind::Wall;
    if (kind == "bounce") return EnemyPatternKind::Bounce;
    if (kind == "ring") return EnemyPatternKind::Ring;
    if (kind == "beat") return EnemyPatternKind::Beat;
    if (kind == "text_wall") return EnemyPatternKind::TextWall;
    return EnemyPatternKind::None;
}

struct EnemyPatternDef {
    std::string id;
    EnemyPatternKind kind = EnemyPatternKind::None;
    int weight = 1;
    int count = 1;
    float speed = 100.0f;
    int damage = 1;
    int furry = 0;
    int heal = 0;
    std::string text;
    std::vector<std::string> words;
    AttackColor color = AttackColor::White;
    bool strike = false;
    float strikeAfter = 0.0f;
    std::string beatmap;
};

inline std::size_t pickWeightedPatternIndex(const std::vector<EnemyPatternDef>& patterns,
                                            std::uint32_t randomValue) {
    int total = 0;
    for (const EnemyPatternDef& pattern : patterns) {
        if (pattern.weight > 0) total += pattern.weight;
    }
    if (total <= 0) return patterns.size();

    int selected = static_cast<int>(randomValue % static_cast<std::uint32_t>(total));
    for (std::size_t index = 0; index < patterns.size(); ++index) {
        const int weight = patterns[index].weight;
        if (weight <= 0) continue;
        if (selected < weight) return index;
        selected -= weight;
    }
    return patterns.size();
}

} // namespace nundertale
