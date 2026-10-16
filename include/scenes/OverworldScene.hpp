#pragma once

#include "core/IScene.hpp"
#include "entities/Player.hpp"
#include "entities/Interactable.hpp"
#include "ui/DialogBox.hpp"
#include <vector>
#include <memory>

namespace nundertale {

class OverworldScene : public IScene {
public:
    OverworldScene();
    ~OverworldScene() override = default;

    void onEnter(GameContext& context) override;
    void update(float dt, GameContext& context) override;
    void render(GameContext& context) override;
    void onExit(GameContext& context) override;

private:
    void initObstacles();
    void initInteractables();

    Player m_player;
    DialogBox m_dialogBox;

    std::vector<Rectangle> m_obstacles;
    std::vector<std::unique_ptr<IInteractable>> m_interactables;
    std::unique_ptr<SavePoint> m_savePoint;

    bool m_paused = false;
};

} // namespace nundertale
