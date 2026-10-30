#pragma once

#include "core/Cutscene.hpp"
#include "core/ItemDatabase.hpp"
#include "core/RoomObjects.hpp"
#include "core/TiledMap.hpp"

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace ru50_detail {

inline std::string readFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    assert(file && "C1-029 data file must exist");
    std::ostringstream text;
    text << file.rdbuf();
    return text.str();
}

} // namespace ru50_detail

inline void suite_ru50() {
    using namespace nundertale;
    using namespace ru50_detail;

    TiledMap r1a;
    assert(loadTiledMapFromFile("data/maps/ruins_registration.json", r1a));
    const TiledObject* bin = r1a.findObject("lost_and_found");
    const TiledObject* binBeacon = r1a.findObject("lost_and_found_beacon");
    const TiledObject* beacon = r1a.findObject("strange_beacon");
    assert(bin != nullptr && binBeacon != nullptr && beacon != nullptr);
    assert(tiledStringProperty(bin->properties, "cutscene") == "r1a_items");
    assert(tiledStringProperty(bin->properties, "hideIf") == "ruins.r1a.lostfound_seen >= 1");
    assert(tiledStringProperty(binBeacon->properties, "cutscene") == "r1a_beacon");
    assert(tiledStringProperty(binBeacon->properties, "showIf") == "ruins.r1a.lostfound_seen >= 1");
    assert(tiledStringProperty(beacon->properties, "sprite") == "assets/props/strange_beacon.png");
    assert(tiledStringProperty(beacon->properties, "anim") == "glow");
    assert(tiledStringProperty(beacon->properties, "showIf") ==
           "ruins.r1a.lostfound_seen >= 1 && ruins.beacon.state == 0");

    Cutscene scene;
    assert(parseCutsceneJson(readFile("data/cutscenes/r1a_beacon.json"), scene));
    bool choice = false;
    bool flash = false;
    bool shake = false;
    bool gives = false;
    bool takenState = false;
    bool leftState = false;
    for (const CutsceneStep& step : scene.steps) {
        assert(step.type != CutsceneStepType::Unknown && step.type != CutsceneStepType::Unsupported);
        assert(step.type != CutsceneStepType::AddFur && step.type != CutsceneStepType::SetFur);
        if (step.type == CutsceneStepType::Choice) {
            choice = step.options.size() == 2 && step.options[0] == "Take it" &&
                     step.options[1] == "Leave it";
        }
        flash = flash || (step.type == CutsceneStepType::Flash && step.r == 255 && step.g == 255 &&
                          step.b == 255);
        shake = shake || step.type == CutsceneStepType::Shake;
        gives = gives || (step.type == CutsceneStepType::GiveItem && step.item == "Strange Beacon");
        takenState = takenState || (step.type == CutsceneStepType::SetFlag &&
                                    step.key == "ruins.beacon.state" && step.value == 1);
        leftState = leftState || (step.type == CutsceneStepType::SetFlag &&
                                  step.key == "ruins.beacon.state" && step.value == 2);
    }
    assert(choice && flash && shake && gives && takenState && leftState);

    const std::string text = readFile("data/cutscenes/r1a_beacon.json");
    assert(text.find("* A NEW PAW TOUCHES THE BEACON.") != std::string::npos);
    assert(text.find("* HEAR ME, GAMER, AND OBEY.") != std::string::npos);
    assert(text.find("* (You put it back immediately. Not doing that quest again.)") != std::string::npos);
    assert(text.find("* It hums. It wants you to clear out a temple.") != std::string::npos);
    assert(readFile("data/cutscenes/r1a_items.json").find("ruins.r1a.lostfound_seen") !=
           std::string::npos);

    ItemDatabase items;
    assert(items.parse(readFile("data/items/items.json")));
    const ItemDef* item = items.findItemByName("Strange Beacon");
    assert(item != nullptr);
    assert(item->id == "strange_beacon");
    assert(item->type == "key");
    assert(item->noDrop);
    assert(item->dropText == "* It won't let go of you.");

    std::cout << "[PASS] C1-029: Meridia's Beacon in R1a's lost and found (take/leave, key item)." << std::endl;
}
