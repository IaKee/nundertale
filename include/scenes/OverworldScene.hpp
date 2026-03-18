#pragma once

#include "core/IScene.hpp"
#include "entities/Player.hpp"
#include "entities/Interactable.hpp"
#include "ui/DialogBox.hpp"
#include <vector>
#include <memory>

namespace nundertale {

enum class OverworldMenuState {
    Closed,
    Main,
    Item,
    ItemAction,
    Stat,
    Cell
};

class OverworldScene : public IScene {
public:
    OverworldScene();
    ~OverworldScene() override = default;

    void onEnter(GameContext& context) override;
    void onResume(GameContext& context) override;
    void update(float dt, GameContext& context) override;
    void render(GameContext& context) override;
    void onExit(GameContext& context) override;

private:
    void initObstacles();
    void initInteractables(GameContext& context);

    void handleMenuInput(GameContext& context);
    void renderMenu(GameContext& context, Font font);
    void drawSoul(float x, float y) const;

    Player m_player;
    DialogBox m_dialogBox;

    std::vector<Rectangle> m_obstacles;
    std::vector<std::unique_ptr<IInteractable>> m_interactables;
    std::unique_ptr<SavePoint> m_savePoint;

    // Overworld C Menu (MNU-001)
    OverworldMenuState m_menuState = OverworldMenuState::Closed;
    int m_menuIndex = 0;       // 0: ITEM, 1: STAT, 2: CELL
    int m_itemSubIndex = 0;
    int m_itemActionIndex = 0; // 0: USE, 1: INFO, 2: DROP

    // Save Point Dialog & Box (SAV-002)
    enum class SaveBoxState {
        Closed,
        Active,
        Saved
    };
    SaveBoxState m_saveBoxState = SaveBoxState::Closed;
    int m_saveBoxChoice = 0; // 0: Save, 1: Return

    void handleSaveBoxInput(GameContext& context);
    void renderSaveBox(GameContext& context, Font font);
};

} // namespace nundertale
