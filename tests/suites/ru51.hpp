#pragma once

#include "core/Cutscene.hpp"
#include "core/RoomObjects.hpp"
#include "core/TiledMap.hpp"

#include <array>
#include <cassert>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace ru51_detail {

inline std::string readText(const std::string& path) {
    std::ifstream input(path);
    assert(input.good());
    std::ostringstream text;
    text << input.rdbuf();
    return text.str();
}

inline nundertale::Cutscene loadCutscene(const std::string& id) {
    nundertale::Cutscene cutscene;
    assert(nundertale::parseCutsceneJson(readText("data/cutscenes/" + id + ".json"), cutscene));
    return cutscene;
}

inline const nundertale::TiledObject& requireObject(
    const nundertale::TiledMap& map, const char* name
) {
    const nundertale::TiledObject* object = map.findObject(name);
    assert(object != nullptr);
    return *object;
}

inline bool pointIn(const nundertale::TiledObject& object, float x, float y) {
    const nundertale::TiledRect bounds = object.bounds();
    return x >= bounds.x && x < bounds.x + bounds.width &&
           y >= bounds.y && y < bounds.y + bounds.height;
}

inline bool centerInViewport(
    const nundertale::TiledObject& object, float x, float y, float width, float height
) {
    const nundertale::TiledRect bounds = object.bounds();
    const float cx = bounds.x + bounds.width * 0.5f;
    const float cy = bounds.y + bounds.height * 0.5f;
    return cx >= x && cx <= x + width && cy >= y && cy <= y + height;
}

} // namespace ru51_detail

