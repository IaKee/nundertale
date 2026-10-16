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
};

} // namespace nundertale
