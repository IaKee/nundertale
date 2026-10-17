#pragma once

#include <algorithm>

namespace nundertale {

// AUD-006: linear music volume fade. The factor multiplies the configured music volume.
struct MusicFade {
    float from = 1.0f;
    float to = 1.0f;
    float duration = 0.0f;
    float elapsed = 0.0f;
    bool active = false;
    bool stopAtEnd = false;
};

inline void fadeStart(MusicFade& fade, float from, float to, float seconds, bool stopAtEnd) {
    fade.from = from;
    fade.to = to;
    fade.duration = seconds;
    fade.elapsed = 0.0f;
    fade.active = seconds > 0.0f;
    fade.stopAtEnd = stopAtEnd;
}

// Advances the fade and returns the current factor. Deactivates at the end (stopAtEnd is kept for the caller).
inline float fadeStep(MusicFade& fade, float dt) {
    if (!fade.active) {
        return fade.to;
    }
    fade.elapsed = std::min(fade.duration, fade.elapsed + std::max(0.0f, dt));
    const float t = fade.elapsed / fade.duration;
    if (fade.elapsed >= fade.duration) {
        fade.active = false;
    }
    return fade.from + (fade.to - fade.from) * t;
}

} // namespace nundertale
