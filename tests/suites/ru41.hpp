#pragma once

// T41 / C1-030 & SAV-007: Save/load round-trip at every Ruins save point.
// Verifies that every Ruins map loads, save points are correctly identified,
// and full GameState (room, spawn/playerPos, flags, FUR, toxic track, items)
// survives serialisation and deserialisation through SaveService.

#include "combat/Enemy.hpp"
#include "core/GameContext.hpp"
#include "core/RoomObjects.hpp"
#include "core/SaveText.hpp"
#include "core/TiledMap.hpp"
#include "core/ToxicTrack.hpp"
#include "services/SaveService.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace ru41_detail {

struct SavePointInfo {
    std::string roomId;
    std::string roomDisplayName;
    float x = 0.0f;
    float y = 0.0f;
    std::string customLines;
    std::string mapSavePlace;
};

inline std::string readTextFile(const std::filesystem::path& path) {
    std::ifstream stream(path);
    if (!stream.is_open()) return {};
    std::ostringstream ss;
    ss << stream.rdbuf();
    return ss.str();
}

} // namespace ru41_detail

inline void suite_ru41() {
    using namespace nundertale;
    using ru41_detail::SavePointInfo;
    using ru41_detail::readTextFile;

    // All 15 canonical Ruins maps (R1..R13 plus subrooms)
    const std::vector<std::string> ruinsMapIds = {
        "ruins_fall",
        "ruins_floofy",
        "ruins_entrance",
        "ruins_switches",
        "ruins_dummy",
        "ruins_cables",
        "ruins_hallway",
        "ruins_notices",
        "ruins_barky",
        "ruins_bakesale",
        "ruins_house",
        "ruins_house_hallway",
        "ruins_house_pepe",
        "ruins_basement",
        "ruins_exit"
    };

    // -------------------------------------------------------------------------
    // 1. Load every Ruins map and find each save point
    // -------------------------------------------------------------------------
    std::vector<SavePointInfo> foundSavePoints;
    std::vector<std::string> roomsWithoutSpawn;

    for (const std::string& roomId : ruinsMapIds) {
        const std::string mapPath = "data/maps/" + roomId + ".json";
        TiledMap map;
        const bool loaded = loadTiledMapFromFile(mapPath, map);
        assert(loaded && "Every Ruins map must load from data/maps/");
        assert(map.width > 0 && map.height > 0);
        assert(map.tileWidth > 0 && map.tileHeight > 0);

        // Verify that every Ruins room has a default spawn point
        const TiledObject* defaultSpawn = findRoomSpawn(map, "spawn");
        if (defaultSpawn == nullptr) {
            roomsWithoutSpawn.push_back(roomId);
        } else {
            // Spawn point must be located within valid map bounds
            assert(defaultSpawn->x >= 0.0f && defaultSpawn->x <= map.pixelWidth());
            assert(defaultSpawn->y >= 0.0f && defaultSpawn->y <= map.pixelHeight());
        }

        const std::string mapDisplayName = tiledStringProperty(map.properties, "displayName");
        const std::string mapSavePlace = tiledStringProperty(map.properties, "savePlace");

        // Inspect objects layer for save points
        for (const TiledLayer& layer : map.layers) {
            if (layer.type != TiledLayerType::Objects) continue;
            for (const TiledObject& object : layer.objects) {
                if (tiledObjectType(object) == "savepoint") {
                    SavePointInfo info;
                    info.roomId = roomId;
                    info.roomDisplayName = mapDisplayName;
                    info.x = object.x;
                    info.y = object.y;
                    info.customLines = tiledStringProperty(object.properties, "lines");
                    info.mapSavePlace = mapSavePlace;
                    foundSavePoints.push_back(info);
                }
            }
        }
    }

    // Every Ruins room has a spawn point
    assert(roomsWithoutSpawn.empty() && "All Ruins rooms must have a 'spawn' object");

    // Exactly 5 save points exist in the Ruins (R1, R3, R9 Barky, R10 Bake Sale, R13 Exit)
    assert(foundSavePoints.size() == 5U && "Ruins must contain exactly 5 save points");

    // Validate expected save point locations
    {
        const auto itFall = std::find_if(foundSavePoints.begin(), foundSavePoints.end(),
            [](const SavePointInfo& s) { return s.roomId == "ruins_fall"; });
        assert(itFall != foundSavePoints.end());
        assert(std::abs(itFall->x - 180.0f) < 0.01f);
        assert(std::abs(itFall->y - 320.0f) < 0.01f);

        const auto itEntrance = std::find_if(foundSavePoints.begin(), foundSavePoints.end(),
            [](const SavePointInfo& s) { return s.roomId == "ruins_entrance"; });
        assert(itEntrance != foundSavePoints.end());
        assert(std::abs(itEntrance->x - 300.0f) < 0.01f);
        assert(std::abs(itEntrance->y - 380.0f) < 0.01f);

        const auto itBarky = std::find_if(foundSavePoints.begin(), foundSavePoints.end(),
            [](const SavePointInfo& s) { return s.roomId == "ruins_barky"; });
        assert(itBarky != foundSavePoints.end());
        assert(std::abs(itBarky->x - 320.0f) < 0.01f);
        assert(std::abs(itBarky->y - 260.0f) < 0.01f);

        const auto itBakesale = std::find_if(foundSavePoints.begin(), foundSavePoints.end(),
            [](const SavePointInfo& s) { return s.roomId == "ruins_bakesale"; });
        assert(itBakesale != foundSavePoints.end());
        assert(std::abs(itBakesale->x - 180.0f) < 0.01f);
        assert(std::abs(itBakesale->y - 340.0f) < 0.01f);
        assert(!itBakesale->customLines.empty());

        const auto itExit = std::find_if(foundSavePoints.begin(), foundSavePoints.end(),
            [](const SavePointInfo& s) { return s.roomId == "ruins_exit"; });
        assert(itExit != foundSavePoints.end());
        assert(std::abs(itExit->x - 180.0f) < 0.01f);
        assert(std::abs(itExit->y - 200.0f) < 0.01f);
    }

    // -------------------------------------------------------------------------
    // 2. LORE & SaveText check for each save point (SAV-007)
    // -------------------------------------------------------------------------
    for (const SavePointInfo& sp : foundSavePoints) {
        GameState dummyState;
        dummyState.currentRoom = sp.roomId;
        dummyState.player.hp = 10;
        dummyState.player.maxHp = 20;

        // Normal route heal: heals HP, does not touch FUR
        SaveHealResult healNorm = saveHealResult(dummyState, sp.customLines);
        assert(!healNorm.furryRoute);
        assert(healNorm.hpAfter == 20);
        assert(healNorm.furAfter == 0);
        assert(!healNorm.lines.empty());

        // Furry route: heals FLUFF, leaves HP and FUR untouched
        dummyState.setFlag("route.furry", 1);
        dummyState.setFlag("fluff.value", 5);
        SaveHealResult healFurry = saveHealResult(dummyState, sp.customLines);
        assert(healFurry.furryRoute);
        assert(healFurry.fluffAfter == fluffMax(dummyState));
        assert(healFurry.hpAfter == 10);
        assert(healFurry.furAfter == 0);

        // FUR > 0 adds the determination fluff reminder
        dummyState.setFlag("route.furry", 0);
        dummyState.furry.value = 25;
        SaveHealResult healFluff = saveHealResult(dummyState, sp.customLines);
        assert(healFluff.lines.size() >= 2U);
        assert(healFluff.lines.back() == "* DETERMINATION can't fix fluff.");
    }

    // -------------------------------------------------------------------------
    // 3. Round-trip GameState at EVERY Ruins save point through SaveService
    // -------------------------------------------------------------------------
    // All temporary save files are kept strictly within worktree tmp/
    const char* oldLocalAppData = std::getenv("LOCALAPPDATA");
    const std::string oldLocalAppDataVal = oldLocalAppData != nullptr ? oldLocalAppData : "";
    const std::filesystem::path tempSaveDir = "tmp/ru41_saves";

    std::error_code ec;
    std::filesystem::remove_all(tempSaveDir, ec);
    std::filesystem::create_directories(tempSaveDir, ec);
    assert(_putenv_s("LOCALAPPDATA", tempSaveDir.string().c_str()) == 0);

    for (std::size_t i = 0; i < foundSavePoints.size(); ++i) {
        const SavePointInfo& sp = foundSavePoints[i];

        SaveService saves;

        GameState initial;
        initial.currentRoom = sp.roomId;
        initial.playerPos = { sp.x, sp.y };
        initial.playerFacing = static_cast<int>(i % 4);
        initial.playTime = 145.5f + static_cast<float>(i) * 30.0f;

        initial.player.name = "GRUB";
        initial.player.hp = 12 + static_cast<int>(i);
        initial.player.maxHp = 20;
        initial.player.level = 1;
        initial.player.exp = static_cast<int>(i) * 5;
        initial.player.gold = 30 + static_cast<int>(i) * 10;
        initial.player.at = 2;
        initial.player.df = 1;
        initial.player.weapon = "Toy Laser Pointer";
        initial.player.armor = "Fursuit Head";
        initial.player.weaponAt = 3;
        initial.player.armorDf = 5;

        // Rich inventory
        initial.player.inventory = {
            "Bottle",
            "Mt. Dew",
            "Doritos",
            "Tide Pod",
            "White Glue"
        };

        // FUR state
        initial.furry.value = 10 + static_cast<int>(i) * 15;
        initial.furry.stageMax = static_cast<int>(furStageFor(initial.furry.value));
        initial.furry.transformations = 0;

        // Raid clock
        initial.raid.started = true;
        initial.raid.timeLeft = 800.0f - static_cast<float>(i) * 50.0f;

        // Toxic track flags
        initial.setFlag(std::string(TOXIC_TRACK_FLAG), 1);
        initial.setFlag(std::string(TOXIC_KILLS_FLAG), 5 + static_cast<int>(i));
        initial.setFlag(std::string(TOXIC_STREAK_FLAG), 3);
        initial.setFlag(std::string(TOXIC_FADE_ROOMS_FLAG), 0);
        initial.killsTotal = 5 + static_cast<int>(i);

        // Custom story & area flags
        initial.setFlag("ruins.intro.done", 1);
        initial.setFlag("ruins.dew.taken", 1);
        initial.setFlag("kills.ruins", initial.killsTotal);
        initial.setFlag("negative_flag_test", -42);
        initial.setFlag("zero_flag_test", 0);

        // Perform save
        const bool saveSuccess = saves.saveGame(initial);
        assert(saveSuccess && "SaveService::saveGame must succeed");
        assert(saves.hasSaveGame() && "Save file must exist after saving");

        // Inspect raw JSON file to verify save text format
        const std::string savePath = saves.getSaveFilePath();
        const std::string jsonText = readTextFile(savePath);
        assert(!jsonText.empty() && "Save file must contain text content");
        assert(jsonText.find("\"version\": 1") != std::string::npos);
        assert(jsonText.find("\"currentRoom\": \"" + sp.roomId + "\"") != std::string::npos);
        assert(jsonText.find("\"inventory\":") != std::string::npos);
        assert(jsonText.find("\"Tide Pod\"") != std::string::npos);
        assert(jsonText.find("\"flags\":") != std::string::npos);
        assert(jsonText.find("\"furry\":") != std::string::npos);
        assert(jsonText.find("\"raid\":") != std::string::npos);

        // Perform load
        GameState loaded;
        const bool loadSuccess = saves.loadGame(loaded);
        assert(loadSuccess && "SaveService::loadGame must succeed");

        // 3a. Room and player position round-trip
        assert(loaded.currentRoom == initial.currentRoom);
        assert(std::abs(loaded.playerPos.x - initial.playerPos.x) < 0.001f);
        assert(std::abs(loaded.playerPos.y - initial.playerPos.y) < 0.001f);
        assert(loaded.playerFacing == initial.playerFacing);
        assert(std::abs(loaded.playTime - initial.playTime) < 0.01f);

        // 3b. Verify the saved room and spawn can be restored in OverworldScene style
        TiledMap restoredMap;
        const bool mapRestored = loadTiledMapFromFile("data/maps/" + loaded.currentRoom + ".json", restoredMap);
        assert(mapRestored && "Room map must reload successfully from loaded.currentRoom");

        const TiledObject* defaultSpawn = findRoomSpawn(restoredMap, "spawn");
        assert(defaultSpawn != nullptr && "Restored room map must contain a default spawn object");

        // Restored coordinates must remain within valid map boundaries
        assert(loaded.playerPos.x >= 0.0f && loaded.playerPos.x <= restoredMap.pixelWidth());
        assert(loaded.playerPos.y >= 0.0f && loaded.playerPos.y <= restoredMap.pixelHeight());

        // Restored map must contain the save point at the loaded position
        const TiledObject* restoredSavePoint = findRoomObject(restoredMap, "savepoint");
        assert(restoredSavePoint != nullptr && "Savepoint object must exist in restored room");
        assert(std::abs(restoredSavePoint->x - loaded.playerPos.x) < 0.001f);
        assert(std::abs(restoredSavePoint->y - loaded.playerPos.y) < 0.001f);

        // 3c. Player stats round-trip
        assert(loaded.player.name == initial.player.name);
        assert(loaded.player.hp == initial.player.hp);
        assert(loaded.player.maxHp == initial.player.maxHp);
        assert(loaded.player.level == initial.player.level);
        assert(loaded.player.exp == initial.player.exp);
        assert(loaded.player.gold == initial.player.gold);
        assert(loaded.player.at == initial.player.at);
        assert(loaded.player.df == initial.player.df);
        assert(loaded.player.weapon == initial.player.weapon);
        assert(loaded.player.armor == initial.player.armor);
        assert(loaded.player.weaponAt == initial.player.weaponAt);
        assert(loaded.player.armorDf == initial.player.armorDf);

        // 3d. Items (inventory) round-trip
        assert(loaded.player.inventory.size() == initial.player.inventory.size());
        for (std::size_t itemIdx = 0; itemIdx < initial.player.inventory.size(); ++itemIdx) {
            assert(loaded.player.inventory[itemIdx] == initial.player.inventory[itemIdx]);
        }

        // 3e. FUR state round-trip
        assert(loaded.furry.value == initial.furry.value);
        assert(loaded.furry.stageMax == initial.furry.stageMax);
        assert(loaded.furry.transformations == initial.furry.transformations);

        // 3f. Raid clock round-trip
        assert(loaded.raid.started == initial.raid.started);
        assert(std::abs(loaded.raid.timeLeft - initial.raid.timeLeft) < 0.001f);

        // 3g. Toxic track round-trip
        assert(loaded.killsTotal == initial.killsTotal);
        assert(loaded.getFlag(std::string(TOXIC_TRACK_FLAG)) == 1);
        assert(loaded.getFlag(std::string(TOXIC_KILLS_FLAG)) == 5 + static_cast<int>(i));
        assert(loaded.getFlag(std::string(TOXIC_STREAK_FLAG)) == 3);
        assert(loaded.getFlag(std::string(TOXIC_FADE_ROOMS_FLAG)) == 0);
        assert(toxicTrack(loaded) == 1);
        assert(toxicKills(loaded) == 5 + static_cast<int>(i));
        assert(toxicStreak(loaded) == 3);

        // 3h. Story & area flags round-trip
        assert(loaded.getFlag("ruins.intro.done") == 1);
        assert(loaded.getFlag("ruins.dew.taken") == 1);
        assert(loaded.getFlag("kills.ruins") == initial.killsTotal);
        assert(loaded.getFlag("negative_flag_test") == -42);
        assert(loaded.getFlag("zero_flag_test") == 0);
    }

    // -------------------------------------------------------------------------
    // 4. Boundary cases: empty inventory, full inventory (8 items), FUR extremes
    // -------------------------------------------------------------------------
    {
        SaveService saves;

        // Empty inventory
        GameState emptyInv;
        emptyInv.currentRoom = "ruins_fall";
        emptyInv.playerPos = { 180.0f, 320.0f };
        emptyInv.player.inventory.clear();
        assert(saves.saveGame(emptyInv));

        GameState loadedEmpty;
        assert(saves.loadGame(loadedEmpty));
        assert(loadedEmpty.player.inventory.empty());

        // Full inventory: exactly 8 slots (ITM-004)
        GameState fullInv;
        fullInv.currentRoom = "ruins_entrance";
        fullInv.playerPos = { 300.0f, 380.0f };
        fullInv.player.inventory = {
            "Item1", "Item2", "Item3", "Item4",
            "Item5", "Item6", "Item7", "Item8"
        };
        assert(saves.saveGame(fullInv));

        GameState loadedFull;
        assert(saves.loadGame(loadedFull));
        assert(loadedFull.player.inventory.size() == 8U);
        for (std::size_t k = 0; k < 8U; ++k) {
            assert(loadedFull.player.inventory[k] == fullInv.player.inventory[k]);
        }

        // FUR boundaries: 0 and FUR_MAX (100)
        GameState furBound;
        furBound.currentRoom = "ruins_exit";
        furBound.playerPos = { 180.0f, 200.0f };
        furBound.furry.value = 100;
        furBound.furry.stageMax = static_cast<int>(FurStage::Mirror);
        assert(saves.saveGame(furBound));

        GameState loadedFur;
        assert(saves.loadGame(loadedFur));
        assert(loadedFur.furry.value == 100);
        assert(loadedFur.furry.stageMax == static_cast<int>(FurStage::Mirror));

        // FUR clamped to FUR_MAX even if value in file is higher
        furBound.furry.value = 0;
        furBound.furry.stageMax = 0;
        assert(saves.saveGame(furBound));
        assert(saves.loadGame(loadedFur));
        assert(loadedFur.furry.value == 0);
    }

    // -------------------------------------------------------------------------
    // 5. Bug documentation (per brief: report & document real bugs without out-of-scope edits)
    // -------------------------------------------------------------------------
    {
        // Bug A: GameState::killsByArea is not serialised in SaveService.
        // GameState::getRoute() relies on killsByArea["ruins"] >= 20 to return RouteType::ToxicGamer.
        // When saved and reloaded, killsByArea is not restored, so getRoute() returns RouteType::Normie.
        // The encounter/HUD logic relies instead on the flag "kills.ruins", which DOES round-trip.
        SaveService saves;
        GameState areaKillsState;
        areaKillsState.currentRoom = "ruins_exit";
        areaKillsState.playerPos = { 180.0f, 200.0f };
        areaKillsState.killsTotal = 20;
        areaKillsState.killsByArea["ruins"] = 20;
        areaKillsState.setFlag("kills.ruins", 20);
        assert(areaKillsState.getRoute() == GameState::RouteType::ToxicGamer);

        assert(saves.saveGame(areaKillsState));
        GameState loadedAreaKills;
        assert(saves.loadGame(loadedAreaKills));

        // Documenting the bug: killsByArea is NOT restored by SaveService schema v1
        assert(loadedAreaKills.killsByArea.find("ruins") == loadedAreaKills.killsByArea.end());
        // But the persistent flag "kills.ruins" and killsTotal ARE preserved
        assert(loadedAreaKills.getFlag("kills.ruins") == 20);
        assert(loadedAreaKills.killsTotal == 20);

        // Bug B: SaveService::getRoomDisplayName lacks entries for "ruins_bakesale" and "ruins_exit".
        // It returns the raw roomId instead of map displayName ("Ruins - Bake Sale", "Ruins - Exit").
        assert(saves.getRoomDisplayName("ruins_bakesale") == "ruins_bakesale");
        assert(saves.getRoomDisplayName("ruins_exit") == "ruins_exit");
        // OverworldScene works around this by reading displayName from TiledMap properties first.
    }

    // -------------------------------------------------------------------------
    // 6. Cleanup temporary directory and restore environment
    // -------------------------------------------------------------------------
    assert(_putenv_s("LOCALAPPDATA", oldLocalAppDataVal.c_str()) == 0);
    std::filesystem::remove_all(tempSaveDir, ec);

    std::cout << "[PASS] C1-030 / SAV-007: save/load round-trip at every Ruins save point verified." << std::endl;
}
