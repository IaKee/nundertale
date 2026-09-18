#pragma once

// T46 / TOX-009: Kill-scaled difficulty (killScaling block).
// LORE §8.11 / BACKLOG TOX-009:
// Optional killScaling block in enemy JSON scales ATK and bullet speed by kills.ruins.
// Pure helper tested here; no change when absent.

#include "combat/KillScaling.hpp"
#include "combat/Enemy.hpp"
#include "combat/EnemyPatterns.hpp"

#include <cassert>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

inline void suite_ru43() {
    using namespace nundertale;

    // 1. Default / Uninitialized state
    {
        KillScalingDef def;
        assert(!def.present);
        assert(def.atkPerKill == 0);
        assert(def.speedPerKill == 0.0f);
        assert(def.atkMultiplierPerKill == 0.0f);
        assert(def.speedMultiplierPerKill == 0.0f);
        assert(def.atkMultiplier == 1.0f);
        assert(def.speedMultiplier == 1.0f);
    }

    // 2. No change when absent (def.present == false)
    {
        KillScalingDef def;
        def.present = false;
        def.atkPerKill = 5;
        def.speedPerKill = 50.0f;

        assert(scaleEnemyAtk(4, def, 0) == 4);
        assert(scaleEnemyAtk(4, def, 1) == 4);
        assert(scaleEnemyAtk(4, def, 5) == 4);
        assert(scaleEnemyAtk(10, def, 20) == 10);
        assert(scaleAtk(4, def, 3) == 4);

        assert(scaleBulletSpeed(100.0f, def, 0) == 100.0f);
        assert(scaleBulletSpeed(100.0f, def, 1) == 100.0f);
        assert(scaleBulletSpeed(100.0f, def, 5) == 100.0f);
        assert(scaleSpeed(80.0f, def, 10) == 80.0f);
    }

    // 3. No change when killsRuins <= 0, even when block is present
    {
        KillScalingDef def;
        def.present = true;
        def.atkPerKill = 3;
        def.speedPerKill = 25.0f;
        def.atkMultiplierPerKill = 0.10f;
        def.speedMultiplierPerKill = 0.10f;

        assert(scaleEnemyAtk(4, def, 0) == 4);
        assert(scaleEnemyAtk(4, def, -1) == 4);
        assert(scaleEnemyAtk(10, def, -10) == 10);

        assert(scaleBulletSpeed(120.0f, def, 0) == 120.0f);
        assert(scaleBulletSpeed(120.0f, def, -1) == 120.0f);
        assert(scaleBulletSpeed(80.0f, def, -4) == 80.0f);
    }

    // 4. Additive scaling by kills.ruins
    {
        KillScalingDef def;
        def.present = true;
        def.atkPerKill = 2;
        def.speedPerKill = 15.0f;

        // Base ATK = 4
        assert(scaleEnemyAtk(4, def, 1) == 6);
        assert(scaleEnemyAtk(4, def, 2) == 8);
        assert(scaleEnemyAtk(4, def, 3) == 10);
        assert(scaleEnemyAtk(4, def, 6) == 16);

        // Base bullet speed = 100.0f
        assert(std::fabs(scaleBulletSpeed(100.0f, def, 1) - 115.0f) < 0.001f);
        assert(std::fabs(scaleBulletSpeed(100.0f, def, 2) - 130.0f) < 0.001f);
        assert(std::fabs(scaleBulletSpeed(100.0f, def, 4) - 160.0f) < 0.001f);
    }

    // 5. Multiplicative scaling per kill in Ruins
    {
        KillScalingDef def;
        def.present = true;
        def.atkMultiplierPerKill = 0.10f; // +10% per kill
        def.speedMultiplierPerKill = 0.05f; // +5% per kill

        // Base ATK = 10, kills = 2 -> 10 * 1.20 = 12
        assert(scaleEnemyAtk(10, def, 2) == 12);
        // Base ATK = 10, kills = 5 -> 10 * 1.50 = 15
        assert(scaleEnemyAtk(10, def, 5) == 15);

        // Base speed = 100.0f, kills = 4 -> 100 * 1.20 = 120.0f
        assert(std::fabs(scaleBulletSpeed(100.0f, def, 4) - 120.0f) < 0.01f);
    }

    // 6. JSON parsing from strings
    {
        // 6a: Enemy JSON without killScaling block
        const std::string noScalingJson = R"({
            "id": "plain_foe",
            "name": "Plain Foe",
            "hp": 25,
            "atk": 4
        })";
        KillScalingDef parsedNo;
        const bool foundNo = parseKillScalingFromJson(noScalingJson, parsedNo);
        assert(!foundNo);
        assert(!parsedNo.present);
        assert(scaleEnemyAtk(4, parsedNo, 5) == 4);
        assert(scaleBulletSpeed(90.0f, parsedNo, 5) == 90.0f);

        // 6b: Enemy JSON with explicit killScaling block
        const std::string scalingJson = R"({
            "id": "scaled_foe",
            "name": "Scaled Foe",
            "hp": 30,
            "atk": 5,
            "killScaling": {
                "atkPerKill": 1,
                "speedPerKill": 20.0
            }
        })";
        KillScalingDef parsedYes;
        const bool foundYes = parseKillScalingFromJson(scalingJson, parsedYes);
        assert(foundYes);
        assert(parsedYes.present);
        assert(parsedYes.atkPerKill == 1);
        assert(std::fabs(parsedYes.speedPerKill - 20.0f) < 0.001f);
        assert(scaleEnemyAtk(5, parsedYes, 3) == 8);
        assert(std::fabs(scaleBulletSpeed(100.0f, parsedYes, 3) - 160.0f) < 0.001f);

        // 6c: Direct killScaling object (without outer wrapper)
        const std::string directJson = R"({
            "atkPerKill": 2,
            "speedPerKill": 10.5
        })";
        KillScalingDef parsedDirect;
        const bool foundDirect = parseKillScalingFromJson(directJson, parsedDirect);
        assert(foundDirect);
        assert(parsedDirect.present);
        assert(parsedDirect.atkPerKill == 2);
        assert(std::fabs(parsedDirect.speedPerKill - 10.5f) < 0.001f);
    }

    // 7. Applying scaling to Enemy instance and pattern definitions
    {
        KillScalingDef def;
        def.present = true;
        def.atkPerKill = 3;
        def.speedPerKill = 25.0f;

        Enemy enemy("ScaleTester", 35, 6, 2);
        EnemyPatternDef pat1;
        pat1.id = "bullet_hop";
        pat1.speed = 80.0f;
        EnemyPatternDef pat2;
        pat2.id = "bullet_rain";
        pat2.speed = 120.0f;

        // Access internal patterns and add them
        using namespace killscaling_detail;
        auto& patterns = enemy.*accessMember(EnemyPatternsTag{});
        patterns.push_back(pat1);
        patterns.push_back(pat2);

        // At 0 kills: nothing changes
        applyKillScaling(enemy, def, 0);
        assert(enemy.getAtk() == 6);
        assert(std::fabs(enemy.getPatterns()[0].speed - 80.0f) < 0.001f);
        assert(std::fabs(enemy.getPatterns()[1].speed - 120.0f) < 0.001f);

        // At 2 kills: ATK +6 -> 12, pat1 speed +50 -> 130, pat2 speed +50 -> 170
        applyKillScaling(enemy, def, 2);
        assert(enemy.getAtk() == 12);
        assert(std::fabs(enemy.getPatterns()[0].speed - 130.0f) < 0.001f);
        assert(std::fabs(enemy.getPatterns()[1].speed - 170.0f) < 0.001f);
    }

    // 8. Existing enemy files: verify no change when absent
    {
        KillScalingDef barkyScale;
        loadKillScalingFromFile("data/enemies/barky.json", barkyScale);
        assert(!barkyScale.present);
        assert(scaleEnemyAtk(4, barkyScale, 5) == 4);
        assert(scaleBulletSpeed(110.0f, barkyScale, 5) == 110.0f);

        KillScalingDef froggScale;
        loadKillScalingFromFile("data/enemies/froggsona.json", froggScale);
        assert(!froggScale.present);
        assert(scaleEnemyAtk(4, froggScale, 3) == 4);
        assert(scaleBulletSpeed(120.0f, froggScale, 3) == 120.0f);
    }

    std::cout << "[PASS] ru43 / TOX-009: optional killScaling block in enemy JSON scales ATK and bullet speed by kills.ruins (no change when absent)." << std::endl;
}
