#pragma once

#include "core/Cutscene.hpp"
#include "core/RoomObjects.hpp"
#include "core/TiledMap.hpp"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace ru15_detail {

inline std::string slurp(const std::string& path) {
    std::ifstream file(path);
    assert(file.good() && "File must exist and be readable");
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

inline nundertale::TiledMap loadMap(const std::string& roomId) {
    nundertale::TiledMap map;
    const bool loaded = nundertale::loadTiledMapFromFile("data/maps/" + roomId + ".json", map);
    assert(loaded && "Failed to load map file");
    return map;
}

inline nundertale::Cutscene loadCutscene(const std::string& cutsceneId) {
    nundertale::Cutscene cutscene;
    const std::string json = slurp("data/cutscenes/" + cutsceneId + ".json");
    const bool parsed = nundertale::parseCutsceneJson(json, cutscene);
    assert(parsed && "Failed to parse cutscene JSON");
    assert(!cutscene.steps.empty() && "Cutscene must have at least one step");
    return cutscene;
}

inline bool hasLineContaining(const nundertale::Cutscene& cutscene, const std::string& needle) {
    for (const auto& step : cutscene.steps) {
        if (step.type != nundertale::CutsceneStepType::Dialog) continue;
        for (const auto& line : step.lines) {
            if (line.find(needle) != std::string::npos) return true;
        }
    }
    return false;
}

inline bool setsFlag(const nundertale::Cutscene& cutscene, const std::string& flag) {
    for (const auto& step : cutscene.steps) {
        if (step.type == nundertale::CutsceneStepType::SetFlag && step.key == flag) {
            return true;
        }
    }
    return false;
}

} // namespace ru15_detail

