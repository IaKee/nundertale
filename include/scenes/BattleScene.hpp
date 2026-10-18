#pragma once

#include "core/IScene.hpp"
#include "combat/BattleSystem.hpp"
#include <memory>
#include <string>

namespace nundertale {

class BattleScene : public IScene {
public:
    explicit BattleScene(std::string encounterId = "barky");
    ~BattleScene() override = default;

    void onEnter(GameContext& context) override;
    void update(float dt, GameContext& context) override;
    void render(GameContext& context) override;
    void onExit(GameContext& context) override;
    bool isValid() const { return m_valid; }

private:
    std::unique_ptr<BattleSystem> m_battleSystem;
    std::string m_encounterId;
    std::string m_musicId;
    std::string m_backgroundId;
    bool m_valid = false;
    bool m_victoryMusicStopped = false; // AUD-004
};

} // namespace nundertale
