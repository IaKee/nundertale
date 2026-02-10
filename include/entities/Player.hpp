#pragma once

#include <raylib.h>
#include <vector>
#include "core/Constants.hpp"

namespace nundertale {

enum class Direction {
    Up = 1,
    Down = 2,
    Left = 3,
    Right = 4
};

class Player {
public:
    Player(Vector2 startPos = { 320.0f, 240.0f });

    void update(float dt, Vector2 inputDir, const std::vector<Rectangle>& obstacles);
    void render(Texture2D spriteSheet);

    Vector2 getPosition() const { return m_position; }
    void setPosition(Vector2 pos) { m_position = pos; }

    Rectangle getCollisionBox() const;
    Rectangle getInteractionBox() const;

    Direction getDirection() const { return m_direction; }
    void setDirection(Direction dir) { m_direction = dir; }

    bool isMoving() const { return m_isMoving; }

private:
    Vector2 m_position;
    Direction m_direction = Direction::Down;
    bool m_isMoving = false;

    float m_animTimer = 0.0f;
    int m_animFrame = 0;
    bool m_alterna = false;
    bool m_alternaVert = false;

    // Sprite dimensions on sheet
    int m_clipX = 1;
    int m_clipY = 1;
    int m_clipW = 19;
    int m_clipH = 29;
    int m_drawW = 38;
    int m_drawH = 58;
};

} // namespace nundertale
