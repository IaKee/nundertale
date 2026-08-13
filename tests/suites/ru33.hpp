#pragma once

#include "core/Cutscene.hpp"
#include "core/TiledMap.hpp"

#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace ru33_detail {

inline std::string readText(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    assert(in.good());
    std::stringstream buffer;
    buffer << in.rdbuf();
    return buffer.str();
}

inline nundertale::Cutscene loadCutscene(const std::string& id) {
    nundertale::Cutscene cutscene;
    assert(nundertale::parseCutsceneJson(
        readText("data/cutscenes/" + id + ".json"), cutscene));
    assert(!cutscene.steps.empty());
    return cutscene;
}

inline std::vector<std::vector<std::string>> dialogLines(const nundertale::Cutscene& cutscene) {
    std::vector<std::vector<std::string>> dialogs;
    for (const nundertale::CutsceneStep& step : cutscene.steps) {
        if (!step.lines.empty()) dialogs.push_back(step.lines);
    }
    return dialogs;
}

inline const nundertale::TiledObject& requireObject(
    const nundertale::TiledMap& map, const std::string& name) {
    const nundertale::TiledObject* object = map.findObject(name);
    assert(object != nullptr);
    return *object;
}

inline void assertDoor(
    const nundertale::TiledMap& map,
    const std::string& doorName,
    const std::string& targetRoom,
    const std::string& targetSpawn) {
    const nundertale::TiledObject& door = requireObject(map, doorName);
    assert(nundertale::tiledObjectType(door) == "door");
    assert(nundertale::tiledStringProperty(door.properties, "targetRoom") == targetRoom);
    assert(nundertale::tiledStringProperty(door.properties, "targetSpawn") == targetSpawn);
}

inline void assertSpawnClearOfDoor(
    const nundertale::TiledMap& map,
    const std::string& spawnName,
    const std::string& doorName) {
    const nundertale::TiledObject& spawn = requireObject(map, spawnName);
    const nundertale::TiledObject& door = requireObject(map, doorName);
    assert(nundertale::tiledObjectType(spawn) == "spawn");
    const bool inside = spawn.x >= door.x && spawn.x <= door.x + door.width &&
                        spawn.y >= door.y && spawn.y <= door.y + door.height;
    assert(!inside);
}

inline void assertQuietRoom(const nundertale::TiledMap& map) {
    assert(nundertale::tiledStringProperty(map.properties, "encounters").empty());
    for (const nundertale::TiledLayer& layer : map.layers) {
        for (const nundertale::TiledObject& object : layer.objects) {
            const std::string type = nundertale::tiledObjectType(object);
            assert(type != "npc");
            assert(type != "trigger");
        }
    }
}

inline void assertExaminer(
    const nundertale::TiledMap& map,
    const std::string& name,
    const std::string& cutscene,
    const std::string& exactLines) {
    const nundertale::TiledObject& object = requireObject(map, name);
    const std::string type = nundertale::tiledObjectType(object);
    assert(type == "prop" || type == "examine" || type == "sign");
    assert(nundertale::tiledStringProperty(object.properties, "cutscene") == cutscene);
    assert(nundertale::tiledStringProperty(object.properties, "lines") == exactLines);
}

} // namespace ru33_detail

