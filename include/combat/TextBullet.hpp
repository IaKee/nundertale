#pragma once

#include <algorithm>
#include <cstddef>
#include <string>
#include <vector>

#include <raylib.h>

namespace nundertale {

inline constexpr float TEXT_BULLET_FONT_SIZE = 16.0f;
inline constexpr float TEXT_WALL_GAP_HEIGHT = 48.0f;
inline constexpr float TEXT_BULLET_STRIKE_LINGER = 0.25f;

inline Rectangle textBulletRect(Vector2 position, float textWidth, float fontSize) {
    return {
        position.x - textWidth * 0.5f,
        position.y - fontSize * 0.5f,
        textWidth,
        fontSize
    };
}

inline bool rectHitsCircle(Rectangle rect, Vector2 center, float radius) {
    const float closestX = std::clamp(center.x, rect.x, rect.x + rect.width);
    const float closestY = std::clamp(center.y, rect.y, rect.y + rect.height);
    const float dx = center.x - closestX;
    const float dy = center.y - closestY;
    return dx * dx + dy * dy <= radius * radius;
}

inline const std::string& textBulletWord(const std::vector<std::string>& words,
                                         std::size_t index,
                                         const std::string& fallback) {
    return words.empty() ? fallback : words[index % words.size()];
}

inline float textWallGapTop(float requestedTop,
                            float arenaTop,
                            float arenaBottom,
                            float gapHeight = TEXT_WALL_GAP_HEIGHT) {
    return std::clamp(requestedTop, arenaTop, std::max(arenaTop, arenaBottom - gapHeight));
}

inline bool textWallWordOverlapsGap(float wordCenterY,
                                    float fontSize,
                                    float gapTop,
                                    float gapHeight = TEXT_WALL_GAP_HEIGHT) {
    const float wordTop = wordCenterY - fontSize * 0.5f;
    const float wordBottom = wordCenterY + fontSize * 0.5f;
    return wordBottom >= gapTop && wordTop <= gapTop + gapHeight;
}

inline bool textBulletStruck(bool strike, float age, float strikeAfter) {
    return strike && age >= std::max(0.0f, strikeAfter);
}

inline bool textBulletExpired(bool strike, float age, float strikeAfter) {
    return strike && age >= std::max(0.0f, strikeAfter) + TEXT_BULLET_STRIKE_LINGER;
}

} // namespace nundertale
