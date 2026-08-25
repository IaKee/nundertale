#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include <vector>

namespace nundertale {

// ART-183 row 11 / C1-002: Furiell's water-burst / spritz effect.
// Used for battle (shoos Floofy) and overworld (heals/spritzes Grub).
// Pure logic: header-only, no Raylib dependencies, deterministic and testable.

enum class WaterBurstVariant {
    Battle,     // White droplets for battle use
    Overworld   // Light-blue tint variant for the overworld
};

struct WaterBurstColor {
    unsigned char r = 255;
    unsigned char g = 255;
    unsigned char b = 255;
    unsigned char a = 255;
};

inline WaterBurstColor waterBurstColor(WaterBurstVariant variant) {
    if (variant == WaterBurstVariant::Battle) {
        return WaterBurstColor{ 255, 255, 255, 255 };
    }
    // Light-blue water tint for overworld
    return WaterBurstColor{ 120, 205, 255, 255 };
}

inline WaterBurstColor waterBurstHighlightColor(WaterBurstVariant variant) {
    if (variant == WaterBurstVariant::Battle) {
        return WaterBurstColor{ 255, 255, 255, 255 };
    }
    return WaterBurstColor{ 240, 250, 255, 255 };
}

inline WaterBurstColor waterBurstDarkColor(WaterBurstVariant variant) {
    if (variant == WaterBurstVariant::Battle) {
        return WaterBurstColor{ 220, 220, 220, 255 };
    }
    return WaterBurstColor{ 64, 140, 210, 255 };
}

struct WaterDroplet {
    float x = 0.0f;
    float y = 0.0f;
    float vx = 0.0f;
    float vy = 0.0f;
    float size = 2.0f;
    float life = 0.0f;     // remaining lifetime in seconds
    float maxLife = 0.0f;  // initial lifetime in seconds
    float gravity = 280.0f; // downward acceleration in px/s^2
    WaterBurstColor color{ 255, 255, 255, 255 };

    [[nodiscard]] bool alive() const {
        return life > 0.0f;
    }

    [[nodiscard]] float alpha() const {
        if (maxLife <= 0.0f) {
            return 0.0f;
        }
        const float ratio = life / maxLife;
        return std::clamp(ratio, 0.0f, 1.0f);
    }
};

inline constexpr int kWaterBurstFrameCount = 3;
inline constexpr float kWaterBurstFps = 12.0f;
inline constexpr float kWaterBurstFrameDuration = 1.0f / kWaterBurstFps; // ~0.08333s
inline constexpr float kWaterBurstAnimDuration = static_cast<float>(kWaterBurstFrameCount) * kWaterBurstFrameDuration; // 0.25s
inline constexpr int kWaterBurstDefaultDropletCount = 5;
inline constexpr float kWaterBurstDefaultGravity = 280.0f;

inline constexpr std::string_view kWaterBurstPngPath = "assets/ui/water_burst.png";
inline constexpr std::string_view kWaterBurstJsonPath = "assets/ui/water_burst.json";

namespace water_burst_detail {

inline std::uint32_t nextRng(std::uint32_t& state) {
    state = state * 1664525u + 1013904223u;
    return state;
}

inline float randomFloat(std::uint32_t& state, float minVal, float maxVal) {
    const float norm = static_cast<float>(nextRng(state) & 0x00FFFFFFu) / 16777216.0f;
    return minVal + norm * (maxVal - minVal);
}

} // namespace water_burst_detail

class WaterBurst {
public:
    float originX = 0.0f;
    float originY = 0.0f;
    float elapsed = 0.0f;
    WaterBurstVariant variant = WaterBurstVariant::Battle;
    bool done = false;
    std::vector<WaterDroplet> droplets;

    float fps = kWaterBurstFps;
    int frameCount = kWaterBurstFrameCount;
    float frameDuration = kWaterBurstFrameDuration;
    float gravity = kWaterBurstDefaultGravity;

    WaterBurst() = default;

    WaterBurst(float spawnX, float spawnY,
               WaterBurstVariant var = WaterBurstVariant::Battle,
               std::uint32_t seed = 0x508172u,
               int dropletCount = kWaterBurstDefaultDropletCount) {
        spawn(spawnX, spawnY, var, seed, dropletCount);
    }

    WaterBurst(float spawnX, float spawnY,
               bool overworld,
               std::uint32_t seed = 0x508172u,
               int dropletCount = kWaterBurstDefaultDropletCount) {
        spawn(spawnX, spawnY, overworld ? WaterBurstVariant::Overworld : WaterBurstVariant::Battle,
              seed, dropletCount);
    }

