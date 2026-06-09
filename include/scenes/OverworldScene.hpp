#pragma once

#include "core/IScene.hpp"
#include "entities/Player.hpp"
#include "entities/Interactable.hpp"
#include "ui/DialogBox.hpp"
#include "core/FluffParticles.hpp"
#include "core/Cutscene.hpp"
#include "core/TiledMap.hpp"
#include "core/RoomObjects.hpp"
#include "core/RandomEncounters.hpp"
#include <vector>
#include <memory>
#include <string>
#include <unordered_set>

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
    void loadRoomMap(const std::string& roomId);
    void initInteractables(GameContext& context);
    void initRoomObjects(GameContext& context);
    void refreshDynamicObstacles(GameContext& context);
    bool roomObjectVisible(const TiledObject& object, GameContext& context);
    void interactWithRoomObject(const TiledObject& object, GameContext& context);
    void updateDoorsAndTriggers(GameContext& context);
    void beginDoorTransition(const TiledObject& door);
    void updateDoorTransition(GameContext& context);
    void swapDoorRoom(GameContext& context);
    void startCutscene(const std::string& id);
    void drawTileLayers(GameContext& context, bool above);
    void drawRoomObject(const TiledObject& object, GameContext& context);
    void drawPlayer(GameContext& context);
    std::vector<std::string> currentSaveLines(GameContext& context) const;
    std::string roomDisplayName(GameContext& context) const;
    std::string roomMusicKey(const std::string& roomId) const;
    Rectangle objectBounds(const std::string& name, Rectangle fallback) const;
    Vector2 objectPosition(const std::string& name, Vector2 fallback) const;

    void handleMenuInput(GameContext& context);
    void renderMenu(GameContext& context, Font font);
    void drawSoul(float x, float y) const;

    Player m_player;
    DialogBox m_dialogBox;

    std::vector<Rectangle> m_obstacles;
    std::vector<Rectangle> m_staticObstacles;
    std::vector<std::unique_ptr<IInteractable>> m_interactables;
    std::vector<TiledObject> m_roomObjects;
    std::unique_ptr<SavePoint> m_savePoint;
    TiledMap m_tiledMap;
    float m_roomWidth = 640.0f;
    float m_roomHeight = 480.0f;
    std::string m_backgroundImage;
    Vector2 m_backgroundOffset = { 0.0f, 0.0f };
    bool m_legacyBarky = false;
    std::string m_saveLinesOverride;
    float m_roomAnimationTime = 0.0f;
    std::unordered_set<std::string> m_loggedConditions;
    std::unordered_set<int> m_triggerOverlaps;
    int m_talkingObjectId = -1;     // OVW-007: the NPC turned toward Pepe while talking
    std::string m_talkingFacing;
    std::vector<EncounterEntry> m_roomEncounters;
    int m_encounterBudget = 0;
    int m_encounterStepsRemaining = 0;
    std::uint32_t m_encounterRng = 0xC0FFEEu;

    enum class DoorFadeState {
        None,
        Out,
        In
    };
    DoorFadeState m_doorFadeState = DoorFadeState::None;
    int m_doorFadeFrame = 0;
    std::string m_targetRoom;
    std::string m_targetSpawn = "spawn";
    std::string m_targetFacing;

    // Overworld C Menu (MNU-001)
    OverworldMenuState m_menuState = OverworldMenuState::Closed;
    int m_menuIndex = 0;       // 0: ITEM, 1: STAT, 2: CELL
    int m_itemSubIndex = 0;
    int m_itemActionIndex = 0; // 0: USE, 1: INFO, 2: DROP

    // CELL Menu (MNU-004 / NAR-004)
    enum class CellSubState {
        Contacts,
        FuriellMenu
    };
    CellSubState m_cellSubState = CellSubState::Contacts;
    int m_cellIndex = 0;
    int m_furiellIndex = 0;

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

    // Ambient fluff particles (FUR-010)
    std::vector<FluffParticle> m_fluffParticles;
    std::uint32_t m_fluffRng = 0x5F100Fu;

    CutsceneRunner m_cutscene;
};

} // namespace nundertale
