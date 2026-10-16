#include "entities/Player.hpp"
#include <cmath>

namespace nundertale {

Player::Player(Vector2 startPos)
    : m_position(startPos) {}

void Player::update(float dt, Vector2 inputDir, const std::vector<Rectangle>& obstacles) {
    m_isMoving = (inputDir.x != 0.0f || inputDir.y != 0.0f);

    if (m_isMoving) {
        // OVW-006: Undertale keeps the prior pressed direction if still pressed on diagonal
        bool dirXMatches = (inputDir.x > 0 && m_direction == Direction::Right) || (inputDir.x < 0 && m_direction == Direction::Left);
        bool dirYMatches = (inputDir.y > 0 && m_direction == Direction::Down) || (inputDir.y < 0 && m_direction == Direction::Up);

        if (inputDir.x != 0.0f && inputDir.y != 0.0f) {
            // Diagonal movement: keep current direction if it matches one of the axes
            if (!dirXMatches && !dirYMatches) {
                m_direction = (inputDir.x > 0) ? Direction::Right : Direction::Left;
            }
        } else if (inputDir.x != 0.0f) {
            m_direction = (inputDir.x > 0) ? Direction::Right : Direction::Left;
        } else if (inputDir.y != 0.0f) {
            m_direction = (inputDir.y > 0) ? Direction::Down : Direction::Up;
        }

        // OVW-006: Do NOT normalize diagonal movement in Undertale
        // Move X with collision
        float newX = m_position.x + inputDir.x * PLAYER_SPEED * dt;
        Rectangle testBoxX = { newX + 5, m_position.y + 38, 28, 18 };
        bool collideX = false;
        for (const auto& obs : obstacles) {
            if (CheckCollisionRecs(testBoxX, obs)) {
                collideX = true;
                break;
            }
        }
        if (!collideX) {
            m_position.x = newX;
        }

        // Move Y with collision
        float newY = m_position.y + inputDir.y * PLAYER_SPEED * dt;
        Rectangle testBoxY = { m_position.x + 5, newY + 38, 28, 18 };
        bool collideY = false;
        for (const auto& obs : obstacles) {
            if (CheckCollisionRecs(testBoxY, obs)) {
                collideY = true;
                break;
            }
        }
        if (!collideY) {
            m_position.y = newY;
        }

        // Update animation frames
        m_animTimer += dt;
        if (m_animTimer >= 0.15f) {
            m_animTimer = 0.0f;
            m_alterna = !m_alterna;
            if (m_alterna) {
                m_alternaVert = !m_alternaVert;
            }
        }
    } else {
        m_animTimer = 0.0f;
        m_alterna = false;
        m_alternaVert = false;
    }

    // Calculate sprite clip region based on movement and direction
    m_clipH = 29;
    m_drawW = 38;
    m_drawH = 58;

    switch (m_direction) {
        case Direction::Down:
            m_clipW = 19;
            m_clipY = 1;
            m_clipX = m_isMoving ? (1 + 19 + (m_alternaVert ? 19 : 0)) : 1;
            break;
        case Direction::Up:
            m_clipW = 19;
            m_clipY = 1 + 29 * 2;
            m_clipX = m_isMoving ? (1 + 19 + (m_alternaVert ? 19 : 0)) : 1;
            break;
        case Direction::Left:
            m_clipW = 15;
            m_clipY = 1 + 29;
            m_clipX = m_isMoving ? (1 + 15 * (m_alterna ? 1 : 0)) : 1;
            m_drawW = 30;
            break;
        case Direction::Right:
            m_clipW = 15;
            m_clipY = 1 + 29;
            m_clipX = m_isMoving ? (33 + 15 * (m_alterna ? 1 : 0)) : 49;
            m_drawW = 30;
            break;
    }
}

void Player::render(Texture2D spriteSheet) {
    Rectangle sourceRec = {
        static_cast<float>(m_clipX),
        static_cast<float>(m_clipY),
        static_cast<float>(m_clipW),
        static_cast<float>(m_clipH)
    };

    Rectangle destRec = {
        m_position.x,
        m_position.y,
        static_cast<float>(m_drawW),
        static_cast<float>(m_drawH)
    };

    DrawTexturePro(spriteSheet, sourceRec, destRec, { 0, 0 }, 0.0f, WHITE);
}

Rectangle Player::getCollisionBox() const {
    return { m_position.x + 5, m_position.y + 38, 28, 18 };
}

Rectangle Player::getInteractionBox() const {
    switch (m_direction) {
        case Direction::Up:
            return { m_position.x + 5, m_position.y + 10, 28, 28 };
        case Direction::Down:
            return { m_position.x + 5, m_position.y + 50, 28, 28 };
        case Direction::Left:
            return { m_position.x - 20, m_position.y + 30, 28, 28 };
        case Direction::Right:
            return { m_position.x + 30, m_position.y + 30, 28, 28 };
    }
    return getCollisionBox();
}

} // namespace nundertale
