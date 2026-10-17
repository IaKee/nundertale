#pragma once

namespace nundertale {

inline constexpr float TITLE_IDLE_SECONDS = 20.0f;

enum class TitleNext {
    Instructions,
    ContinueCard,
    Intro,
    Stay
};

inline TitleNext titleNext(bool confirmPressed, bool hasSave, float idleSeconds) {
    if (confirmPressed) {
        return hasSave ? TitleNext::ContinueCard : TitleNext::Instructions;
    }
    if (idleSeconds >= TITLE_IDLE_SECONDS) {
        return TitleNext::Intro;
    }
    return TitleNext::Stay;
}

inline bool pressPromptVisible(float elapsedSeconds) {
    return elapsedSeconds >= 1.0f;
}

} // namespace nundertale
