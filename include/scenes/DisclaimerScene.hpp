#pragma once

#include "core/IScene.hpp"
#include <string>

namespace nundertale {

class DisclaimerScene : public IScene {
public:
    DisclaimerScene() = default;
    ~DisclaimerScene() override = default;

    void onEnter(GameContext& context) override;
    void update(float dt, GameContext& context) override;
    void render(GameContext& context) override;
    void onExit(GameContext& context) override;

private:
    float m_timer = 0.0f;
    float m_alpha = 0.0f;
    bool m_fadingIn = true;
};

} // namespace nundertale
