#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>
#include <raylib.h>

#include "combat/Bullet.hpp"
#include "combat/DamageZone.hpp"
#include "combat/ResidualDamage.hpp"

namespace nundertale {

inline constexpr float FLOOFY_PI = 3.14159265358979323846f;
inline constexpr const char* FLOOFY_SPRITE_FALLBACK = "assets/bullets/petal.png";

// C1-027: The 8 named patterns for Floofy the Floof
enum class FloofyPatternId {
    FriendshipPetals, // Buzzing rose-toy petals that drift and wobble
    ThornVines,       // Thorny rose vines sweeping horizontally from sides
    PetalStorm,       // Spiral of spiked petals
    PollenCloud,      // Lingering residual-damage cloud in the arena
    BouquetBurst,     // Stationary central bouquet bursting into thorns
    PounceRing,       // Closing ring of petals (Turn 6 before Furiell spray)
    TutorialPetals,   // Slow, highly dodgeable petals for fake tutorial
    ForcedPetal       // Fast, unavoidable tutorial hit after "RUN. INTO. THE. PETALS!"
};

inline const char* floofyPatternIdToString(FloofyPatternId id) {
    switch (id) {
        case FloofyPatternId::FriendshipPetals: return "friendship_petals";
        case FloofyPatternId::ThornVines:        return "thorn_vines";
        case FloofyPatternId::PetalStorm:        return "petal_storm";
        case FloofyPatternId::PollenCloud:       return "pollen_cloud";
        case FloofyPatternId::BouquetBurst:      return "bouquet_burst";
        case FloofyPatternId::PounceRing:        return "pounce_ring";
        case FloofyPatternId::TutorialPetals:    return "tutorial_petals";
        case FloofyPatternId::ForcedPetal:       return "forced_petal";
    }
    return "";
}

inline bool floofyPatternIdFromString(const std::string& name, FloofyPatternId& outId) {
    if (name == "friendship_petals") { outId = FloofyPatternId::FriendshipPetals; return true; }
    if (name == "thorn_vines")        { outId = FloofyPatternId::ThornVines; return true; }
    if (name == "petal_storm")        { outId = FloofyPatternId::PetalStorm; return true; }
    if (name == "pollen_cloud")       { outId = FloofyPatternId::PollenCloud; return true; }
    if (name == "bouquet_burst")      { outId = FloofyPatternId::BouquetBurst; return true; }
    if (name == "pounce_ring")        { outId = FloofyPatternId::PounceRing; return true; }
    if (name == "tutorial_petals")    { outId = FloofyPatternId::TutorialPetals; return true; }
    if (name == "forced_petal")       { outId = FloofyPatternId::ForcedPetal; return true; }
    return false;
}

inline bool isFloofyPatternName(const std::string& name) {
    FloofyPatternId ignored = FloofyPatternId::FriendshipPetals;
    return floofyPatternIdFromString(name, ignored);
}

// Preferred sprite path per contracts.md
inline const char* floofyPatternPreferredSprite(FloofyPatternId id) {
    switch (id) {
        case FloofyPatternId::FriendshipPetals: return "assets/sprites/bullets/petal.png";
        case FloofyPatternId::ThornVines:        return "assets/sprites/bullets/rose_vine.png";
        case FloofyPatternId::PetalStorm:        return "assets/sprites/bullets/thorn_petal.png";
        case FloofyPatternId::PollenCloud:       return "assets/sprites/bullets/pollen.png";
        case FloofyPatternId::BouquetBurst:      return "assets/sprites/bullets/thorn_petal.png";
        case FloofyPatternId::PounceRing:        return "assets/sprites/bullets/petal.png";
        case FloofyPatternId::TutorialPetals:    return "assets/sprites/bullets/petal.png";
        case FloofyPatternId::ForcedPetal:       return "assets/sprites/bullets/petal.png";
    }
    return "assets/sprites/bullets/petal.png";
}

// Fallback to existing sprites if missing on disk
inline std::string resolveFloofySprite(const std::string& preferred,
                                       const std::string& fallback = FLOOFY_SPRITE_FALLBACK) {
    if (preferred.empty()) return fallback;
    std::ifstream file(preferred);
    if (file.good()) {
        return preferred;
    }
    return fallback;
}

inline std::string floofyPatternSprite(FloofyPatternId id, const std::string& customSprite = "") {
    if (!customSprite.empty()) {
        return resolveFloofySprite(customSprite, FLOOFY_SPRITE_FALLBACK);
    }
    return resolveFloofySprite(floofyPatternPreferredSprite(id), FLOOFY_SPRITE_FALLBACK);
}

// Deterministic Pseudo-Random Number Generator for seed-based repeatability
struct FloofyRng {
    std::uint32_t state = 0x57F716u;