    void spawn(float spawnX, float spawnY,
               WaterBurstVariant var = WaterBurstVariant::Battle,
               std::uint32_t seed = 0x508172u,
               int dropletCount = kWaterBurstDefaultDropletCount) {
        originX = spawnX;
        originY = spawnY;
        elapsed = 0.0f;
        variant = var;
        done = false;
        fps = kWaterBurstFps;
        frameCount = kWaterBurstFrameCount;
        frameDuration = 1.0f / fps;
        gravity = kWaterBurstDefaultGravity;

        droplets.clear();
        if (dropletCount <= 0) {
            return;
        }

        droplets.reserve(static_cast<std::size_t>(dropletCount));
        std::uint32_t rng = seed ^ 0x9E3779B9u;

        for (int i = 0; i < dropletCount; ++i) {
            WaterDroplet d;
            // Droplets disperse from around the impact center
            const float ox = water_burst_detail::randomFloat(rng, -3.0f, 3.0f);
            const float oy = water_burst_detail::randomFloat(rng, -3.0f, 2.0f);
            d.x = spawnX + ox;
            d.y = spawnY + oy;

            // Spritz outward spray: spread horizontally, initial upward velocity
            const float sprayVx = water_burst_detail::randomFloat(rng, -60.0f, 60.0f);
            const float sprayVy = water_burst_detail::randomFloat(rng, -120.0f, -40.0f);
            d.vx = sprayVx;
            d.vy = sprayVy;

            const float dLife = water_burst_detail::randomFloat(rng, 0.35f, 0.60f);
            d.life = dLife;
            d.maxLife = dLife;
            d.gravity = gravity;
            d.size = water_burst_detail::randomFloat(rng, 1.5f, 2.5f);

            // Assign color according to variant
            if (var == WaterBurstVariant::Battle) {
                d.color = WaterBurstColor{ 255, 255, 255, 255 };
            } else {
                const std::uint32_t pick = water_burst_detail::nextRng(rng) % 3u;
                if (pick == 0u) {
                    d.color = waterBurstHighlightColor(var);
                } else if (pick == 1u) {
                    d.color = waterBurstColor(var);
                } else {
                    d.color = waterBurstDarkColor(var);
                }
            }

            droplets.push_back(d);
        }
    }

    void spawn(float spawnX, float spawnY,
               bool overworld,
               std::uint32_t seed = 0x508172u,
               int dropletCount = kWaterBurstDefaultDropletCount) {
        spawn(spawnX, spawnY, overworld ? WaterBurstVariant::Overworld : WaterBurstVariant::Battle,
              seed, dropletCount);
    }

    void update(float dt) {
        if (done || dt <= 0.0f) {
            return;
        }

        elapsed += dt;

        bool anyDropletAlive = false;
        for (auto& d : droplets) {
            if (d.life > 0.0f) {
                d.life -= dt;
                if (d.life < 0.0f) {
                    d.life = 0.0f;
                }
                d.vy += d.gravity * dt;
                d.x += d.vx * dt;
                d.y += d.vy * dt;
                if (d.life > 0.0f) {
                    anyDropletAlive = true;
                }
            }
        }

        // Done when all 3 animation frames have played AND all falling droplets have expired
        if (isAnimDone() && !anyDropletAlive) {
            done = true;
        }
    }

    [[nodiscard]] bool isDone() const {
        return done;
    }

    [[nodiscard]] float animDuration() const {
        return static_cast<float>(frameCount) * frameDuration;
    }

    [[nodiscard]] bool isAnimDone() const {
        return elapsed >= animDuration();
    }

    // Returns frame index 0, 1, or 2 clamped within [0, frameCount - 1]
    [[nodiscard]] int currentFrame() const {
        if (elapsed <= 0.0f) {
            return 0;
        }
        const int frameIdx = static_cast<int>(elapsed / frameDuration);
        return std::clamp(frameIdx, 0, frameCount - 1);
    }

    // Returns frame index 0, 1, or 2 while active, or -1 if the 3 frames have completed
    [[nodiscard]] int activeAnimFrame() const {
        if (elapsed < 0.0f || elapsed >= animDuration()) {
            return -1;
        }
        const int frameIdx = static_cast<int>(elapsed / frameDuration);
        return std::clamp(frameIdx, 0, frameCount - 1);
    }

    [[nodiscard]] float getOriginX() const { return originX; }
    [[nodiscard]] float getOriginY() const { return originY; }
    [[nodiscard]] float getX() const { return originX; }
    [[nodiscard]] float getY() const { return originY; }
    [[nodiscard]] float getElapsed() const { return elapsed; }
    [[nodiscard]] WaterBurstVariant getVariant() const { return variant; }
    [[nodiscard]] bool isOverworld() const { return variant == WaterBurstVariant::Overworld; }

    [[nodiscard]] const std::vector<WaterDroplet>& getDroplets() const { return droplets; }
    std::vector<WaterDroplet>& getDroplets() { return droplets; }

    [[nodiscard]] std::string_view animName() const {
        return (variant == WaterBurstVariant::Battle) ? "battle" : "overworld";
    }

    [[nodiscard]] std::string_view spriteSheetPath() const {
        return kWaterBurstPngPath;
    }

    [[nodiscard]] std::string_view spriteSheetJson() const {
        return kWaterBurstJsonPath;
    }

    void reset() {
        elapsed = 0.0f;
        done = false;
        droplets.clear();
    }
};

} // namespace nundertale
