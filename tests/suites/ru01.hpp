#pragma once

// T01 / C1-005: the first real fight of the Ruins (R5, Fursuit Dummy) must happen on a real walk
// R1 -> R5, and nothing in the trigger -> cutscene -> battle chain may lose it silently.

#include "core/Cutscene.hpp"
#include "core/RoomObjects.hpp"
#include "core/TiledMap.hpp"

#include <algorithm>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace ru01_detail {

using nundertale::TiledMap;
using nundertale::TiledObject;
using nundertale::TiledRect;

struct Box {
    float x = 0.0f;
    float y = 0.0f;
    float w = 0.0f;
    float h = 0.0f;
};

inline bool overlap(const Box& a, const Box& b) {
    return a.x < b.x + b.w && b.x < a.x + a.w && a.y < b.y + b.h && b.y < a.y + a.h;
}

inline Box boxOf(const TiledObject& object) {
    const TiledRect rect = object.bounds();
    return { rect.x, rect.y, rect.width, rect.height };
}

// Player collision box (src/entities/Player.cpp: getCollisionBox) and walking speed per 30 Hz tick
// (include/core/Constants.hpp: PLAYER_SPEED 180 px/s).
inline Box footBox(float px, float py) { return { px + 5.0f, py + 38.0f, 28.0f, 18.0f }; }
constexpr float STEP = 180.0f / 30.0f;

inline std::string slurp(const std::string& path) {
    std::ifstream file(path);
    assert(file && "file must exist");
    std::ostringstream text;
    text << file.rdbuf();
    return text.str();
}

inline TiledMap loadRoom(const std::string& room) {
    TiledMap map;
    const bool loaded = nundertale::loadTiledMapFromFile("data/maps/" + room + ".json", map);
    assert(loaded);
    return map;
}

inline std::vector<const TiledObject*> roomObjects(const TiledMap& map, const std::string& layerName) {
    std::vector<const TiledObject*> out;
    for (const nundertale::TiledLayer& layer : map.layers) {
        if (layer.type != nundertale::TiledLayerType::Objects || layer.name != layerName) continue;
        for (const TiledObject& object : layer.objects) out.push_back(&object);
    }
    return out;
}

inline nundertale::Cutscene loadCutscene(const std::string& id) {
    nundertale::Cutscene cutscene;
    const bool parsed = nundertale::parseCutsceneJson(slurp("data/cutscenes/" + id + ".json"), cutscene);
    assert(parsed);
    return cutscene;
}

// Records what the runner asks of its host; dialogs close on their own.
struct RecordingHost final : nundertale::CutsceneHost {
    std::vector<std::string> flags;
    std::vector<std::string> battles;
    int dialogs = 0;
    bool battleSucceeds = true;
    bool battleStarted = false;

    void dialog(const std::string&, const std::vector<std::string>&) override { ++dialogs; }
    bool dialogOpen() const override { return false; }
    void sound(const std::string&, float) override {}
    void setFlag(const std::string& key, int) override { flags.push_back(key); }
    void shake(float, float) override {}
    void flash(int, int, int, float) override {}
    void startBattle(const std::string& encounter) override {
        battles.push_back(encounter);
        battleStarted = battleSucceeds;
    }
    void setFur(int) override {}
    void healFull() override {}
    void facePlayer(const std::string&) override {}
    void playerAnim(const std::string&, const std::string&, float) override {}
};

} // namespace ru01_detail

