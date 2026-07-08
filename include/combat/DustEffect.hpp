#pragma once

// C1-012 / C1-022 (LORE §8.8, BND-003): killed furries disintegrate into plush stuffing + glitter.
// exGamer enemies (Barky, Napstablurk...) also release one red spark that flies off toward the castle.
// Pure logic: no Raylib, deterministic from the seed.

#include <cmath>
#include <cstdint>
#include <string_view>
#include <vector>

namespace nundertale {

enum class DustKind { Stuffing, Glitter, RedSpark };

struct DustRgb {
    unsigned char r = 255;
    unsigned char g = 255;
    unsigned char b = 255;
};

struct DustSeed {
    float x = 0.0f;
    float y = 0.0f;
    float vx = 0.0f;
    float vy = 0.0f;
    float size = 4.0f;
    float life = 1.0f; // alpha fade multiplier: higher fades faster
    DustKind kind = DustKind::Stuffing;
    DustRgb color;
};

inline constexpr float kDustCastleDirX = 1.0f; // the castle is off to the right of the screen

// Enemies the LORE files under exGamer; used when the enemy JSON carries no explicit "exGamer" field.
inline bool isExGamerEnemyId(std::string_view id) {
    return id == "barky" || id == "napstablurk";
}

namespace dust_detail {
inline std::uint32_t next(std::uint32_t& s) {
    s = s * 1664525u + 1013904223u;
    return s >> 8;
}
inline float unit(std::uint32_t& s) { // [0, 1)
    return static_cast<float>(next(s) & 0xFFFFu) / 65536.0f;
}
inline float range(std::uint32_t& s, float lo, float hi) { return lo + (hi - lo) * unit(s); }
} // namespace dust_detail

// `count` plush/glitter particles (about 2/3 stuffing, 1/3 glitter; at least one of each when count >= 2),
// plus exactly one red spark when exGamer is true. Negative counts yield no plush particles.
inline std::vector<DustSeed> generateDustEffect(std::uint32_t seed, int count, bool exGamer,
                                                float cx = 0.0f, float cy = 0.0f) {
    std::vector<DustSeed> out;
    if (count < 0) count = 0;
    out.reserve(static_cast<std::size_t>(count) + 1u);
    std::uint32_t s = seed ^ 0x9E3779B9u;
    const int glitterCount = count >= 2 ? (count + 2) / 3 : 0;
    for (int i = 0; i < count; ++i) {
        DustSeed p;
        const bool glitter = i < glitterCount;
        const float angle = dust_detail::range(s, 0.0f, 6.2831853f);
        const float spd = dust_detail::range(s, 30.0f, 130.0f);
        p.x = cx;
        p.y = cy;
        p.vx = std::cos(angle) * spd;
        p.vy = std::sin(angle) * spd - 35.0f;
        if (glitter) {
            p.kind = DustKind::Glitter;
            p.size = dust_detail::range(s, 2.0f, 4.0f);
            p.life = 0.8f;
            p.color = (dust_detail::next(s) & 1u) ? DustRgb{ 255, 170, 220 } : DustRgb{ 255, 255, 255 };
        } else {
            p.kind = DustKind::Stuffing;
            p.size = dust_detail::range(s, 4.0f, 8.0f);
            p.life = 0.6f;
            p.color = (dust_detail::next(s) % 3u == 0u) ? DustRgb{ 255, 214, 235 } : DustRgb{ 245, 240, 235 };
        }
        out.push_back(p);
    }
    if (exGamer) {
        DustSeed spark;
        spark.kind = DustKind::RedSpark;
        spark.x = cx;
        spark.y = cy;
        spark.vx = kDustCastleDirX * dust_detail::range(s, 160.0f, 220.0f);
        spark.vy = dust_detail::range(s, -120.0f, -70.0f);
        spark.size = 5.0f;
        spark.life = 0.35f;
        spark.color = DustRgb{ 255, 40, 40 };
        out.push_back(spark);
    }
    return out;
}

} // namespace nundertale
