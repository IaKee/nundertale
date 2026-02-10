#pragma once

#include "core/IScene.hpp"
#include "combat/BattleSystem.hpp"
#include <memory>

namespace nundertale {

class BattleScene : public IScene {
public:
    BattleScene();
    ~BattleScene() override = default;

    void onEnter(GameContext& context) override;
    void update(float dt, GameContext& context) override;
    void render(GameContext& context) override;
    void onExit(GameContext& context) override;

private:
    std::unique_ptr<BattleSystem> m_battleSystem;
};

} // namespace nundertale
