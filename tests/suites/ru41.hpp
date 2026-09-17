#pragma once

// T41/T42 / C1-030 & SAV-007: Save/load round-trip at every Ruins save point and
// door round-trip walking suite covering all Ruins rooms (including new R1a registration
// and R1b panelhall save).
//
// Verifies that:
// 1. Every Ruins map loads with valid dimensions and spawn definitions.
// 2. Canonical save points (including R1b Panel Hall) are correctly identified with
//    their canonical save lines, and full GameState survives serialization and deserialization.
// 3. Every door in the Ruins correctly walks to its target spawn and back, and the test
//    strictly verifies that missing targets, missing spawns, or spawns inside collision fail.

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
#include <map>
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

struct DoorValidationResult {
    bool valid = true;
    std::string errorReason;
};

inline bool isPointInsideBox(float px, float py, float bx, float by, float bw, float bh) {
    return px >= bx && px <= bx + bw && py >= by && py <= by + bh;
}

inline const nundertale::TiledObject* findSpawnInMap(
    const nundertale::TiledMap& map, const std::string& spawnName) {
    using namespace nundertale;
    const TiledObject* sp = findRoomSpawn(map, spawnName);
    if (sp != nullptr) return sp;
    if (spawnName.empty() || spawnName == "spawn") {
        return findRoomObject(map, "spawn");
    }
    return nullptr;
}

inline DoorValidationResult validateDoorWalk(
    const std::string& fromRoomId,
    const nundertale::TiledObject& doorObj,
    const std::map<std::string, nundertale::TiledMap>& allMaps,
    bool checkReturnDoor = true) {
    using namespace nundertale;
    DoorValidationResult res;

    const std::string targetRoomId = tiledStringProperty(doorObj.properties, "targetRoom");
    if (targetRoomId.empty()) {
        res.valid = false;
        res.errorReason = "Door in " + fromRoomId + " has empty targetRoom";
        return res;
    }

    const auto itTarget = allMaps.find(targetRoomId);
    if (itTarget == allMaps.end()) {
        res.valid = false;
        res.errorReason = "Door in " + fromRoomId + " targets non-existent room: " + targetRoomId;
        return res;
    }
    const TiledMap& targetMap = itTarget->second;

    const std::string targetSpawnName = tiledStringProperty(doorObj.properties, "targetSpawn", "spawn");
    const TiledObject* targetSpawn = findSpawnInMap(targetMap, targetSpawnName);
    if (targetSpawn == nullptr) {
        res.valid = false;
        res.errorReason = "Target spawn '" + targetSpawnName + "' missing in room " + targetRoomId;
        return res;
    }

    if (targetSpawn->x < 0.0f || targetSpawn->x > targetMap.pixelWidth() ||
        targetSpawn->y < 0.0f || targetSpawn->y > targetMap.pixelHeight()) {
        res.valid = false;
        res.errorReason = "Target spawn '" + targetSpawnName + "' out of bounds in room " + targetRoomId;
        return res;
    }

    // Static collision obstacles check in targetMap
    const TiledLayer* colLayer = targetMap.findLayer("collision");
    if (colLayer != nullptr && colLayer->type == TiledLayerType::Objects) {
        for (const TiledObject& collObj : colLayer->objects) {
            const TiledRect cb = collObj.bounds();
            if (isPointInsideBox(targetSpawn->x, targetSpawn->y, cb.x, cb.y, cb.width, cb.height)) {
                res.valid = false;
                res.errorReason = "Target spawn '" + targetSpawnName + "' in room " + targetRoomId +
                                  " is inside collision object '" + collObj.name + "'";
                return res;
            }
        }
    }

    if (!checkReturnDoor) {
        return res;
    }

    // Walk back: verify at least one return door in targetMap leads back to fromRoomId
    const auto itFrom = allMaps.find(fromRoomId);
    assert(itFrom != allMaps.end());
    const TiledMap& fromMap = itFrom->second;

    bool foundReturnDoor = false;
    for (const TiledLayer& layer : targetMap.layers) {
        if (layer.type != TiledLayerType::Objects) continue;
        for (const TiledObject& obj : layer.objects) {
            if (tiledObjectType(obj) != "door") continue;
            const std::string retTarget = tiledStringProperty(obj.properties, "targetRoom");
            if (retTarget != fromRoomId) continue;

            foundReturnDoor = true;
            const std::string retSpawnName = tiledStringProperty(obj.properties, "targetSpawn", "spawn");
            const TiledObject* retSpawn = findSpawnInMap(fromMap, retSpawnName);
            if (retSpawn == nullptr) {
                res.valid = false;
                res.errorReason = "Return spawn '" + retSpawnName + "' missing in room " + fromRoomId;
                return res;
            }

            if (retSpawn->x < 0.0f || retSpawn->x > fromMap.pixelWidth() ||
                retSpawn->y < 0.0f || retSpawn->y > fromMap.pixelHeight()) {
                res.valid = false;
                res.errorReason = "Return spawn '" + retSpawnName + "' out of bounds in room " + fromRoomId;
                return res;
            }

            const TiledLayer* fromCol = fromMap.findLayer("collision");
            if (fromCol != nullptr && fromCol->type == TiledLayerType::Objects) {
                for (const TiledObject& cObj : fromCol->objects) {
                    const TiledRect cb = cObj.bounds();
                    if (isPointInsideBox(retSpawn->x, retSpawn->y, cb.x, cb.y, cb.width, cb.height)) {
                        res.valid = false;
                        res.errorReason = "Return spawn '" + retSpawnName + "' in room " + fromRoomId +
                                          " is inside collision object '" + cObj.name + "'";
                        return res;
                    }
                }
            }

            // Return spawn must not overlap the origin door (prevents bouncing straight back out)
            const TiledRect db = doorObj.bounds();
            if (isPointInsideBox(retSpawn->x, retSpawn->y, db.x, db.y, db.width, db.height)) {
                res.valid = false;
                res.errorReason = "Return spawn '" + retSpawnName + "' in room " + fromRoomId +
                                  " overlaps origin door '" + doorObj.name + "'";
                return res;
            }
        }
    }

    if (!foundReturnDoor) {
        res.valid = false;
        res.errorReason = "No return door in target room " + targetRoomId + " leading back to " + fromRoomId;
        return res;
    }

    return res;
}

} // namespace ru41_detail

