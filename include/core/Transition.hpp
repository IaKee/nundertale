#pragma once

#include <raylib.h>
#include <memory>
#include <algorithm>
#include "core/Constants.hpp"

namespace nundertale {

class ITransition {
public:
    virtual ~ITransition() = default;
    virtual float duration() const = 0;            // seconds for ONE half (out or in); Cut => 0
    virtual float durationIn() const { return duration(); }
    // progress 0..1 of the current half; coveringOut=true while covering the old scene, false while revealing the new one
    virtual void render(float progress, bool coveringOut) const = 0;
    // pure helper for tests: how covered the screen is (0 = clear, 1 = fully covered)
    virtual float coverage(float progress, bool coveringOut) const = 0;
};

class FadeTransition : public ITransition {
public:
    FadeTransition(Color color, float seconds)
        : m_color(color), m_durationOut(std::max(0.0f, seconds)), m_durationIn(std::max(0.0f, seconds)) {}

    FadeTransition(Color color, float outSeconds, float inSeconds)
        : m_color(color), m_durationOut(std::max(0.0f, outSeconds)), m_durationIn(std::max(0.0f, inSeconds)) {}

    float duration() const override {
        return m_durationOut;
    }

    float durationIn() const override {
        return m_durationIn;
    }

    float coverage(float progress, bool coveringOut) const override {
        float raw = coveringOut ? progress : (1.0f - progress);
        return std::clamp(raw, 0.0f, 1.0f);
    }

    void render(float progress, bool coveringOut) const override {
        float cov = coverage(progress, coveringOut);
        if (cov <= 0.0f) return;
        unsigned char a = static_cast<unsigned char>(std::clamp(cov * 255.0f, 0.0f, 255.0f));
        DrawRectangle(0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT, Color{ m_color.r, m_color.g, m_color.b, a });
    }

private:
    Color m_color;
    float m_durationOut = 0.0f;
    float m_durationIn = 0.0f;
};

class CutTransition : public ITransition {
public:
    float duration() const override {
        return 0.0f;
    }

    void render(float /*progress*/, bool /*coveringOut*/) const override {
        // Cut transition never draws
    }

    float coverage(float /*progress*/, bool /*coveringOut*/) const override {
        return 0.0f;
    }
};

inline std::unique_ptr<ITransition> fadeBlack(float seconds) {
    return std::make_unique<FadeTransition>(Color{ 0, 0, 0, 255 }, seconds);
}

inline std::unique_ptr<ITransition> fadeBlack(float outSeconds, float inSeconds) {
    return std::make_unique<FadeTransition>(Color{ 0, 0, 0, 255 }, outSeconds, inSeconds);
}

inline std::unique_ptr<ITransition> fadeWhite(float seconds) {
    return std::make_unique<FadeTransition>(Color{ 255, 255, 255, 255 }, seconds);
}

inline std::unique_ptr<ITransition> fadeWhite(float outSeconds, float inSeconds) {
    return std::make_unique<FadeTransition>(Color{ 255, 255, 255, 255 }, outSeconds, inSeconds);
}

inline std::unique_ptr<ITransition> cut() {
    return std::make_unique<CutTransition>();
}

} // namespace nundertale