    constexpr explicit FloofyRng(std::uint32_t seed = 0u)
        : state(seed == 0u ? 0x57F716u : seed) {}

    constexpr std::uint32_t next() {
        state ^= state << 13;
        state ^= state >> 17;
        state ^= state << 5;
        return state;
    }

    constexpr float nextFloat() {
        return static_cast<float>(next() & 0x00FFFFFFu) / 16777216.0f;
    }

    constexpr float rangeFloat(float minVal, float maxVal) {
        return minVal + (maxVal - minVal) * nextFloat();
    }

    constexpr int rangeInt(int minVal, int maxVal) {
        if (minVal >= maxVal) return minVal;
        const std::uint32_t span = static_cast<std::uint32_t>(maxVal - minVal + 1);
        return minVal + static_cast<int>(next() % span);
    }
};

// Configuration struct with intensity parameters for escalation
struct FloofyPatternConfig {
    FloofyPatternId patternId = FloofyPatternId::FriendshipPetals;
    float intensity = 1.0f;          // Escalation multiplier (turn 1 to 6)
    float speed = 0.0f;              // 0 = automatic default from intensity
    int damage = 3;                  // 3-4 dmg against 20 HP
    int fur = 8;                     // FUR gained on hit (8 baseline, 20 in tutorial/forced)
    int count = 0;                   // 0 = automatic default from intensity
    std::uint32_t seed = 0x57F716u;  // Deterministic seed
    std::string customSprite = "";
    float turnDuration = 5.0f;       // Standard turn duration
};

// Motion: Buzz wobble calculation (rose-toy vibration)
inline float floofyBuzzOffset(float age, float frequency = 26.0f, float amplitude = 8.0f) {
    return std::sin(age * frequency) * amplitude;
}

inline Vector2 floofyBuzzWobble(Vector2 basePos, Vector2 travelDirection, float age,
                                float frequency = 26.0f, float amplitude = 8.0f) {
    const float len = std::sqrt(travelDirection.x * travelDirection.x + travelDirection.y * travelDirection.y);
    if (len < 1e-4f) {
        return { basePos.x + floofyBuzzOffset(age, frequency, amplitude), basePos.y };
    }
    const float perpX = -travelDirection.y / len;
    const float perpY = travelDirection.x / len;
    const float offset = floofyBuzzOffset(age, frequency, amplitude);
    return { basePos.x + perpX * offset, basePos.y + perpY * offset };
}

// A single spawn event on the pattern timeline
struct FloofySpawnEvent {
    float spawnTime = 0.0f;
    Vector2 startPosition{ 0.0f, 0.0f };
    Vector2 initialVelocity{ 0.0f, 0.0f };
    float speed = 0.0f;
    float angle = 0.0f;
    float radius = 5.0f;
    float wobbleFrequency = 26.0f;
    float wobbleAmplitude = 8.0f;
    int damage = 3;
    int fur = 8;
    std::string spritePath;
    FloofyPatternId patternId = FloofyPatternId::FriendshipPetals;

    // Pounce ring metadata
    Vector2 targetCenter{ 320.0f, 320.0f };
    float initialRingRadius = 70.0f;
    float closeDuration = 2.0f;
    bool isRing = false;

    // Bouquet burst metadata
    float burstDelay = 0.8f;
    bool isBouquet = false;