inline void suite_ru15() {
    using namespace nundertale;
    using namespace ru15_detail;

    // 1. Verify ruins_basement map triggers and once flags
    const TiledMap map = loadMap("ruins_basement");

    const TiledObject* entryTrigger = map.findObject("trigger_r12_entry");
    const TiledObject* stop1Trigger = map.findObject("trigger_r12_stop1");
    const TiledObject* stop2Trigger = map.findObject("trigger_r12_stop2");
    const TiledObject* stop3Trigger = map.findObject("trigger_r12_stop3");
    const TiledObject* bossTrigger = map.findObject("trigger_r12_boss");
    const TiledObject* furiellNpc = map.findObject("furiell");

    assert(entryTrigger != nullptr && "trigger_r12_entry must exist in ruins_basement");
    assert(stop1Trigger != nullptr && "trigger_r12_stop1 must exist in ruins_basement");
    assert(stop2Trigger != nullptr && "trigger_r12_stop2 must exist in ruins_basement");
    assert(stop3Trigger != nullptr && "trigger_r12_stop3 must exist in ruins_basement");
    assert(bossTrigger != nullptr && "trigger_r12_boss must exist in ruins_basement");
    assert(furiellNpc != nullptr && "furiell NPC must exist in ruins_basement");

    // All triggers span the corridor width (160 px centered between void walls)
    assert(tiledObjectType(*entryTrigger) == "trigger");
    assert(tiledObjectType(*stop1Trigger) == "trigger");
    assert(tiledObjectType(*stop2Trigger) == "trigger");
    assert(tiledObjectType(*stop3Trigger) == "trigger");
    assert(tiledObjectType(*bossTrigger) == "trigger");

    assert(entryTrigger->width == 160.0f);
    assert(stop1Trigger->width == 160.0f);
    assert(stop2Trigger->width == 160.0f);
    assert(stop3Trigger->width == 160.0f);
    assert(bossTrigger->width == 160.0f);

    // Each stop trigger has once flag
    const std::string entryOnce = tiledStringProperty(entryTrigger->properties, "once");
    const std::string stop1Once = tiledStringProperty(stop1Trigger->properties, "once");
    const std::string stop2Once = tiledStringProperty(stop2Trigger->properties, "once");
    const std::string stop3Once = tiledStringProperty(stop3Trigger->properties, "once");

    assert(!entryOnce.empty() && entryOnce == "ruins.r12.walk");
    assert(!stop1Once.empty() && stop1Once == "ruins.r12.stop1");
    assert(!stop2Once.empty() && stop2Once == "ruins.r12.stop2");
    assert(!stop3Once.empty() && stop3Once == "ruins.r12.stop3");

    // Cutscene associations
    assert(tiledStringProperty(entryTrigger->properties, "cutscene") == "r12_entry");
    assert(tiledStringProperty(stop1Trigger->properties, "cutscene") == "r12_stop1");
    assert(tiledStringProperty(stop2Trigger->properties, "cutscene") == "r12_stop2");
    assert(tiledStringProperty(stop3Trigger->properties, "cutscene") == "r12_stop3");
    assert(tiledStringProperty(bossTrigger->properties, "cutscene") == "r12_boss");

    // Order by Y along the corridor (south entrance to north door):
    // Coordinates decrease from south (1520 max) to north (0 min).
    assert(entryTrigger->y > stop1Trigger->y);
    assert(stop1Trigger->y > stop2Trigger->y);
    assert(stop2Trigger->y > stop3Trigger->y);
    assert(stop3Trigger->y > bossTrigger->y);

    assert(entryTrigger->y == 1340.0f);
    assert(stop1Trigger->y == 1060.0f);
    assert(stop2Trigger->y == 780.0f);
    assert(stop3Trigger->y == 500.0f);
    assert(bossTrigger->y == 240.0f);

    // 2. Cutscenes parse and validate contents
    const Cutscene csEntry = loadCutscene("r12_entry");
    const Cutscene csStop1 = loadCutscene("r12_stop1");
    const Cutscene csStop2 = loadCutscene("r12_stop2");
    const Cutscene csStop3 = loadCutscene("r12_stop3");
    const Cutscene csBoss = loadCutscene("r12_boss");

    // r12_entry: LORE quote about beanbag and fursuit head, no pressure
    assert(hasLineContaining(csEntry, "beanbag"));
    assert(hasLineContaining(csEntry, "fursuit head"));
    assert(hasLineContaining(csEntry, "No pressure."));
    assert(setsFlag(csEntry, "ruins.r12.walk"));

    // r12_stop1: v3.1 voice (pup/hun), warning about cold and endless con floor
    assert(hasLineContaining(csStop1, "hun") || hasLineContaining(csStop1, "pup"));
    assert(setsFlag(csStop1, "ruins.r12.stop1"));

    // r12_stop2: v3.1 voice, gamers getting clipped/streamed
    assert(hasLineContaining(csStop2, "pup") || hasLineContaining(csStop2, "hun"));
    assert(setsFlag(csStop2, "ruins.r12.stop2"));

    // r12_stop3: v3.1 voice, last warning
    assert(hasLineContaining(csStop3, "pup") || hasLineContaining(csStop3, "hun"));
    assert(setsFlag(csStop3, "ruins.r12.stop3"));

    // All stop cutscenes move Furiell forward
    for (const auto* cs : { &csEntry, &csStop1, &csStop2, &csStop3 }) {
        bool movesFuriell = false;
        for (const auto& step : cs->steps) {
            if (step.type == CutsceneStepType::MoveNpc && step.npc == "furiell") {
                movesFuriell = true;
                // Pace tuning: her walking speed is ~80-120 px/s (not 10000 teleport)
                if (step.speed < 500.0f) {
                    assert(step.speed >= 60.0f && step.speed <= 140.0f);
                }
            }
        }
        assert(movesFuriell && "Cutscene must move Furiell forward");
    }

    // r12_boss ends in fight against furiell
    assert(cutsceneStartsBattle(csBoss));
    bool startsFuriellBattle = false;
    for (const auto& step : csBoss.steps) {
        if (step.type == CutsceneStepType::StartBattle && step.encounter == "furiell") {
            startsFuriellBattle = true;
        }
    }
    assert(startsFuriellBattle);

    std::cout << "[PASS] NAR-003 / C1-012 (ru15): basement walk stop triggers, cutscenes, once flags and y-order verified." << std::endl;
}