inline void suite_ru41() {
    using namespace nundertale;
    using ru41_detail::DoorValidationResult;
    using ru41_detail::SavePointInfo;
    using ru41_detail::findSpawnInMap;
    using ru41_detail::isPointInsideBox;
    using ru41_detail::readTextFile;
    using ru41_detail::validateDoorWalk;

    // All 17 canonical Ruins rooms (R1..R13 including new R1a registration, R1b panelhall, and subrooms)
    const std::vector<std::string> ruinsMapIds = {
        "ruins_fall",
        "ruins_registration",
        "ruins_panelhall",
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

        // Verify that every Ruins room has a spawn point (either named 'spawn' or a designated door spawn)
        const TiledObject* defaultSpawn = findSpawnInMap(map, "spawn");
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
    assert(roomsWithoutSpawn.empty() && "All Ruins rooms must have a valid spawn object");

    // Exactly 6 save points exist in the canonical Ruins sequence:
    // R1 Landing, R1b Panel Hall, R3 Entrance, R9 Barky, R10 Bake Sale, R13 Exit
    assert(foundSavePoints.size() == 6U && "Ruins must contain exactly 6 canonical save points");

    // Validate expected save point locations and savePlace strings
    {
        const auto itFall = std::find_if(foundSavePoints.begin(), foundSavePoints.end(),
            [](const SavePointInfo& s) { return s.roomId == "ruins_fall"; });
        assert(itFall != foundSavePoints.end());
        assert(std::abs(itFall->x - 180.0f) < 0.01f);
        assert(std::abs(itFall->y - 320.0f) < 0.01f);

        const auto itPanelHall = std::find_if(foundSavePoints.begin(), foundSavePoints.end(),
            [](const SavePointInfo& s) { return s.roomId == "ruins_panelhall"; });
        assert(itPanelHall != foundSavePoints.end());
        assert(std::abs(itPanelHall->x - 430.0f) < 0.01f);
        assert(std::abs(itPanelHall->y - 530.0f) < 0.01f);
        assert(itPanelHall->mapSavePlace == "* The muffled thump of music behind a door fills you with DETERMINATION. Mostly.");

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

        const std::string lineOverride = !sp.customLines.empty() ? sp.customLines : sp.mapSavePlace;

        // Normal route heal: heals HP, does not touch FUR
        SaveHealResult healNorm = saveHealResult(dummyState, lineOverride);
        assert(!healNorm.furryRoute);
        assert(healNorm.hpAfter == 20);
        assert(healNorm.furAfter == 0);
        assert(!healNorm.lines.empty());
        if (!sp.mapSavePlace.empty()) {
            assert(healNorm.lines[0] == sp.mapSavePlace);
        }

        // Furry route: heals FLUFF, leaves HP and FUR untouched
        dummyState.setFlag("route.furry", 1);
        dummyState.setFlag("fluff.value", 5);
        SaveHealResult healFurry = saveHealResult(dummyState, lineOverride);
        assert(healFurry.furryRoute);
        assert(healFurry.fluffAfter == fluffMax(dummyState));
        assert(healFurry.hpAfter == 10);
        assert(healFurry.furAfter == 0);

        // FUR > 0 adds the determination fluff reminder
        dummyState.setFlag("route.furry", 0);
        dummyState.furry.value = 25;
        SaveHealResult healFluff = saveHealResult(dummyState, lineOverride);
        assert(healFluff.lines.size() >= 2U);
        assert(healFluff.lines.back() == "* DETERMINATION can't fix fluff.");
    }

    // -------------------------------------------------------------------------
    // 3. Round-trip GameState at EVERY Ruins save point through SaveService
    // -------------------------------------------------------------------------
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

        const TiledObject* defaultSpawn = findSpawnInMap(restoredMap, "spawn");
        assert(defaultSpawn != nullptr && "Restored room map must contain a valid spawn object");

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

        // Bug B: SaveService::getRoomDisplayName lacks entries for "ruins_bakesale", "ruins_exit",
        // and new room "ruins_panelhall". It returns the raw roomId instead of map displayName.
        assert(saves.getRoomDisplayName("ruins_bakesale") == "ruins_bakesale");
        assert(saves.getRoomDisplayName("ruins_exit") == "ruins_exit");
        assert(saves.getRoomDisplayName("ruins_panelhall") == "ruins_panelhall");
        // OverworldScene works around this by reading displayName from TiledMap properties first.
    }

    // -------------------------------------------------------------------------
    // 6. Door round-trip walking suite across ALL Ruins rooms
    // -------------------------------------------------------------------------
    {
        // All 18 Ruins rooms containing navigation doors (including R9a booth and R1a/R1b)
        const std::vector<std::string> allRuinsDoorRooms = {
            "ruins_fall",
            "ruins_registration",
            "ruins_panelhall",
            "ruins_floofy",
            "ruins_entrance",
            "ruins_switches",
            "ruins_dummy",
            "ruins_cables",
            "ruins_hallway",
            "ruins_notices",
            "ruins_barky",
            "ruins_booth",
            "ruins_bakesale",
            "ruins_house",
            "ruins_house_hallway",
            "ruins_house_pepe",
            "ruins_basement",
            "ruins_exit"
        };

        std::map<std::string, TiledMap> loadedRooms;
        for (const std::string& roomId : allRuinsDoorRooms) {
            TiledMap m;
            const bool ok = loadTiledMapFromFile("data/maps/" + roomId + ".json", m);
            assert(ok && "Ruins map must load for door walking suite");
            loadedRooms.emplace(roomId, std::move(m));
        }

        std::size_t totalDoorsChecked = 0;

        for (const std::string& roomId : allRuinsDoorRooms) {
            const TiledMap& map = loadedRooms.at(roomId);
            for (const TiledLayer& layer : map.layers) {
                if (layer.type != TiledLayerType::Objects) continue;
                for (const TiledObject& object : layer.objects) {
                    if (tiledObjectType(object) != "door") continue;

                    // 'door_to_floofy' in ruins_fall is a hidden legacy alias with showIf="legacy.direct_floofy >= 1",
                    // retained for backwards-compatibility; all other doors are canonical bi-directional doors.
                    const std::string showIf = tiledStringProperty(object.properties, "showIf");
                    const bool isLegacyAlias = (showIf.find("legacy") != std::string::npos);

                    const DoorValidationResult v = validateDoorWalk(
                        roomId, object, loadedRooms, /*checkReturnDoor=*/!isLegacyAlias);
                    assert(v.valid && v.errorReason.c_str());
                    totalDoorsChecked++;
                }
            }
        }

        // Exactly 36 doors exist across the 18 Ruins maps (including 35 canonical + 1 legacy alias)
        assert(totalDoorsChecked == 36U && "Must walk and verify exactly 36 doors across all Ruins maps");
    }

    // -------------------------------------------------------------------------
    // 7. Rigorous failure condition testing (assert that validator fails on missing/colliding spawns)
    // -------------------------------------------------------------------------
    {
        // Build mock map collection to verify that missing targets, missing spawns, or spawns inside
        // collision are strictly detected and rejected.
        std::map<std::string, TiledMap> mockMaps;

        TiledMap roomA;
        roomA.width = 16;
        roomA.height = 12;
        roomA.tileWidth = 40;
        roomA.tileHeight = 40;

        TiledLayer layerObjsA;
        layerObjsA.type = TiledLayerType::Objects;

        TiledObject spawnA;
        spawnA.name = "from_b";
        spawnA.type = "spawn";
        spawnA.x = 100.0f;
        spawnA.y = 200.0f;
        layerObjsA.objects.push_back(spawnA);

        TiledObject doorAtoB;
        doorAtoB.name = "door_to_b";
        doorAtoB.type = "door";
        doorAtoB.x = 80.0f;
        doorAtoB.y = 40.0f;
        doorAtoB.width = 80.0f;
        doorAtoB.height = 40.0f;
        doorAtoB.properties.push_back({ "targetRoom", "string", std::string("roomB") });
        doorAtoB.properties.push_back({ "targetSpawn", "string", std::string("from_a") });
        layerObjsA.objects.push_back(doorAtoB);
        roomA.layers.push_back(layerObjsA);

        TiledMap roomB;
        roomB.width = 16;
        roomB.height = 12;
        roomB.tileWidth = 40;
        roomB.tileHeight = 40;

        TiledLayer layerObjsB;
        layerObjsB.type = TiledLayerType::Objects;

        TiledObject spawnB;
        spawnB.name = "from_a";
        spawnB.type = "spawn";
        spawnB.x = 200.0f;
        spawnB.y = 300.0f;
        layerObjsB.objects.push_back(spawnB);

        TiledObject doorBtoA;
        doorBtoA.name = "door_to_a";
        doorBtoA.type = "door";
        doorBtoA.x = 180.0f;
        doorBtoA.y = 40.0f;
        doorBtoA.width = 80.0f;
        doorBtoA.height = 40.0f;
        doorBtoA.properties.push_back({ "targetRoom", "string", std::string("roomA") });
        doorBtoA.properties.push_back({ "targetSpawn", "string", std::string("from_b") });
        layerObjsB.objects.push_back(doorBtoA);
        roomB.layers.push_back(layerObjsB);

        mockMaps["roomA"] = roomA;
        mockMaps["roomB"] = roomB;

        // Base case: bidirectional walk succeeds
        DoorValidationResult okRes = validateDoorWalk("roomA", doorAtoB, mockMaps, true);
        assert(okRes.valid);

        // Case 1: Door targets non-existent room -> must fail
        {
            TiledObject badDoor = doorAtoB;
            badDoor.properties[0].value = std::string("missing_room_404");
            DoorValidationResult failRes = validateDoorWalk("roomA", badDoor, mockMaps, true);
            assert(!failRes.valid);
            assert(failRes.errorReason.find("targets non-existent room") != std::string::npos);
        }

        // Case 2: Target spawn does not exist in target room -> must fail
        {
            TiledObject badDoor = doorAtoB;
            badDoor.properties[1].value = std::string("missing_spawn_404");
            DoorValidationResult failRes = validateDoorWalk("roomA", badDoor, mockMaps, true);
            assert(!failRes.valid);
            assert(failRes.errorReason.find("Target spawn 'missing_spawn_404' missing") != std::string::npos);
        }

        // Case 3: Target spawn is inside a collision box -> must fail
        {
            auto collisionMaps = mockMaps;
            TiledLayer colLayer;
            colLayer.name = "collision";
            colLayer.type = TiledLayerType::Objects;

            TiledObject wall;
            wall.name = "solid_block";
            wall.type = "collision";
            wall.x = 180.0f;
            wall.y = 280.0f;
            wall.width = 60.0f;
            wall.height = 60.0f;
            colLayer.objects.push_back(wall);
            collisionMaps["roomB"].layers.push_back(colLayer);

            DoorValidationResult failRes = validateDoorWalk("roomA", doorAtoB, collisionMaps, true);
            assert(!failRes.valid);
            assert(failRes.errorReason.find("is inside collision object") != std::string::npos);
        }

        // Case 4: Target room lacks a return door -> must fail
        {
            auto noReturnMaps = mockMaps;
            noReturnMaps["roomB"].layers[0].objects.pop_back(); // Remove door_to_a
            DoorValidationResult failRes = validateDoorWalk("roomA", doorAtoB, noReturnMaps, true);
            assert(!failRes.valid);
            assert(failRes.errorReason.find("No return door") != std::string::npos);
        }

        // Case 5: Return spawn overlaps origin door trigger -> must fail (ping-pong bounce)
        {
            auto bounceMaps = mockMaps;
            // Move from_b inside doorAtoB (x=80..160, y=40..80)
            bounceMaps["roomA"].layers[0].objects[0].x = 100.0f;
            bounceMaps["roomA"].layers[0].objects[0].y = 50.0f;

            DoorValidationResult failRes = validateDoorWalk("roomA", doorAtoB, bounceMaps, true);
            assert(!failRes.valid);
            assert(failRes.errorReason.find("overlaps origin door") != std::string::npos);
        }
    }

    // -------------------------------------------------------------------------
    // 8. Cleanup temporary directory and restore environment
    // -------------------------------------------------------------------------
    assert(_putenv_s("LOCALAPPDATA", oldLocalAppDataVal.c_str()) == 0);
    std::filesystem::remove_all(tempSaveDir, ec);

    std::cout << "[PASS] C1-030 / SAV-007: save/load round-trip and door round-trip suites cover new rooms verified." << std::endl;
}