inline void suite_ru33() {
    using namespace nundertale;
    using namespace ru33_detail;

    TiledMap landing;
    TiledMap registration;
    TiledMap panelHall;
    TiledMap lounge;
    assert(loadTiledMapFromFile("data/maps/ruins_fall.json", landing));
    assert(loadTiledMapFromFile("data/maps/ruins_registration.json", registration));
    assert(loadTiledMapFromFile("data/maps/ruins_panelhall.json", panelHall));
    assert(loadTiledMapFromFile("data/maps/ruins_floofy.json", lounge));

    assert(registration.width == 18 && registration.height == 15);
    assert(panelHall.width == 16 && panelHall.height == 18);
    assert(tiledStringProperty(registration.properties, "displayName") == "Ruins - Registration");
    assert(tiledStringProperty(panelHall.properties, "displayName") == "Ruins - Panel Hall");
    assert(tiledStringProperty(registration.properties, "music") == "ruins_quiet");
    assert(tiledStringProperty(panelHall.properties, "music") == "ruins_panel_leak");
    assert(tiledStringProperty(panelHall.properties, "savePlace") ==
           "* The muffled thump of music behind a door fills you with DETERMINATION. Mostly.");
    assertQuietRoom(registration);
    assertQuietRoom(panelHall);

    // R1 <-> R1a <-> R1b <-> R2, using the session's shared spawn names.
    assertDoor(landing, "door_to_registration", "ruins_registration", "from_landing");
    assertDoor(registration, "door_to_landing", "ruins_fall", "from_registration");
    assertDoor(registration, "door_to_panelhall", "ruins_panelhall", "from_registration");
    assertDoor(panelHall, "door_to_registration", "ruins_registration", "from_panelhall");
    assertDoor(panelHall, "door_to_lounge", "ruins_floofy", "from_panelhall");
    assertDoor(lounge, "door_to_panelhall", "ruins_panelhall", "from_lounge");

    requireObject(landing, "from_registration");
    requireObject(registration, "from_landing");
    requireObject(registration, "from_panelhall");
    requireObject(panelHall, "from_registration");
    requireObject(panelHall, "from_lounge");
    requireObject(lounge, "from_panelhall");

    assertSpawnClearOfDoor(registration, "from_landing", "door_to_landing");
    assertSpawnClearOfDoor(registration, "from_panelhall", "door_to_panelhall");
    assertSpawnClearOfDoor(panelHall, "from_registration", "door_to_registration");
    assertSpawnClearOfDoor(panelHall, "from_lounge", "door_to_lounge");
    assertSpawnClearOfDoor(lounge, "from_panelhall", "door_to_panelhall");

    assertExaminer(registration, "badge_box", "r1a_items",
                   "* Unclaimed badges, 2014. Nobody came back for them.");
    assertExaminer(registration, "rules_board", "r1a_items",
                   "* CON RULES: 1. No yiffing in the hallways. 2. Please. 3. We mean it.");
    assertExaminer(registration, "escalator", "r1a_items",
                   "* Out of order since 2014. The \"temporarily\" sign is laminated.");
    assertExaminer(registration, "lost_and_found", "r1a_items",
                   "* One fursuit paw. A phone charger. Somebody's sense of shame. (Unclaimed.)");
    assertExaminer(registration, "petal", "r1a_items", "* A pink petal. It's warm.");

    assertExaminer(panelHall, "panel_maintenance", "r1b_items",
                   "* \"Fursuit Maintenance 101\". CANCELLED (smell).");
    assertExaminer(panelHall, "panel_parents", "r1b_items",
                   "* \"Explaining Your Fursona to Your Parents\". FULL.");
    assertExaminer(panelHall, "panel_gamers", "r1b_items",
                   "* \"Gamers: Are They Real?\". 4 PM, 2014. The room is still booked.");
    assertExaminer(panelHall, "petal_trail", "r1b_items",
                   "* The petals lead to the door at the end.\n"
                   "* (You've played enough games to know what that means.)");
    assertExaminer(panelHall, "lounge_door", "r1b_items",
                   "* \"HEADLESS LOUNGE — SUITERS ONLY\". Someone taped \"FLOOFY'S\" over it.");

    const std::vector<std::vector<std::string>> r1aDialogs =
        dialogLines(loadCutscene("r1a_items"));
    assert(r1aDialogs == (std::vector<std::vector<std::string>>{
        {"* Unclaimed badges, 2014. Nobody came back for them."},
        {"* CON RULES: 1. No yiffing in the hallways. 2. Please. 3. We mean it."},
        {"* Out of order since 2014. The \"temporarily\" sign is laminated."},
        {"* One fursuit paw. A phone charger. Somebody's sense of shame. (Unclaimed.)"},
        {"* A pink petal. It's warm."},
    }));

    const std::vector<std::vector<std::string>> r1bDialogs =
        dialogLines(loadCutscene("r1b_items"));
    assert(r1bDialogs == (std::vector<std::vector<std::string>>{
        {"* \"Fursuit Maintenance 101\". CANCELLED (smell)."},
        {"* \"Explaining Your Fursona to Your Parents\". FULL."},
        {"* \"Gamers: Are They Real?\". 4 PM, 2014. The room is still booked."},
        {"* The petals lead to the door at the end.",
         "* (You've played enough games to know what that means.)"},
        {"* \"HEADLESS LOUNGE — SUITERS ONLY\". Someone taped \"FLOOFY'S\" over it."},
    }));

    // Props are distributed through each short room so a funny examine is never more than
    // one 480px viewport away while walking the main north/south path.
    assert(std::fabs(requireObject(registration, "badge_box").y -
                     requireObject(registration, "escalator").y) < 480.0f);
    assert(std::fabs(requireObject(panelHall, "panel_maintenance").y -
                     requireObject(panelHall, "panel_gamers").y) < 480.0f);

    std::cout << "[PASS] C1-015/C1-016 (suite_ru33): quiet Registration and Panel Hall rooms, "
                 "canonical examines, save line, and bidirectional R1-R2 door chain verified."
              << std::endl;
}
