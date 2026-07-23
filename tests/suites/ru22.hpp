#pragma once

#include <cassert>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "combat/Enemy.hpp"
#include "combat/FloofyPatterns.hpp"

inline void suite_ru22() {
    using namespace nundertale;

    const Rectangle arena = { 237.0f, 250.0f, 165.0f, 140.0f };
    const Vector2 soulCenter = { 320.0f, 320.0f };

    // 1. Identification and naming of all 8 patterns
    const std::vector<FloofyPatternId> allPatterns = {
        FloofyPatternId::FriendshipPetals,
        FloofyPatternId::ThornVines,
        FloofyPatternId::PetalStorm,
        FloofyPatternId::PollenCloud,
        FloofyPatternId::BouquetBurst,
        FloofyPatternId::PounceRing,
        FloofyPatternId::TutorialPetals,
        FloofyPatternId::ForcedPetal
    };
    assert(allPatterns.size() == 8);

    for (FloofyPatternId id : allPatterns) {
        const std::string name = floofyPatternIdToString(id);
        assert(!name.empty());
        FloofyPatternId parsedId = FloofyPatternId::FriendshipPetals;
        assert(floofyPatternIdFromString(name, parsedId));
        assert(parsedId == id);
        assert(isFloofyPatternName(name));
        assert(Enemy::isFloofyPattern(name));

        const EnemyPatternKind kind = enemyPatternKindFromString(name);
        assert(kind != EnemyPatternKind::None);
    }
    assert(!isFloofyPatternName("unknown_petal_pattern"));

    // 2. Sprite mapping and fallback per contracts.md
    assert(std::string(floofyPatternPreferredSprite(FloofyPatternId::FriendshipPetals)) == "assets/sprites/bullets/petal.png");
    assert(std::string(floofyPatternPreferredSprite(FloofyPatternId::ThornVines)) == "assets/sprites/bullets/rose_vine.png");
    assert(std::string(floofyPatternPreferredSprite(FloofyPatternId::PetalStorm)) == "assets/sprites/bullets/thorn_petal.png");
    assert(std::string(floofyPatternPreferredSprite(FloofyPatternId::PollenCloud)) == "assets/sprites/bullets/pollen.png");
    assert(std::string(floofyPatternPreferredSprite(FloofyPatternId::BouquetBurst)) == "assets/sprites/bullets/thorn_petal.png");
    assert(std::string(floofyPatternPreferredSprite(FloofyPatternId::PounceRing)) == "assets/sprites/bullets/petal.png");
    assert(std::string(floofyPatternPreferredSprite(FloofyPatternId::TutorialPetals)) == "assets/sprites/bullets/petal.png");
    assert(std::string(floofyPatternPreferredSprite(FloofyPatternId::ForcedPetal)) == "assets/sprites/bullets/petal.png");

    // Existing fallback resolution
    assert(resolveFloofySprite("non_existent_sprite_file.png") == FLOOFY_SPRITE_FALLBACK);
    assert(resolveFloofySprite("assets/bullets/petal.png") == "assets/bullets/petal.png");

    // 3. Determinism given a seed
    for (FloofyPatternId id : allPatterns) {
        FloofyPatternConfig c1;
        c1.patternId = id;
        c1.seed = 0x57F716u;
        c1.intensity = 1.0f;

        FloofyPatternConfig c2 = c1;

        const FloofyTimeline t1 = generateFloofyTimeline(c1, arena, soulCenter);
        const FloofyTimeline t2 = generateFloofyTimeline(c2, arena, soulCenter);

        assert(t1.events.size() == t2.events.size());
        for (std::size_t i = 0; i < t1.events.size(); ++i) {
            const auto& e1 = t1.events[i];
            const auto& e2 = t2.events[i];
            assert(e1.spawnTime == e2.spawnTime);
            assert(e1.startPosition.x == e2.startPosition.x);
            assert(e1.startPosition.y == e2.startPosition.y);
            assert(e1.initialVelocity.x == e2.initialVelocity.x);
            assert(e1.initialVelocity.y == e2.initialVelocity.y);
            assert(e1.angle == e2.angle);
            assert(e1.damage == e2.damage);
            assert(e1.fur == e2.fur);
        }

        // Different seed produces different positions
        FloofyPatternConfig cDiff = c1;
        cDiff.seed = 0x987654u;
        const FloofyTimeline tDiff = generateFloofyTimeline(cDiff, arena, soulCenter);
        if (id == FloofyPatternId::FriendshipPetals || id == FloofyPatternId::ThornVines) {
            bool hasDifference = false;
            for (std::size_t i = 0; i < std::min(t1.events.size(), tDiff.events.size()); ++i) {
                if (t1.events[i].startPosition.x != tDiff.events[i].startPosition.x ||
                    t1.events[i].startPosition.y != tDiff.events[i].startPosition.y) {
                    hasDifference = true;
                    break;
                }
            }
            assert(hasDifference);
        }
    }

    // 4. Spawn Timelines & Ordering
    for (FloofyPatternId id : allPatterns) {
        FloofyPatternConfig cfg;
        cfg.patternId = id;
        cfg.turnDuration = 5.0f;
        const FloofyTimeline timeline = generateFloofyTimeline(cfg, arena, soulCenter);
        assert(!timeline.events.empty());

        float prevSpawnTime = -0.01f;
        for (const auto& ev : timeline.events) {
            assert(ev.spawnTime >= 0.0f);
            assert(ev.spawnTime <= cfg.turnDuration);
            assert(ev.spawnTime >= prevSpawnTime); // Monotonically non-decreasing
            prevSpawnTime = ev.spawnTime;
            assert(ev.damage > 0);
            assert(ev.fur > 0);
        }
    }

    // 5. Motion and Buzz Wobble
    {
        // Oscillation around zero
        constexpr float freq = 26.0f;
        constexpr float amp = 8.0f;
        assert(std::abs(floofyBuzzOffset(0.0f, freq, amp)) < 1e-4f);
        const float peak = floofyBuzzOffset(FLOOFY_PI / (2.0f * freq), freq, amp);
        assert(std::abs(peak - amp) < 1e-3f);
        const float trough = floofyBuzzOffset(3.0f * FLOOFY_PI / (2.0f * freq), freq, amp);
        assert(std::abs(trough - (-amp)) < 1e-3f);

        // Buzz wobble vector calculation
        const Vector2 basePos = { 300.0f, 300.0f };
        const Vector2 dirDown = { 0.0f, 1.0f };
        const Vector2 wobDown = floofyBuzzWobble(basePos, dirDown, FLOOFY_PI / (2.0f * freq), freq, amp);
        // Orthogonal to downward direction: lateral offset along X
        assert(std::abs(wobDown.y - basePos.y) < 1e-4f);
        assert(std::abs(std::abs(wobDown.x - basePos.x) - amp) < 1e-3f);

        const Vector2 dirRight = { 1.0f, 0.0f };
        const Vector2 wobRight = floofyBuzzWobble(basePos, dirRight, FLOOFY_PI / (2.0f * freq), freq, amp);
        assert(std::abs(wobRight.x - basePos.x) < 1e-4f);
        assert(std::abs(std::abs(wobRight.y - basePos.y) - amp) < 1e-3f);
    }

    // 6. Pollen Cloud & Residual Damage
    {
        FloofyPollenCloud cloud;
        assert(cloud.isActive());

        // Safe upper region of arena does not trigger cloud
        const Vector2 safePoint = { 300.0f, 270.0f };
        assert(!cloud.contains(safePoint));

        // Lower region inside cloud
        const Vector2 cloudPoint = { 300.0f, 340.0f };
        assert(cloud.contains(cloudPoint));

        // Damage zone update and residual accumulation
        const int dmg = cloud.update(0.6f, true);
        assert(dmg == cloud.damagePerTick);
        assert(cloud.residualMeter.pending >= 1.0f);

        // Residual drain
        const int drained = cloud.residualMeter.update(1.0f);
        assert(drained >= 1);
    }

    // 7. Closing Ring for pounce_ring & Bounds
    {
        FloofyPatternConfig ringCfg;
        ringCfg.patternId = FloofyPatternId::PounceRing;
        ringCfg.seed = 0x57F716u;
        const FloofyTimeline ringTimeline = generateFloofyTimeline(ringCfg, arena, soulCenter);

        assert(!ringTimeline.events.empty());
        const float spawnT = ringTimeline.events[0].spawnTime;
        assert(spawnT >= 0.0f);
        const float closeDur = ringTimeline.events[0].closeDuration;
        const float initialR = ringTimeline.events[0].initialRingRadius;
        assert(initialR > 50.0f);

        // Sample positions as time progresses
        float prevRadius = initialR + 1.0f;
        constexpr int samples = 10;
        for (int s = 0; s <= samples; ++s) {
            const float tOffset = closeDur * (static_cast<float>(s) / static_cast<float>(samples));
            const Vector2 pos = floofyBulletPositionAt(ringTimeline.events[0], tOffset);
            const float currentDist = std::hypot(pos.x - soulCenter.x, pos.y - soulCenter.y);

            // Radius must strictly decrease
            if (s > 0) {
                assert(currentDist < prevRadius);
            }
            prevRadius = currentDist;

            // Bounds check
            assert(checkFloofyBulletBounds(pos, arena, 40.0f));
        }

        // The ring closes completely: distance at closeDuration is zero
        const Vector2 closedPos = floofyBulletPositionAt(ringTimeline.events[0], closeDur);
        const float finalDist = std::hypot(closedPos.x - soulCenter.x, closedPos.y - soulCenter.y);
        assert(finalDist < 1e-3f); // Distance <= 0 px, the ring closed!
    }

    // 8. Dodgeability: careful player dodges all patterns except forced_petal
    {
        FloofyPatternConfig simCfg;
        simCfg.intensity = 1.0f;
        simCfg.turnDuration = 3.5f;

        // Patterns 1 through 7 can be dodged by a careful player
        assert(canCarefulPlayerDodge(FloofyPatternId::FriendshipPetals, simCfg, arena, soulCenter));
        assert(canCarefulPlayerDodge(FloofyPatternId::ThornVines, simCfg, arena, soulCenter));
        assert(canCarefulPlayerDodge(FloofyPatternId::PetalStorm, simCfg, arena, soulCenter));
        assert(canCarefulPlayerDodge(FloofyPatternId::PollenCloud, simCfg, arena, soulCenter));
        assert(canCarefulPlayerDodge(FloofyPatternId::BouquetBurst, simCfg, arena, soulCenter));
        assert(canCarefulPlayerDodge(FloofyPatternId::PounceRing, simCfg, arena, soulCenter));
        assert(canCarefulPlayerDodge(FloofyPatternId::TutorialPetals, simCfg, arena, soulCenter));

        // Pattern 8 (forced_petal) is UNAVOIDABLE: cannot be dodged
        assert(!canCarefulPlayerDodge(FloofyPatternId::ForcedPetal, simCfg, arena, soulCenter));
    }

    // 9. Intensity Escalation
    {
        FloofyPatternConfig lowCfg;
        lowCfg.patternId = FloofyPatternId::FriendshipPetals;
        lowCfg.intensity = 0.5f;
        const FloofyTimeline lowTimeline = generateFloofyTimeline(lowCfg, arena, soulCenter);

        FloofyPatternConfig highCfg;
        highCfg.patternId = FloofyPatternId::FriendshipPetals;
        highCfg.intensity = 2.5f;
        const FloofyTimeline highTimeline = generateFloofyTimeline(highCfg, arena, soulCenter);

        assert(highTimeline.events.size() > lowTimeline.events.size());
        assert(highTimeline.events[0].speed > lowTimeline.events[0].speed);
        assert(highTimeline.events[0].wobbleFrequency > lowTimeline.events[0].wobbleFrequency);
    }

    // 10. Bounds check on all spawned bullets across all patterns
    for (FloofyPatternId id : allPatterns) {
        FloofyPatternConfig cfg;
        cfg.patternId = id;
        const FloofyTimeline timeline = generateFloofyTimeline(cfg, arena, soulCenter);
        for (const auto& ev : timeline.events) {
            assert(checkFloofyBulletBounds(ev.startPosition, arena, 50.0f));
            // Check position after 1 second of flight
            const Vector2 pos1s = floofyBulletPositionAt(ev, 1.0f);
            assert(checkFloofyBulletBounds(pos1s, arena, 120.0f));
        }
    }

    std::cout << "[PASS] C1-027: Floofy petal patterns verified (timelines, bounds, ring closure, dodgeability)." << std::endl;
}
