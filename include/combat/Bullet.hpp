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
    Vector2 position;
    Vector2 velocity;
    float radius = 5.0f;
    Color color = { 255, 255, 255, 255 };
    BulletType type = BulletType::Normal;
    std::string text = "";
    bool active = true;

    void update(float dt) {
        position.x += velocity.x * dt;
        position.y += velocity.y * dt;
    }

    void render() const {
        if (!active) return;
        if (type == BulletType::UwU && !text.empty()) {
            DrawText(text.c_str(), (int)position.x - 12, (int)position.y - 8, 14, color);
        } else if (type == BulletType::SodaCan) {
            DrawRectangle((int)position.x - 4, (int)position.y - 8, 8, 16, color);
            DrawRectangleLines((int)position.x - 4, (int)position.y - 8, 8, 16, { 0, 0, 0, 255 });
        } else {
            DrawCircle((int)position.x, (int)position.y, radius, color);
        }
    }

    Rectangle getBounds() const {
        return { position.x - radius, position.y - radius, radius * 2, radius * 2 };
    }
};

} // namespace nundertale
