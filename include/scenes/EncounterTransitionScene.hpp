#pragma once

#include "core/IScene.hpp"
#include <raylib.h>
#include <memory>

namespace nundertale {

class EncounterTransitionScene : public IScene {
public:
    EncounterTransitionScene(Vector2 playerPos, std::unique_ptr<IScene> targetScene);

    void onEnter(GameContext& context) override;
    void update(float dt, GameContext& context) override;
    void render(GameContext& context) override;
    void onExit(GameContext& context) override;

private:
    Vector2 m_startPos;
    Vector2 m_targetPos{ 48.0f, 446.0f }; // Soul position on FIGHT button (BUI-006, ENC-002)
    Vector2 m_currentPos;
    std::unique_ptr<IScene> m_targetScene;

    enum class Phase {
        Alert,     // ENC-001: "!" alert bubble above player's head
        Flash,     // 3 beeps with soul flashing (ENC-002)
        Slide,     // Soul slides down to FIGHT button
        Complete
    };

    Phase m_phase = Phase::Flash;
    float m_timer = 0.0f;
    int m_flashStep = 0;
    bool m_soulVisible = true;

    void drawSoul(float x, float y) const;
};

} // namespace nundertale
