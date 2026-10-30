#pragma once

#include "core/Cutscene.hpp"
#include "core/RoomObjects.hpp"
#include "core/TiledMap.hpp"

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace ru48_detail {

inline std::string slurp(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    assert(file && "file must exist");
    std::ostringstream text;
    text << file.rdbuf();
    return text.str();
}

inline bool intersects(const nundertale::TiledRect& a, const nundertale::TiledRect& b) {
    return a.x < b.x + b.width && a.x + a.width > b.x &&
           a.y < b.y + b.height && a.y + a.height > b.y;
}

} // namespace ru48_detail

inline void suite_ru48() {
    using namespace nundertale;
    using namespace ru48_detail;

    TiledMap map;
    assert(loadTiledMapFromFile("data/maps/ruins_fall.json", map));

    const TiledObject* floofy = map.findObject("floofy");
    const TiledObject* bed = map.findObject("flower_bed");
    const TiledObject* can = map.findObject("can_decal");
    const TiledObject* tidePod = map.findObject("tide_pod");
    const TiledObject* floorMessage = map.findObject("floor_message");
    assert(floofy != nullptr && bed != nullptr && can != nullptr && tidePod != nullptr);
    assert(floorMessage != nullptr);

    assert(tiledObjectType(*floofy) == "npc");
    assert(tiledBoolProperty(floofy->properties, "solid", false));
    assert(tiledObjectType(*bed) == "prop");
    // BUG-R8-01/02: the bed stays walkable (Grub wakes on it and picks the badge out of the dent).
    assert(!tiledBoolProperty(bed->properties, "solid", true));
    assert(tiledObjectType(*can) == "prop");
    assert(tiledBoolProperty(can->properties, "solid", false));
    assert(tiledObjectType(*tidePod) == "prop");
    assert(tiledBoolProperty(tidePod->properties, "solid", false));
    assert(tiledStringProperty(tidePod->properties, "hideIf") == "item.tide_pod.taken >= 1");

    assert(tiledObjectType(*floorMessage) == "prop");
    assert(!tiledBoolProperty(floorMessage->properties, "solid", false));
    assert(tiledStringProperty(floorMessage->properties, "cutscene") == "floormsg_r1");
    assert(floorMessage->x < 160.0f && floorMessage->y >= 340.0f);

    assert(!intersects(tidePod->bounds(), bed->bounds()));
    assert(tidePod->y < bed->y);

    Cutscene floorCutscene;
    assert(parseCutsceneJson(slurp("data/cutscenes/r1_floormsg.json"), floorCutscene));
    assert(!floorCutscene.steps.empty());
    assert(floorCutscene.steps.front().type == CutsceneStepType::Dialog);
    assert(floorCutscene.steps.front().reveal == "fade_glow");

    Cutscene legacyFloorCutscene;
    assert(parseCutsceneJson(slurp("data/cutscenes/floormsg_r1.json"), legacyFloorCutscene));
    assert(legacyFloorCutscene.steps.front().reveal == "fade_glow");

    Cutscene ordinary;
    assert(parseCutsceneJson(
        R"({"steps":[{"type":"dialog","lines":["* Normal."]}]})", ordinary));
    assert(ordinary.steps.front().reveal.empty());

    std::cout << "[PASS] BUG-56/57 + OVW-022: R1 solids, clear Tide Pod, and fade-glow floor message." << std::endl;
}
