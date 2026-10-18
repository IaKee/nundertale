#pragma once

#include "core/IScene.hpp"
#include "entities/Player.hpp"
#include "entities/Interactable.hpp"
#include "ui/DialogBox.hpp"
#include "core/FluffParticles.hpp"
#include "core/Cutscene.hpp"
#include "core/TiledMap.hpp"
#include "core/RoomTransition.hpp"
#include "core/EncounterTable.hpp"
#include <cstddef>
#include <vector>
#include <memory>
#include <optional>
#include <string>

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
    struct DoorDef {
        Rectangle bounds{};
        std::string targetRoom;
        std::string targetSpawn = "default";
        std::string facing;
    };

    struct PropDef {
        Rectangle bounds{};
        std::string sprite;
        int frames = 1;
        float fps = 4.0f;
        float animationTime = 0.0f;
    };

    void initObstacles();
    void loadRoomMap(const std::string& roomId);
    void initInteractables(GameContext& context);
    void initDataObjects(GameContext& context);
    void swapRoom(GameContext& context);
    const TiledObject* findSpawn(const std::string& spawnId) const;
    bool hasMapObject(const std::string& name) const;
    Rectangle objectBounds(const std::string& name, Rectangle fallback) const;
    void drawMapLayers(GameContext& context, bool above) const;
    void drawProp(GameContext& context, const PropDef& prop) const;
    void drawPlayer(GameContext& context) const;
    void resetRandomEncounters();
    bool updateRandomEncounters(float movedDistance, GameContext& context);

    void handleMenuInput(GameContext& context);
    void renderMenu(GameContext& context, Font font);
    void drawSoul(float x, float y) const;

    Player m_player;
    DialogBox m_dialogBox;

    std::vector<Rectangle> m_obstacles;
    std::size_t m_baseObstacleCount = 0;
    std::vector<std::unique_ptr<IInteractable>> m_interactables;
    std::vector<DoorDef> m_doors;
    std::vector<PropDef> m_props;
    std::unique_ptr<SavePoint> m_savePoint;
    TiledMap m_tiledMap;
    RoomTransition m_roomTransition;
    bool m_mapLoaded = false;
    float m_roomWidth = 640.0f;
    float m_roomHeight = 480.0f;
    std::string m_backgroundImage = "assets/img/test.bmp";
    Vector2 m_backgroundOffset = { 0.0f, 0.0f };
    std::optional<EncounterTable> m_encounterTable;
    float m_encounterGraceRemaining = 0.0f;

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
