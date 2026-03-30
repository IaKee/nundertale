#pragma once

#include "core/IScene.hpp"
#include "services/ISaveService.hpp"

namespace nundertale {

class SettingsScene : public IScene {
public:
    SettingsScene() = default;
    ~SettingsScene() override = default;

    void onEnter(GameContext& context) override;
    void update(float dt, GameContext& context) override;
    void render(GameContext& context) override;
    void onExit(GameContext& context) override;

private:
    int m_selectedIndex = 0;
    UserConfig m_config{};
    UserConfig m_initialConfig{};

    bool isConfigChanged() const;
};

} // namespace nundertale
