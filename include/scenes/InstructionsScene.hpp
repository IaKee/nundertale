#pragma once

#include "core/IScene.hpp"

namespace nundertale {

class InstructionsScene : public IScene {
public:
    InstructionsScene() = default;
    ~InstructionsScene() override = default;

    void onEnter(GameContext& context) override;
    void update(float dt, GameContext& context) override;
    void render(GameContext& context) override;
    void onExit(GameContext& context) override;

private:
    int m_selectedIndex = 0;
    float m_settingsMessageTimer = 0.0f;
};

} // namespace nundertale
