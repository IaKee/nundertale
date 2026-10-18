#pragma once

#include "combat/Bullet.hpp"
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <raylib.h>

namespace nundertale {

struct GenericPatternDef {
    std::string id;
    std::string kind = "rain"; // rain, bounce, ring, wall, aimed, hop
    int count = 4;
    float speed = 80.0f;
    float duration = 5.0f;
    int damage = 3;
    int fur = 3;
    std::string bullet = ":3"; // text or sprite path
};

inline std::vector<Bullet> spawnGenericPattern(
    const GenericPatternDef& def,
    const Rectangle& box,
    const Vector2& target = { 320.0f, 320.0f },
    int step = 0)
{
    std::vector<Bullet> bullets;
    const int n = std::max(1, def.count);
    bullets.reserve(static_cast<size_t>(n));

    const bool isSprite = (def.bullet.find('/') != std::string::npos ||
                           def.bullet.find(".png") != std::string::npos ||
                           def.bullet == "fly" || def.bullet == "moth" ||
                           def.bullet == "gel_hammer" || def.bullet == "colon_three");

    for (int i = 0; i < n; ++i) {
        Bullet b;
        b.damage = def.damage;
        b.fur = def.fur;
        b.active = true;

        if (isSprite) {
            b.type = BulletType::Normal;
            b.text = def.bullet;
        } else {
            b.type = BulletType::UwU;
            b.text = def.bullet;
        }

        if (def.kind == "hop") {
            // Arcs hopping off the arena floor
            const float padding = 16.0f;
            const float span = std::max(0.0f, box.width - 2.0f * padding);
            const float x = (n == 1) ? (box.x + box.width * 0.5f)
                                     : (box.x + padding + span * (static_cast<float>(i) / static_cast<float>(n - 1)));
            const float y = box.y + box.height - 18.0f;
            b.position = { x, y };

            const float dir = (i % 2 == 0) ? 1.0f : -1.0f;
            b.velocity = { dir * def.speed * 0.45f, -def.speed };
            b.gravity = 140.0f;
            b.bounces = true;
            b.radius = 6.0f;
        } else if (def.kind == "bounce") {
            // Bouncing blobs inside the box
            const float padding = 18.0f;
            const float spanX = std::max(0.0f, box.width - 2.0f * padding);
            const float spanY = std::max(0.0f, box.height - 2.0f * padding);
            const float fx = (n == 1) ? 0.5f : (static_cast<float>(i) / static_cast<float>(n - 1));
            const float fy = (n == 1) ? 0.4f : (0.25f + 0.5f * static_cast<float>((i * 2 + 1) % n) / static_cast<float>(n));
            b.position = { box.x + padding + spanX * fx, box.y + padding + spanY * fy };

            const float angle = 0.785f + static_cast<float>(i) * (6.2831853f / static_cast<float>(n));
            b.velocity = { std::cos(angle) * def.speed, std::sin(angle) * def.speed };
            b.bounces = true;
            b.gravity = 0.0f;
            b.radius = 6.0f;
        } else if (def.kind == "ring") {
            // Ring of bullets around center of box
            const Vector2 center = { box.x + box.width * 0.5f, box.y + box.height * 0.5f };
            const float angle = static_cast<float>(i) * (6.2831853f / static_cast<float>(n));
            const float r = 24.0f;
            b.position = { center.x + std::cos(angle) * r, center.y + std::sin(angle) * r };
            b.velocity = { std::cos(angle) * def.speed, std::sin(angle) * def.speed };
            b.bounces = false;
            b.radius = 5.0f;
        } else if (def.kind == "wall") {
            // Vertical column moving horizontally inside box
            const float wallX = box.x + 8.0f;
            const float topY = box.y + 10.0f;
            const float h = box.height - 20.0f;
            const float y = (n == 1) ? (box.y + box.height * 0.5f)
                                     : (topY + h * (static_cast<float>(i) / static_cast<float>(n - 1)));
            b.position = { wallX, y };
            b.velocity = { def.speed, 0.0f };
            b.bounces = false;
            b.radius = 5.0f;
        } else if (def.kind == "aimed") {
            // Spawn at top inside box and aim towards target
            const float padding = 12.0f;
            const float span = std::max(0.0f, box.width - 2.0f * padding);
            const float x = (n == 1) ? (box.x + box.width * 0.5f)
                                     : (box.x + padding + span * (static_cast<float>(i) / static_cast<float>(n - 1)));
            const float y = box.y + 12.0f;
            b.position = { x, y };
            const float dx = target.x - x;
            const float dy = target.y - y;
            const float len = std::hypot(dx, dy);
            if (len > 0.001f) {
                b.velocity = { (dx / len) * def.speed, (dy / len) * def.speed };
            } else {
                b.velocity = { 0.0f, def.speed };
            }
            b.bounces = false;
            b.radius = 5.0f;
        } else { // "rain"
            const float padding = 14.0f;
            const float span = std::max(0.0f, box.width - 2.0f * padding);
            const float x = (n == 1) ? (box.x + box.width * 0.5f)
                                     : (box.x + padding + span * (static_cast<float>(i) / static_cast<float>(n - 1)));
            const float y = box.y + 10.0f + static_cast<float>(i % 3) * 12.0f;
            b.position = { x, y };
            b.velocity = { 0.0f, def.speed };
            b.bounces = false;
            b.radius = 5.0f;
        }

        (void)step;
        bullets.push_back(b);
    }

    return bullets;
}

} // namespace nundertale
