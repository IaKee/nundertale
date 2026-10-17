#pragma once

#include <raylib.h>
#include <string>
#include <algorithm>
#include <cmath>

namespace nundertale {

enum class BulletType {
    Normal,
    UwU,
    SodaCan,
    Awoo,
    Bone
};

enum class AttackColor {
    White,
    Blue,
    Orange,
    Green
};

inline constexpr Color ATTACK_COLOR_BLUE{ 0, 162, 232, 255 };    // cyan #00A2E8
inline constexpr Color ATTACK_COLOR_ORANGE{ 252, 166, 0, 255 };   // #FCA600
inline constexpr Color ATTACK_COLOR_GREEN{ 0, 192, 0, 255 };

struct Bullet {
    Vector2 position{ 0, 0 };
    Vector2 velocity{ 0, 0 };
    float radius = 5.0f;
    Color color = { 255, 255, 255, 255 };
    BulletType type = BulletType::Normal;
    AttackColor attackColor = AttackColor::White;
    int heal = 0; // used only when attackColor == Green
    std::string text = "";
    int damage = 3;
    int fur = 6;
    bool active = true;
    float gravity = 0.0f;
    bool bounces = false;
    float arcRadius = 14.0f;

    // Pure rule, unit-testable: does touching this bullet hurt the soul?
    bool hurtsSoul(bool soulMoving) const {
        switch (attackColor) {
            case AttackColor::White:
                return true;
            case AttackColor::Blue:
                return soulMoving;
            case AttackColor::Orange:
                return !soulMoving;
            case AttackColor::Green:
                return false;
        }
        return true;
    }

    bool healsSoul() const {
        return attackColor == AttackColor::Green;
    }

    void update(float dt, const Rectangle* box = nullptr) {
        if (gravity > 0.0f) {
            velocity.y += gravity * dt;
        }
        position.x += velocity.x * dt;
        position.y += velocity.y * dt;

        if (bounces && box) {
            // Bounce off left/right sides
            if (position.x - 5.0f <= box->x + 5.0f && velocity.x < 0.0f) {
                position.x = box->x + 5.0f;
                velocity.x = -velocity.x;
            } else if (position.x + 5.0f >= box->x + box->width - 5.0f && velocity.x > 0.0f) {
                position.x = box->x + box->width - 5.0f;
                velocity.x = -velocity.x;
            }
            // Bounce off bottom arena floor
            if (position.y + 8.0f >= box->y + box->height - 5.0f && velocity.y > 0.0f) {
                position.y = box->y + box->height - 5.0f - 8.0f;
                velocity.y = -std::abs(velocity.y) * 0.82f;
            }
        }
    }

    void render() const {
        if (!active) return;

        Color renderColor = color;
        if (attackColor == AttackColor::Blue) {
            renderColor = ATTACK_COLOR_BLUE;
        } else if (attackColor == AttackColor::Orange) {
            renderColor = ATTACK_COLOR_ORANGE;
        } else if (attackColor == AttackColor::Green) {
            renderColor = ATTACK_COLOR_GREEN;
        }

        if (type == BulletType::UwU && !text.empty()) {
            DrawText(text.c_str(), (int)position.x - 12, (int)position.y - 8, 16, renderColor);
        } else if (type == BulletType::SodaCan) {
            DrawRectangle((int)position.x - 5, (int)position.y - 8, 10, 16, renderColor);
            DrawRectangleLines((int)position.x - 5, (int)position.y - 8, 10, 16, { 0, 0, 0, 255 });
            DrawRectangle((int)position.x - 2, (int)position.y - 10, 4, 3, LIGHTGRAY);
        } else if (type == BulletType::Awoo) {
            DrawCircleLines((int)position.x, (int)position.y, arcRadius, renderColor);
            DrawCircleLines((int)position.x, (int)position.y, std::max(1.0f, arcRadius - 2.0f), renderColor);
        } else {
            DrawCircle((int)position.x, (int)position.y, radius, renderColor);
        }
    }

    // Accurate hitbox per bullet type (BHL-003, BUG-10)
    Rectangle getBounds() const {
        if (type == BulletType::UwU) {
            return { position.x - 10, position.y - 7, 20, 14 };
        } else if (type == BulletType::SodaCan) {
            return { position.x - 5, position.y - 8, 10, 16 };
        } else if (type == BulletType::Awoo) {
            return { position.x - arcRadius * 0.7f, position.y - arcRadius * 0.7f, arcRadius * 1.4f, arcRadius * 1.4f };
        }
        return { position.x - radius, position.y - radius, radius * 2, radius * 2 };
    }

    // Check if bullet exited arena bounds with margin
    bool isOutOfBounds(const Rectangle& box, float margin = 40.0f) const {
        return (position.x < box.x - margin ||
                position.x > box.x + box.width + margin ||
                position.y < box.y - margin ||
                position.y > box.y + box.height + margin);
    }
};

} // namespace nundertale
