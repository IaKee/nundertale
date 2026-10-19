#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <raylib.h>

namespace nundertale {

struct RepeatedSpareOutcome {
    std::size_t lineIndex = 0;
    std::size_t linesShown = 0;
    bool showLine = false;
    bool nowSpareable = false;
    bool spared = false;
};

inline RepeatedSpareOutcome advanceRepeatedSpare(std::size_t linesShown,
                                                  std::size_t lineCount,
                                                  bool alreadySpareable) {
    RepeatedSpareOutcome outcome;
    outcome.lineIndex = linesShown;
    outcome.linesShown = linesShown;
    if (alreadySpareable || lineCount == 0U || linesShown >= lineCount) {
        outcome.spared = true;
        return outcome;
    }

    outcome.showLine = true;
    outcome.linesShown = linesShown + 1U;
    outcome.nowSpareable = outcome.linesShown >= lineCount;
    return outcome;
}

inline bool mercifulBulletCanDamage(bool mercifulBullets, int playerHp) {
    return !mercifulBullets || playerHp >= 5;
}

inline bool mercifulBulletWithinSteerRange(Vector2 bulletPosition,
                                           Vector2 soulPosition,
                                           float range = 24.0f) {
    const float dx = bulletPosition.x - soulPosition.x;
    const float dy = bulletPosition.y - soulPosition.y;
    return dx * dx + dy * dy <= range * range;
}

inline Vector2 mercifulBulletVelocityAway(Vector2 bulletPosition,
                                          Vector2 soulPosition,
                                          Vector2 currentVelocity,
                                          float minimumSpeed = 90.0f) {
    float dx = bulletPosition.x - soulPosition.x;
    float dy = bulletPosition.y - soulPosition.y;
    float length = std::sqrt(dx * dx + dy * dy);
    if (length <= 0.001f) {
        dx = -currentVelocity.x;
        dy = -currentVelocity.y;
        length = std::sqrt(dx * dx + dy * dy);
    }
    if (length <= 0.001f) {
        dx = 1.0f;
        dy = 0.0f;
        length = 1.0f;
    }

    const float currentSpeed = std::sqrt(
        currentVelocity.x * currentVelocity.x + currentVelocity.y * currentVelocity.y);
    const float speed = std::max(minimumSpeed, currentSpeed);
    return { dx / length * speed, dy / length * speed };
}

} // namespace nundertale
