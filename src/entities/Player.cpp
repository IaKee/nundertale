#include "entities/Player.hpp"
#include "core/RoomObjects.hpp"
#include <cmath>

namespace nundertale {

Player::Player(Vector2 startPos)
    : m_position(startPos) {}

void Player::setOverrideAnimation(const std::string& path, const std::string& key, float seconds) {
    m_overridePath = path;
    m_overrideKey = key;
    m_overrideRemaining = seconds;
    m_overrideAnimTimer = 0.0f;
}

void Player::updateOverrideAnimation(float dt) {
    if (m_overridePath.empty()) return;
    m_overrideAnimTimer += dt;
    if (m_overrideRemaining > 0.0f) {
        m_overrideRemaining -= dt;
        if (m_overrideRemaining <= 0.0f) {
            m_overridePath.clear();
            m_overrideKey.clear();
            m_overrideRemaining = 0.0f;
            m_overrideAnimTimer = 0.0f;
        }
    }
}

void Player::update(float dt, Vector2 inputDir, const std::vector<Rectangle>& obstacles) {
    m_blockingObstacles.clear();
    m_isMoving = (inputDir.x != 0.0f || inputDir.y != 0.0f);

    if (m_isMoving) {
        m_spriteAnimTimer += dt;
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
                if (inputDir.x != 0.0f) m_blockingObstacles.push_back(obs);
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
                if (inputDir.y != 0.0f) m_blockingObstacles.push_back(obs);
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
        m_spriteAnimTimer = 0.0f;
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

bool Player::wasBlockedBy(Rectangle obstacle) const {
    for (const Rectangle& blocking : m_blockingObstacles) {
        if (blocking.x == obstacle.x && blocking.y == obstacle.y &&
            blocking.width == obstacle.width && blocking.height == obstacle.height) {
            return true;
        }
    }
    return false;
}

bool Player::moveToward(Vector2 target, float speed, float dt) {
    const Vector2 position = m_position;
    const Vector2 next = stepToward(position, target, speed, dt);
    const Vector2 delta = { next.x - position.x, next.y - position.y };
    static const std::vector<Rectangle> noObstacles;

    if (delta.x == 0.0f && delta.y == 0.0f) {
        update(0.0f, { 0.0f, 0.0f }, noObstacles);
        return position.x == target.x && position.y == target.y;
    }

    if (std::abs(delta.x) > std::abs(delta.y)) {
        m_direction = delta.x < 0.0f ? Direction::Left : Direction::Right;
    } else {
        m_direction = delta.y < 0.0f ? Direction::Up : Direction::Down;
    }

    const float effectiveDt = std::max(0.0f, dt);
    if (effectiveDt > 0.0f) {
        const Vector2 input = {
            delta.x / (PLAYER_SPEED * effectiveDt),
            delta.y / (PLAYER_SPEED * effectiveDt)
        };
        update(effectiveDt, input, noObstacles);
        m_position = next;
    }
    return next.x == target.x && next.y == target.y;
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
