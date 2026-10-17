#pragma once

#include <cmath>
#include <algorithm>

namespace nundertale {

inline float stepVolume(float v, int dir) {
    if (dir > 0) {
        int step = static_cast<int>(std::floor((v + 1e-4f) * 10.0f)) + dir;
        return std::clamp(static_cast<float>(step) / 10.0f, 0.0f, 1.0f);
    } else if (dir < 0) {
        int step = static_cast<int>(std::ceil((v - 1e-4f) * 10.0f)) + dir;
        return std::clamp(static_cast<float>(step) / 10.0f, 0.0f, 1.0f);
    }
    return std::clamp(std::round(v * 10.0f) / 10.0f, 0.0f, 1.0f);
}

inline int volumePercent(float v) {
    return static_cast<int>(std::round(v * 100.0f));
}

inline int wrapIndex(int i, int dir, int count) {
    if (count <= 0) return 0;
    int res = (i + dir) % count;
    if (res < 0) res += count;
    return res;
}

} // namespace nundertale