inline void suite_ru51() {
    using namespace nundertale;
    using namespace ru51_detail;

    TiledMap room;
    assert(loadTiledMapFromFile("data/maps/ruins_entrance.json", room));
    assert(room.width == 22 && room.height == 14);
    assert(room.tileWidth == 40 && room.tileHeight == 40);
    assert(room.tilesets.size() == 1U);
    assert(room.tilesets[0].name == "ruins_hall");
    assert(room.tilesets[0].image == "assets/tiles/ruins_hall.png");
    assert(room.tilesets[0].columns == 8 && room.tilesets[0].tileCount == 32);

    const TiledLayer* ground = room.findLayer("ground");
    const TiledLayer* collisions = room.findLayer("collision");
    assert(ground != nullptr && collisions != nullptr);
    assert(ground->data.size() == 22U * 14U);
    for (const std::uint32_t gid : ground->data) assert(gid >= 1U && gid <= 32U);

    struct HallProp {
        const char* name;
        const char* sprite;
    };
    constexpr std::array<HallProp, 6> hallProps{{
        {"hall_welcome_sign", "assets/props/ruins_hall_welcome_sign.png"},
        {"hall_scanner", "assets/props/ruins_hall_scanner.png"},
        {"hall_schedule", "assets/props/ruins_hall_schedule.png"},
        {"hall_bench", "assets/props/ruins_hall_bench.png"},
        {"hall_snacks", "assets/props/ruins_hall_snacks.png"},
        {"hall_light", "assets/props/ruins_hall_light.png"},
    }};
    for (const HallProp& expected : hallProps) {
        const TiledObject& prop = requireObject(room, expected.name);
        assert(tiledObjectType(prop) == "prop");
        assert(tiledStringProperty(prop.properties, "sprite") == expected.sprite);
        assert(tiledStringProperty(prop.properties, "cutscene") == "r3_hall");
        assert(!tiledStringProperty(prop.properties, "lines").empty());
        assert(std::filesystem::exists(expected.sprite));
    }

    const TiledObject& banner = requireObject(room, "banner_deodorant");
    const TiledObject& pile = requireObject(room, "dorito_pile");
    assert(tiledStringProperty(banner.properties, "sprite") ==
           "assets/props/ruins_banner_deodorant.png");
    assert(tiledStringProperty(pile.properties, "sprite") ==
           "assets/props/ruins_wrapper_pile.png");
    assert(tiledStringProperty(banner.properties, "cutscene") == "r3_items");
    assert(tiledStringProperty(pile.properties, "cutscene") == "r3_items");
    assert(requireObject(room, "savepoint").type == "savepoint");
    assert(tiledStringProperty(room.properties, "savePlace") ==
           "* The distant hum of dial-up internet fills you with DETERMINATION.");

    // A single existing two-frame prop supplies the gentle flicker; the hall tubes
    // themselves remain dead/static, and no flash effect is attached to the room.
    const TiledObject& flicker = requireObject(room, "hall_light_flicker_soft");
    assert(tiledStringProperty(flicker.properties, "sprite") == "assets/props/ruins_lamp.png");
    assert(tiledStringProperty(flicker.properties, "anim") == "flicker");

    // The canonical hand-in-hand intro remains the once-only arrival trigger.
    const TiledObject& intro = requireObject(room, "trigger_furiell");
    assert(tiledStringProperty(intro.properties, "cutscene") == "r3_furiell");
    assert(tiledStringProperty(intro.properties, "once") == "ruins.furiell.r3");
    const Cutscene arrival = loadCutscene("r3_furiell");
    assert(!arrival.steps.empty());
    assert(arrival.steps[0].lines[0] ==
           "Welcome to your new home, pup! uwu ...Do you like bwankets? I have forty.");
    assert(loadCutscene("r3_hall").steps.size() == 12U);
    assert(loadCutscene("r3_items").steps.size() == 4U);

    // R2 and R4 door/spawn contracts, including their original coordinates.
    const TiledObject& southDoor = requireObject(room, "door_to_floofy");
    const TiledObject& northDoor = requireObject(room, "door_to_switches");
    assert(tiledStringProperty(southDoor.properties, "targetRoom") == "ruins_floofy");
    assert(tiledStringProperty(southDoor.properties, "targetSpawn") == "from_entrance");
    assert(tiledStringProperty(northDoor.properties, "targetRoom") == "ruins_switches");
    assert(tiledStringProperty(northDoor.properties, "targetSpawn") == "from_entrance");
    assert(southDoor.x == 400.0f && southDoor.y == 520.0f);
    assert(northDoor.x == 400.0f && northDoor.y == 40.0f);

    constexpr std::array<const char*, 2> routeSpawns{{"from_floofy", "from_switches"}};
    for (const char* name : routeSpawns) {
        const TiledObject& spawn = requireObject(room, name);
        assert(tiledObjectType(spawn) == "spawn");
        const int column = static_cast<int>(spawn.x) / room.tileWidth;
        const int row = static_cast<int>(spawn.y) / room.tileHeight;
        assert(column >= 0 && column < room.width && row >= 0 && row < room.height);
        const std::size_t index = static_cast<std::size_t>(row * room.width + column);
        assert(ground->data[index] != 0U && ground->data[index] != 32U);
        for (const TiledObject& wall : collisions->objects) {
            assert(!pointIn(wall, spawn.x, spawn.y));
        }
    }
    assert(requireObject(room, "from_floofy").x == 420.0f &&
           requireObject(room, "from_floofy").y == 460.0f);
    assert(requireObject(room, "from_switches").x == 420.0f &&
           requireObject(room, "from_switches").y == 140.0f);

    // At every extreme camera position (640x480 virtual viewport), at least one
    // examinable hall relic is visible. This keeps the 30-second room readable.
    constexpr std::array<std::array<float, 2>, 4> cameraCorners{{
        {{0.0f, 0.0f}}, {{240.0f, 0.0f}}, {{0.0f, 80.0f}}, {{240.0f, 80.0f}},
    }};
    for (const auto& corner : cameraCorners) {
        bool found = false;
        for (const HallProp& expected : hallProps) {
            found = found || centerInViewport(
                requireObject(room, expected.name), corner[0], corner[1], 640.0f, 480.0f);
        }
        found = found || centerInViewport(banner, corner[0], corner[1], 640.0f, 480.0f);
        found = found || centerInViewport(pile, corner[0], corner[1], 640.0f, 480.0f);
        assert(found);
    }

    std::cout << "[PASS] C1-003 (ru51): abandoned R3 main hall tiles and relics, canonical intro/save, "
                 "R2/R4 route continuity, walkable spawns and viewport examine density verified."
              << std::endl;
}
