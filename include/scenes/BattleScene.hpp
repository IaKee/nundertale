#pragma once

#include "core/IScene.hpp"
#include "combat/BattleSystem.hpp"
#include <memory>

namespace nundertale {

class BattleScene : public IScene {
public:
    explicit BattleScene(std::string encounterId = "barky");
    ~BattleScene() override = default;

    void onEnter(GameContext& context) override;
    void update(float dt, GameContext& context) override;
    void render(GameContext& context) override;
    void onExit(GameContext& context) override;

    const std::string& getEncounterId() const { return m_encounterId; }

private:
    std::string m_encounterId = "barky";
    std::string m_musicId = "boss_barky";
    std::unique_ptr<BattleSystem> m_battleSystem;
    bool m_victoryMusicStopped = false; // AUD-004
};

} // namespace nundertale
