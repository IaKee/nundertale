#pragma once

#include <raylib.h>
#include <string>

namespace nundertale {

enum class BulletType {
    Normal,
    UwU,
    SodaCan,
    Bone
};

struct Bullet {
    Vector2 position{ 0, 0 };
    Vector2 velocity{ 0, 0 };
    float radius = 5.0f;
    Color color = { 255, 255, 255, 255 };
    BulletType type = BulletType::Normal;
    std::string text = "";
    int damage = 3;
    float furry = 6.0f;
    bool active = true;

    void update(float dt) {
        position.x += velocity.x * dt;
        position.y += velocity.y * dt;
    }

    void render() const {
        if (!active) return;
        if (type == BulletType::UwU && !text.empty()) {
            DrawText(text.c_str(), (int)position.x - 12, (int)position.y - 8, 16, color);
        } else if (type == BulletType::SodaCan) {
            DrawRectangle((int)position.x - 5, (int)position.y - 8, 10, 16, color);
            DrawRectangleLines((int)position.x - 5, (int)position.y - 8, 10, 16, { 0, 0, 0, 255 });
            // Can tab detail
            DrawRectangle((int)position.x - 2, (int)position.y - 10, 4, 3, LIGHTGRAY);
        } else {
            DrawCircle((int)position.x, (int)position.y, radius, color);
        }
    }

    // Accurate hitbox per bullet type (BHL-003, BUG-10)
    Rectangle getBounds() const {
        if (type == BulletType::UwU) {
            return { position.x - 12, position.y - 8, 24, 16 };
        } else if (type == BulletType::SodaCan) {
            return { position.x - 5, position.y - 8, 10, 16 };
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