    // Homing metadata
    bool homes = false;
    float homingSpeed = 0.0f;
};

// Timeline containing all scheduled spawns for a turn
struct FloofyTimeline {
    FloofyPatternId patternId = FloofyPatternId::FriendshipPetals;
    float totalDuration = 5.0f;
    std::vector<FloofySpawnEvent> events;
};

// Deterministic calculation of bullet position at any point in time
inline Vector2 floofyBulletPositionAt(const FloofySpawnEvent& event, float timeSinceSpawn) {
    if (timeSinceSpawn < 0.0f) {
        return event.startPosition;
    }

    if (event.isRing) {
        // Closing ring: radius strictly contracts towards targetCenter
        const float progress = std::clamp(timeSinceSpawn / std::max(0.1f, event.closeDuration), 0.0f, 1.0f);
        const float currentRadius = event.initialRingRadius * (1.0f - progress);
        return {
            event.targetCenter.x + std::cos(event.angle) * currentRadius,
            event.targetCenter.y + std::sin(event.angle) * currentRadius
        };
    }

    if (event.isBouquet) {
        // Bouquet stays stationary during windup, then bursts outward
        if (timeSinceSpawn < event.burstDelay) {
            return event.startPosition;
        }
        const float travelTime = timeSinceSpawn - event.burstDelay;
        return {
            event.startPosition.x + event.initialVelocity.x * travelTime,
            event.startPosition.y + event.initialVelocity.y * travelTime
        };
    }

    // Default linear motion + buzz wobble
    Vector2 basePos = {
        event.startPosition.x + event.initialVelocity.x * timeSinceSpawn,
        event.startPosition.y + event.initialVelocity.y * timeSinceSpawn
    };
    if (event.wobbleAmplitude > 0.0f) {
        return floofyBuzzWobble(basePos, event.initialVelocity, timeSinceSpawn,
                                event.wobbleFrequency, event.wobbleAmplitude);
    }
    return basePos;
}

// Residual damage cloud for pollen_cloud
struct FloofyPollenCloud {
    Rectangle area{ 242.0f, 310.0f, 155.0f, 75.0f }; // Lower portion of arena
    float lifetime = 3.5f;
    float age = 0.0f;
    float tickInterval = 0.5f;
    int damagePerTick = 1;
    int furPerTick = 2;
    float tickTimer = 0.0f;
    ResidualMeter residualMeter;

    bool isActive() const { return age < lifetime; }

    bool contains(Vector2 point) const {
        return point.x >= area.x && point.x <= area.x + area.width &&
               point.y >= area.y && point.y <= area.y + area.height;
    }