inline void suite_ru01() {
    using namespace nundertale;
    using namespace ru01_detail;

    // --- 1. once-flag policy: a fight cutscene burns the flag only when the battle started ---------
    assert(triggerOnceShouldBurn(false, false));  // plain cutscene: burn on start (unchanged)
    assert(triggerOnceShouldBurn(false, true));
    assert(!triggerOnceShouldBurn(true, false));  // battle could not be built: the fight stays available
    assert(triggerOnceShouldBurn(true, true));

    // --- 2. cutscene data: R5 hands over to the dummy; R2 (Floofy) is still only a cutscene -------
    const Cutscene dummy = loadCutscene("r5_dummy");
    assert(cutsceneStartsBattle(dummy));
    assert(!dummy.steps.empty());
    assert(dummy.steps.back().type == CutsceneStepType::StartBattle);
    assert(dummy.steps.back().encounter == "fursuit_dummy");
    for (std::size_t index = 0; index + 1 < dummy.steps.size(); ++index) {
        // nothing before the battle may burn the intro flag or end the cutscene early
        assert(dummy.steps[index].type != CutsceneStepType::StartBattle);
        assert(!(dummy.steps[index].type == CutsceneStepType::SetFlag &&
                 dummy.steps[index].key == "ruins.dummy.intro"));
    }
    assert(!cutsceneStartsBattle(loadCutscene("r1_wake")));
    assert(!cutsceneStartsBattle(loadCutscene("r2_floofy")));  // C1-002: the Floofy battle is a separate task
    assert(!cutsceneStartsBattle(Cutscene{}));

    // --- 3. runner handoff: the battle is requested exactly once, and it is the last thing ---------
    {
        CutsceneRunner runner;
        RecordingHost host;
        runner.start(dummy);
        for (int tick = 0; tick < 20 && runner.running(); ++tick) runner.update(1.0f / 30.0f, host);
        assert(!runner.running());
        assert(host.battles.size() == 1 && host.battles[0] == "fursuit_dummy");
        assert(host.dialogs >= 1);
        assert(host.battleStarted);
        runner.update(1.0f / 30.0f, host);  // a finished runner must not start a second fight
        assert(host.battles.size() == 1);
    }
    {
        // A failing battle build ends the cutscene; the host (not the runner) must keep the flag free.
        CutsceneRunner runner;
        RecordingHost host;
        host.battleSucceeds = false;
        runner.start(dummy);
        for (int tick = 0; tick < 20 && runner.running(); ++tick) runner.update(1.0f / 30.0f, host);
        assert(!runner.running());
        assert(!host.battleStarted);
        assert(!triggerOnceShouldBurn(cutsceneStartsBattle(dummy), host.battleStarted));
    }

    // --- 4. the fight data behind the cutscene exists and is wired --------------------------------
    assert(std::filesystem::exists("data/encounters/fursuit_dummy.json"));
    assert(std::filesystem::exists("data/enemies/fursuit_dummy.json"));
    {
        tiled_detail::JsonValue encounter;
        const std::string text = slurp("data/encounters/fursuit_dummy.json");
        const bool parsed = tiled_detail::JsonParser(text).parse(encounter);
        assert(parsed);
        const tiled_detail::JsonValue* enemies = encounter.field("enemies");
        assert(enemies != nullptr && enemies->array.size() == 1);
        assert(enemies->array[0].string == "fursuit_dummy");
    }

    // --- 5. room chain R1 -> R5: every door lands on an existing spawn of the next room ------------
    const std::vector<std::string> chain = { "ruins_fall", "ruins_floofy", "ruins_entrance",
                                             "ruins_switches", "ruins_dummy" };
    for (std::size_t index = 0; index + 1 < chain.size(); ++index) {
        const TiledMap from = loadRoom(chain[index]);
        bool found = false;
        for (const TiledObject* object : roomObjects(from, "objects")) {
            if (tiledObjectType(*object) != "door") continue;
            if (tiledStringProperty(object->properties, "targetRoom") != chain[index + 1]) continue;
            found = true;
            const TiledMap to = loadRoom(chain[index + 1]);
            const std::string spawn = tiledStringProperty(object->properties, "targetSpawn", "spawn");
            assert(findRoomSpawn(to, spawn) != nullptr);
        }
        assert(found);
    }

    // --- 6. R5 intro trigger: full width, thick enough to never be stepped over, first in line -----
    const TiledMap r5 = loadRoom("ruins_dummy");
    const std::vector<const TiledObject*> objects = roomObjects(r5, "objects");
    const TiledObject* intro = nullptr;
    std::size_t introIndex = 0;
    for (std::size_t index = 0; index < objects.size(); ++index) {
        if (objects[index]->name == "trigger_intro") { intro = objects[index]; introIndex = index; }
    }
    assert(intro != nullptr);
    assert(tiledStringProperty(intro->properties, "cutscene") == "r5_dummy");
    assert(tiledStringProperty(intro->properties, "once") == "ruins.dummy.intro");
    assert(tiledStringProperty(intro->properties, "showIf").empty());
    assert(tiledStringProperty(intro->properties, "hideIf").empty());
    const Box band = boxOf(*intro);

    float leftWall = 0.0f;
    float rightWall = r5.pixelWidth();
    std::vector<Box> solids;
    for (const TiledObject* wall : roomObjects(r5, "collision")) {
        const Box box = boxOf(*wall);
        solids.push_back(box);
        if (wall->name == "left_wall") leftWall = box.x + box.w;
        if (wall->name == "right_wall") rightWall = box.x;
    }
    assert(band.x <= leftWall && band.x + band.w >= rightWall);  // no column of the floor escapes it
    assert(band.h >= 18.0f + STEP);                              // foot box + one step: cannot be skipped

    // every other trigger in R5 that can fire without a flag must come after the intro in the list
    for (std::size_t index = 0; index < objects.size(); ++index) {
        if (tiledObjectType(*objects[index]) != "trigger" || index == introIndex) continue;
        assert(!tiledStringProperty(objects[index]->properties, "showIf").empty() || index > introIndex);
    }

    // solid scenery that the straight walk has to avoid (visible on a fresh game)
    for (const TiledObject* object : objects) {
        const std::string type = tiledObjectType(*object);
        const bool solid = type == "npc" ? tiledBoolProperty(object->properties, "solid", true)
                                         : type == "prop" && tiledBoolProperty(object->properties, "solid", false);
        // hideIf conditions (e.g. "killed >= 1") are all false on a fresh game
        if (solid && tiledStringProperty(object->properties, "showIf").empty()) {
            solids.push_back(boxOf(*object));
        }
    }

    // --- 7. walk north from every south spawn, along several columns: the intro is always met ------
    int walks = 0;
    for (const TiledObject* spawn : objects) {
        if (tiledObjectType(*spawn) != "spawn" || spawn->y <= band.y + band.h) continue;
        for (float x = leftWall - 5.0f; x <= rightWall - 33.0f; x += 7.0f) {
            float py = spawn->y;
            bool blocked = false;
            bool reached = false;
            bool doorFirst = false;
            while (py > 0.0f && !blocked) {
                const float next = py - STEP;
                const Box foot = footBox(x, next);
                for (const Box& solid : solids) {
                    if (overlap(foot, solid)) blocked = true;
                }
                if (blocked) break;
                py = next;
                const Box here = footBox(x, py);
                for (const TiledObject* object : objects) {
                    if (tiledObjectType(*object) == "door" && overlap(here, boxOf(*object)) && !reached) {
                        doorFirst = true;
                    }
                }
                if (overlap(here, band)) reached = true;
                if (doorFirst || reached) break;
            }
            if (blocked && !reached) continue;  // a column walled off by scenery is walked around, not through
            assert(reached && !doorFirst);
            ++walks;
        }
    }
    assert(walks > 20);

    // --- 8. no other cutscene or map sets the intro flag behind the trigger's back -------------------
    int writers = 0;
    for (const auto& entry : std::filesystem::directory_iterator("data/cutscenes")) {
        if (entry.path().extension() != ".json") continue;
        if (slurp(entry.path().string()).find("ruins.dummy.intro") != std::string::npos) ++writers;
    }
    assert(writers == 0);
    int triggersUsingFlag = 0;
    for (const auto& entry : std::filesystem::directory_iterator("data/maps")) {
        if (entry.path().extension() != ".json") continue;
        TiledMap map;
        const bool loaded = loadTiledMapFromFile(entry.path().string(), map);
        assert(loaded);
        for (const TiledObject* object : roomObjects(map, "objects")) {
            if (tiledStringProperty(object->properties, "once") == "ruins.dummy.intro") ++triggersUsingFlag;
        }
    }
    assert(triggersUsingFlag == 1);

    std::cout << "[PASS] ru01 (T01/C1-005): R5 intro trigger spans the floor and is first in line, "
                 "r5_dummy ends in startBattle fursuit_dummy, once-flag burns only when the battle starts, "
                 "R1-R5 door chain intact." << std::endl;
}
