#pragma once

#include "core/IScene.hpp"
#include <string>

namespace nundertale {

class TitleScene : public IScene {
public:
    TitleScene() = default;
    ~TitleScene() override = default;

    void onEnter(GameContext& context) override;
    void update(float dt, GameContext& context) override;
    void render(GameContext& context) override;
    void onExit(GameContext& context) override;

private:
    enum class Screen {
        Splash,
        ContinueCard
    };

    void resetSplash();
    void updateSplash(float dt, GameContext& context);
    void updateContinueCard(float dt, GameContext& context);
    void renderSplash(GameContext& context);
    void renderContinueCard(GameContext& context);

    Screen m_screen = Screen::Splash;
    int m_selectedIndex = 0;
    float m_animTimer = 0.0f;
    float m_idleTimer = 0.0f;
    bool m_promptSoundPlayed = false;
    bool m_hasSave = false;
    std::string m_savedName = "PEPE";
    int m_savedLevel = 1;
    float m_savedPlayTime = 0.0f;
    std::string m_savedRoom = "Ruins - Entrance";
};

} // namespace nundertale