    int update(float dt, bool soulInside) {
        age += dt;
        if (!isActive()) return 0;
        tickTimer -= dt;
        if (soulInside && tickTimer <= 0.0f) {
            tickTimer = tickInterval;
            residualMeter.add(static_cast<float>(damagePerTick));
            return damagePerTick;
        }
        return 0;
    }
};

// Generates the deterministic spawn timeline for any Floofy pattern given a configuration
inline FloofyTimeline generateFloofyTimeline(const FloofyPatternConfig& config,
                                             const Rectangle& arena = { 237.0f, 250.0f, 165.0f, 140.0f },
                                             Vector2 soulPos = { 320.0f, 320.0f }) {
    FloofyTimeline timeline;
    timeline.patternId = config.patternId;
    timeline.totalDuration = config.turnDuration;

    FloofyRng rng(config.seed);
    const std::string sprite = floofyPatternSprite(config.patternId, config.customSprite);
    const float intensity = std::clamp(config.intensity, 0.2f, 5.0f);

    switch (config.patternId) {
        case FloofyPatternId::FriendshipPetals: {
            const int baseCount = config.count > 0 ? config.count : 10;
            const int totalCount = std::max(4, static_cast<int>(std::round(static_cast<float>(baseCount) * (0.8f + 0.2f * intensity))));
            const float baseSpeed = config.speed > 0.0f ? config.speed : 65.0f;
            const float speed = baseSpeed * (0.85f + 0.15f * intensity);
            const float interval = 4.0f / static_cast<float>(totalCount);

            for (int i = 0; i < totalCount; ++i) {
                FloofySpawnEvent ev;
                ev.patternId = FloofyPatternId::FriendshipPetals;
                ev.spawnTime = 0.2f + static_cast<float>(i) * interval;
                ev.startPosition = {
                    rng.rangeFloat(arena.x + 15.0f, arena.x + arena.width - 15.0f),
                    arena.y - 12.0f
                };
                ev.initialVelocity = {
                    rng.rangeFloat(-8.0f, 8.0f),
                    speed
                };
                ev.speed = speed;
                ev.wobbleFrequency = 24.0f + 2.0f * intensity;
                ev.wobbleAmplitude = 8.0f + 1.0f * intensity;
                ev.damage = config.damage > 0 ? config.damage : 3;
                ev.fur = config.fur > 0 ? config.fur : 8;
                ev.spritePath = sprite;
                timeline.events.push_back(ev);
            }
            break;
        }

        case FloofyPatternId::ThornVines: {
            // Sweeps from alternating sides with guaranteed vertical gap for dodging
            const int baseSweeps = config.count > 0 ? config.count : 3;
            const int sweeps = std::max(2, static_cast<int>(std::round(static_cast<float>(baseSweeps) * (0.8f + 0.2f * intensity))));
            const float baseSpeed = config.speed > 0.0f ? config.speed : 80.0f;
            const float sweepSpeed = baseSpeed * (0.85f + 0.15f * intensity);
            const float sweepInterval = 3.8f / static_cast<float>(sweeps);
            const float gapHeight = std::max(45.0f, 58.0f - 3.0f * intensity);

            for (int s = 0; s < sweeps; ++s) {
                const bool fromLeft = (s % 2) == 0;
                const float spawnTime = 0.4f + static_cast<float>(s) * sweepInterval;
                const float startX = fromLeft ? arena.x - 10.0f : arena.x + arena.width + 10.0f;
                const float vx = fromLeft ? sweepSpeed : -sweepSpeed;

                const float gapTop = rng.rangeFloat(arena.y + 12.0f, arena.y + arena.height - 12.0f - gapHeight);

                for (float y = arena.y + 10.0f; y <= arena.y + arena.height - 10.0f; y += 14.0f) {
                    if (y >= gapTop && y <= gapTop + gapHeight) {
                        continue; // Safe gap for careful player
                    }
                    FloofySpawnEvent ev;
                    ev.patternId = FloofyPatternId::ThornVines;
                    ev.spawnTime = spawnTime;
                    ev.startPosition = { startX, y };
                    ev.initialVelocity = { vx, 0.0f };
                    ev.speed = sweepSpeed;
                    ev.wobbleAmplitude = 0.0f; // Vines stay rigid
                    ev.damage = config.damage > 0 ? config.damage : 3;
                    ev.fur = config.fur > 0 ? config.fur : 8;
                    ev.spritePath = sprite;
                    timeline.events.push_back(ev);
                }
            }
            break;
        }

        case FloofyPatternId::PetalStorm: {
            // Spiral of spiked petals with angular spacing between arms
            const int baseCount = config.count > 0 ? config.count : 22;
            const int totalPetals = std::max(12, static_cast<int>(std::round(static_cast<float>(baseCount) * (0.8f + 0.2f * intensity))));
            const float baseSpeed = config.speed > 0.0f ? config.speed : 75.0f;
            const float speed = baseSpeed * (0.85f + 0.15f * intensity);
            const float interval = 3.8f / static_cast<float>(totalPetals);
            const Vector2 origin = { arena.x + arena.width * 0.5f, arena.y + 15.0f };
            const float angleStep = 0.45f;
            const float startAngle = rng.rangeFloat(0.0f, 2.0f * FLOOFY_PI);

            for (int i = 0; i < totalPetals; ++i) {
                const float ang = startAngle + static_cast<float>(i) * angleStep;
                FloofySpawnEvent ev;
                ev.patternId = FloofyPatternId::PetalStorm;
                ev.spawnTime = 0.3f + static_cast<float>(i) * interval;
                ev.startPosition = origin;
                ev.initialVelocity = { std::cos(ang) * speed, std::sin(ang) * speed };
                ev.angle = ang;
                ev.speed = speed;
                ev.wobbleAmplitude = 3.0f;
                ev.wobbleFrequency = 20.0f;
                ev.damage = config.damage > 0 ? config.damage : 3;
                ev.fur = config.fur > 0 ? config.fur : 8;
                ev.spritePath = sprite;
                timeline.events.push_back(ev);
            }
            break;
        }

        case FloofyPatternId::PollenCloud: {
            // Floating pollen particles inside the residual cloud zone
            const int baseCount = config.count > 0 ? config.count : 14;
            const int totalParticles = std::max(6, static_cast<int>(std::round(static_cast<float>(baseCount) * intensity)));
            const float cloudTop = arena.y + arena.height * 0.45f; // Upper 45% is completely safe
            const float interval = 2.5f / static_cast<float>(totalParticles);

            for (int i = 0; i < totalParticles; ++i) {
                FloofySpawnEvent ev;
                ev.patternId = FloofyPatternId::PollenCloud;
                ev.spawnTime = 0.2f + static_cast<float>(i) * interval;
                ev.startPosition = {
                    rng.rangeFloat(arena.x + 10.0f, arena.x + arena.width - 10.0f),
                    rng.rangeFloat(cloudTop + 5.0f, arena.y + arena.height - 10.0f)
                };
                ev.initialVelocity = {
                    rng.rangeFloat(-12.0f, 12.0f),
                    rng.rangeFloat(-8.0f, 8.0f)
                };
                ev.speed = 15.0f;
                ev.wobbleFrequency = 14.0f;
                ev.wobbleAmplitude = 5.0f;
                ev.damage = 1; // Residual tick damage
                ev.fur = 2;
                ev.spritePath = sprite;
                timeline.events.push_back(ev);
            }
            break;
        }

        case FloofyPatternId::BouquetBurst: {
            // Stationary bouquet in the center bursting into radial thorns
            const int bursts = intensity > 1.5f ? 2 : 1;
            const int thornsPerBurst = config.count > 0 ? config.count : 10;
            const float baseSpeed = config.speed > 0.0f ? config.speed : 85.0f;
            const float speed = baseSpeed * (0.85f + 0.15f * intensity);
            const Vector2 center = { arena.x + arena.width * 0.5f, arena.y + arena.height * 0.5f };

            for (int b = 0; b < bursts; ++b) {
                const float spawnTime = 0.3f + static_cast<float>(b) * 2.2f;
                const float startAngle = rng.rangeFloat(0.0f, 2.0f * FLOOFY_PI);
                const float angleStep = (2.0f * FLOOFY_PI) / static_cast<float>(thornsPerBurst);

                for (int t = 0; t < thornsPerBurst; ++t) {
                    const float ang = startAngle + static_cast<float>(t) * angleStep;
                    FloofySpawnEvent ev;
                    ev.patternId = FloofyPatternId::BouquetBurst;
                    ev.spawnTime = spawnTime;
                    ev.startPosition = center;
                    ev.initialVelocity = { std::cos(ang) * speed, std::sin(ang) * speed };
                    ev.speed = speed;
                    ev.angle = ang;
                    ev.isBouquet = true;
                    ev.burstDelay = 0.8f; // 0.8s stationary windup before burst
                    ev.wobbleAmplitude = 0.0f;
                    ev.damage = config.damage > 0 ? config.damage : 4;
                    ev.fur = config.fur > 0 ? config.fur : 8;
                    ev.spritePath = sprite;
                    timeline.events.push_back(ev);
                }
            }
            break;
        }

        case FloofyPatternId::PounceRing: {
            // Turn 6: Closing ring of petals with a deterministic escape gap
            const int ringCount = config.count > 0 ? config.count : 16;
            const float initialRadius = 72.0f;
            const float closeDuration = 2.0f;
            const Vector2 targetCenter = soulPos;
            const int gapStart = rng.rangeInt(0, ringCount - 1);
            const int gapSize = 3; // 3-petal gap allows careful player escape

            for (int i = 0; i < ringCount; ++i) {
                // Check if index falls in the escape gap
                const int diff = (i - gapStart + ringCount) % ringCount;
                if (diff < gapSize) {
                    continue; // Skip gap petals
                }
                const float ang = (static_cast<float>(i) / static_cast<float>(ringCount)) * 2.0f * FLOOFY_PI;
                FloofySpawnEvent ev;
                ev.patternId = FloofyPatternId::PounceRing;
                ev.spawnTime = 0.5f;
                ev.startPosition = {
                    targetCenter.x + std::cos(ang) * initialRadius,
                    targetCenter.y + std::sin(ang) * initialRadius
                };
                ev.angle = ang;
                ev.isRing = true;
                ev.targetCenter = targetCenter;
                ev.initialRingRadius = initialRadius;
                ev.closeDuration = closeDuration;
                ev.wobbleAmplitude = 0.0f;
                ev.damage = config.damage > 0 ? config.damage : 4;
                ev.fur = config.fur > 0 ? config.fur : 10;
                ev.spritePath = sprite;
                timeline.events.push_back(ev);
            }
            break;
        }

        case FloofyPatternId::TutorialPetals: {
            // Slow, widely spaced, gentle drift
            const int totalPetals = 4;
            const float speed = 32.0f;

            for (int i = 0; i < totalPetals; ++i) {
                FloofySpawnEvent ev;
                ev.patternId = FloofyPatternId::TutorialPetals;
                ev.spawnTime = 0.3f + static_cast<float>(i) * 0.9f;
                ev.startPosition = {
                    arena.x + 25.0f + static_cast<float>(i) * (arena.width - 50.0f) / 3.0f,
                    arena.y - 10.0f
                };
                ev.initialVelocity = { 0.0f, speed };
                ev.speed = speed;
                ev.wobbleFrequency = 16.0f;
                ev.wobbleAmplitude = 4.0f; // Very gentle wobble
                ev.damage = 2;
                ev.fur = 5;
                ev.spritePath = sprite;
                timeline.events.push_back(ev);
            }
            break;
        }

        case FloofyPatternId::ForcedPetal: {
            // Unavoidable tutorial hit: dense converging ring + inescapable barrage
            const int ringCount = 28;
            const float closeDuration = 0.35f; // Closes in 0.35s (player moves at most 42px in that time)
            const float initialRadius = 55.0f;
            const Vector2 targetCenter = soulPos;

            // Concentric complete ring with 0 gaps
            for (int i = 0; i < ringCount; ++i) {
                const float ang = (static_cast<float>(i) / static_cast<float>(ringCount)) * 2.0f * FLOOFY_PI;
                FloofySpawnEvent ev;
                ev.patternId = FloofyPatternId::ForcedPetal;
                ev.spawnTime = 0.1f;
                ev.startPosition = {
                    targetCenter.x + std::cos(ang) * initialRadius,
                    targetCenter.y + std::sin(ang) * initialRadius
                };
                ev.angle = ang;
                ev.isRing = true;
                ev.targetCenter = targetCenter;
                ev.initialRingRadius = initialRadius;
                ev.closeDuration = closeDuration;
                ev.wobbleAmplitude = 0.0f;
                ev.damage = 5; // ~25% of 20 HP
                ev.fur = 20;   // LORE §19.0: FUR +20 on forced hit
                ev.spritePath = sprite;
                timeline.events.push_back(ev);
            }
            break;
        }
    }

    std::stable_sort(timeline.events.begin(), timeline.events.end(),
                     [](const FloofySpawnEvent& a, const FloofySpawnEvent& b) {
                         return a.spawnTime < b.spawnTime;
                     });

    return timeline;
}

// Convert Floofy spawn timeline events to standard nUndertale Bullet instances
inline std::vector<Bullet> spawnFloofyBulletsAt(const FloofyTimeline& timeline,
                                                float windowStart,
                                                float windowEnd) {
    std::vector<Bullet> spawned;
    for (const FloofySpawnEvent& ev : timeline.events) {
        if (ev.spawnTime >= windowStart && ev.spawnTime < windowEnd) {
            Bullet b;
            b.position = ev.startPosition;
            b.velocity = ev.initialVelocity;
            b.radius = ev.radius;
            b.damage = ev.damage;
            b.fur = ev.fur;
            b.spritePath = ev.spritePath;
            b.color = WHITE;
            spawned.push_back(b);
        }
    }
    return spawned;
}

// Check if a point is bounded within the arena plus margin
inline bool checkFloofyBulletBounds(Vector2 pos, const Rectangle& arena, float margin = 40.0f) {
    return (pos.x >= arena.x - margin &&
            pos.x <= arena.x + arena.width + margin &&
            pos.y >= arena.y - margin &&
            pos.y <= arena.y + arena.height + margin);
}

// Deterministic Dodgeability Simulation
// Simulates a careful player with 8-directional movement (120 px/s, 4 px/frame at 30 fps)
// Returns true if the careful player survives the pattern with 0 hits, false if hit.
inline bool canCarefulPlayerDodge(FloofyPatternId patternId,
                                  const FloofyPatternConfig& config,
                                  const Rectangle& arena,
                                  Vector2 initialSoulPos) {
    FloofyPatternConfig runConfig = config;
    runConfig.patternId = patternId;
    const FloofyTimeline timeline = generateFloofyTimeline(runConfig, arena, initialSoulPos);

    constexpr float dt = 1.0f / 30.0f; // 30 fps
    constexpr float soulSpeedPerFrame = 4.0f; // 4 px per frame
    constexpr float soulPadding = 12.0f;
    constexpr float soulHitboxRadius = 4.0f;
    const float minX = arena.x + soulPadding;
    const float maxX = arena.x + arena.width - soulPadding;
    const float minY = arena.y + soulPadding;
    const float maxY = arena.y + arena.height - soulPadding;

    Vector2 soul = initialSoulPos;
    const float totalTime = timeline.totalDuration;
    int steps = static_cast<int>(totalTime / dt);

    for (int step = 0; step < steps; ++step) {
        const float currentTime = static_cast<float>(step) * dt;

        // Gather currently active bullets
        struct ActiveBullet {
            Vector2 pos;
            float radius;
        };
        std::vector<ActiveBullet> activeBullets;
        for (const FloofySpawnEvent& ev : timeline.events) {
            if (currentTime >= ev.spawnTime) {
                const float age = currentTime - ev.spawnTime;
                if (ev.isBouquet && age < ev.burstDelay) {
                    continue; // Bouquet windup phase: thorns have not burst outward yet
                }
                Vector2 pos = floofyBulletPositionAt(ev, age);
                if (checkFloofyBulletBounds(pos, arena, 30.0f)) {
                    activeBullets.push_back({ pos, ev.radius });
                }
            }
        }

        // Careful player heuristic: evaluate 9 candidate moves (still + 8 directions)
        // Choose the candidate that maximizes distance to closest bullet and stays in safe zones
        const float dxTable[9] = { 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, -0.7071f, 0.7071f, -0.7071f, 0.7071f };
        const float dyTable[9] = { 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, -0.7071f, -0.7071f, 0.7071f, 0.7071f };

        Vector2 bestPos = soul;
        float bestScore = -1e9f;

        for (int m = 0; m < 9; ++m) {
            Vector2 cand = {
                std::clamp(soul.x + dxTable[m] * soulSpeedPerFrame, minX, maxX),
                std::clamp(soul.y + dyTable[m] * soulSpeedPerFrame, minY, maxY)
            };

            float minDist = 1000.0f;
            for (const ActiveBullet& b : activeBullets) {
                const float dist = std::hypot(cand.x - b.pos.x, cand.y - b.pos.y);
                if (dist < minDist) {
                    minDist = dist;
                }
            }

            float score = activeBullets.empty() ? 0.0f : minDist;

            // Pattern-specific strategic bonus:
            // For pollen_cloud: strongly prefer staying in the upper half (safe zone)
            if (patternId == FloofyPatternId::PollenCloud) {
                const float safeThreshold = arena.y + arena.height * 0.40f;
                if (cand.y <= safeThreshold) {
                    score += 50.0f;
                }
            }
            // For bouquet_burst: move away from center
            if (patternId == FloofyPatternId::BouquetBurst) {
                const Vector2 center = { arena.x + arena.width * 0.5f, arena.y + arena.height * 0.5f };
                const float distFromCenter = std::hypot(cand.x - center.x, cand.y - center.y);
                score += distFromCenter * 2.0f;
            }
            // For pounce_ring: move away from contracting center
            if (patternId == FloofyPatternId::PounceRing) {
                const float distFromCenter = std::hypot(cand.x - initialSoulPos.x, cand.y - initialSoulPos.y);
                score += distFromCenter * 2.0f;
            }

            if (score > bestScore) {
                bestScore = score;
                bestPos = cand;
            }
        }

        soul = bestPos;

        // Check collision against all active bullets at bestPos
        for (const ActiveBullet& b : activeBullets) {
            const float dist = std::hypot(soul.x - b.pos.x, soul.y - b.pos.y);
            if (dist < (soulHitboxRadius + b.radius)) {
                return false; // Hit detected!
            }
        }
    }

    return true; // Survived without taking damage!
}

} // namespace nundertale
