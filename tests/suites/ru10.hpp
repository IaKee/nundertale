#pragma once

#include "core/Cutscene.hpp"
#include "core/FurReactions.hpp"
#include "core/RoomObjects.hpp"
#include "core/TiledMap.hpp"

#include <cassert>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace ru10_detail {

inline std::string readFile(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    assert(in.good());
    std::stringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

inline nundertale::Cutscene loadCutscene(const std::string& id) {
    nundertale::Cutscene cutscene;
    const bool parsed = nundertale::parseCutsceneJson(readFile("data/cutscenes/" + id + ".json"), cutscene);
    assert(parsed);
    assert(!cutscene.steps.empty());
    return cutscene;
}

inline bool contains(const std::string& hay, const std::string& needle) {
    return hay.find(needle) != std::string::npos;
}

using Flags = std::map<std::string, int>;

// Same rule as OverworldScene::roomObjectVisible: showIf true and hideIf false.
inline bool visible(const nundertale::TiledObject& object, const Flags& flags, int fur = 0) {
    const auto lookup = [&flags, fur](const std::string& key) {
        if (key == "fur") return fur;
        const auto it = flags.find(key);
        return it == flags.end() ? 0 : it->second;
    };
    const std::string showIf = nundertale::tiledStringProperty(object.properties, "showIf");
    const std::string hideIf = nundertale::tiledStringProperty(object.properties, "hideIf");
    bool parsed = false;
    if (!showIf.empty()) {
        const bool shown = nundertale::evaluateRoomCondition(showIf, lookup, &parsed);
        assert(parsed);
        if (!shown) return false;
    }
    if (!hideIf.empty()) {
        const bool hidden = nundertale::evaluateRoomCondition(hideIf, lookup, &parsed);
        assert(parsed);
        if (hidden) return false;
    }
    return true;
}

inline bool overlapsPlayer(const nundertale::TiledObject& object, float playerX, float playerY) {
    // Player::getCollisionBox(): { x + 5, y + 38, 28, 18 }
    const float px = playerX + 5.0f;
    const float py = playerY + 38.0f;
    const nundertale::TiledRect box = object.bounds();
    return px < box.x + box.width && px + 28.0f > box.x && py < box.y + box.height && py + 18.0f > box.y;
}

} // namespace ru10_detail

inline void suite_ru10() {
    using namespace nundertale;
    using namespace ru10_detail;

    TiledMap room;
    assert(loadTiledMapFromFile("data/maps/ruins_barky.json", room));
    const TiledLayer* objectLayer = room.findLayer("objects");
    assert(objectLayer != nullptr);

    auto object = [&room](const std::string& name) {
        const TiledObject* found = room.findObject(name);
        assert(found != nullptr);
        return found;
    };

    // --- no legacy path: map property, legacy object types, and the hard-coded block are all gone
    {
        assert(room.findProperty("legacyBarky") == nullptr);
        assert(!contains(readFile("data/maps/ruins_barky.json"), "legacyBarky"));
        assert(!contains(readFile("tools/maps/build_ruins_maps.py"), "legacy_barky"));
        const std::string sceneSource = readFile("src/scenes/OverworldScene.cpp");
        const std::string sceneHeader = readFile("include/scenes/OverworldScene.hpp");
        assert(!contains(sceneSource, "legacyBarky") && !contains(sceneHeader, "legacyBarky"));
        assert(!contains(sceneSource, "m_legacyBarky") && !contains(sceneHeader, "m_legacyBarky"));
        assert(!contains(sceneSource, "barky_entrance.json"));
        assert(!contains(sceneSource, "Halt, hairless intruder"));
        for (const TiledObject& obj : objectLayer->objects) {
            assert(obj.type != "interactable");
        }
    }

    // --- every cutscene R9 refers to exists and parses; every NPC they drive is a room object
    {
        std::vector<std::string> ids;
        for (const TiledObject& obj : objectLayer->objects) {
            const std::string id = tiledStringProperty(obj.properties, "cutscene");
            if (!id.empty()) ids.push_back(id);
        }
        for (const char* required : {"r9_barky_intro", "r9_barky_spared", "r9_barky_killed",
                                      "barky_entrance", "r9_dew_bowl", "floormsg_r9"}) {
            bool found = false;
            for (const std::string& id : ids) found = found || id == required;
            assert(found);
        }
        for (const std::string& id : ids) {
            const Cutscene cutscene = loadCutscene(id);
            for (const CutsceneStep& step : cutscene.steps) {
                assert(step.type != CutsceneStepType::Unknown);
                if (!step.npc.empty()) {
                    assert(room.findObject(step.npc) != nullptr);
                }
            }
        }
    }

    // --- intro: howl, Barky drops from the ceiling into a crouch, his line, startBattle barky
    {
        const Cutscene intro = loadCutscene("r9_barky_intro");
        assert(cutsceneStartsBattle(intro));
        assert(intro.steps.back().type == CutsceneStepType::StartBattle);
        assert(intro.steps.back().encounter == "barky");
        assert(intro.steps.front().type == CutsceneStepType::FacePlayer || intro.steps.front().type == CutsceneStepType::Sound);

        int sounds = 0;
        int dropIndex = -1;
        int hereIndex = -1;
        int firstFall = -1;
        int landIndex = -1;
        int dialogIndex = -1;
        int battleIndex = -1;
        const TiledObject* barky = object("npc_barky");
        for (std::size_t i = 0; i < intro.steps.size(); ++i) {
            const CutsceneStep& step = intro.steps[i];
            const int index = static_cast<int>(i);
            if (step.type == CutsceneStepType::Sound && dropIndex < 0 && landIndex < 0) ++sounds;
            if (step.type == CutsceneStepType::SetFlag && step.key == "ruins.r9.barky_here") {
                assert(step.value == 1);
                hereIndex = index;
            }
            if (step.type == CutsceneStepType::MoveNpc && step.npc == "npc_barky") {
                if (firstFall < 0) {
                    // teleported above the room (invisible until barky_here is set)
                    assert(step.y < 0.0f);
                    firstFall = index;
                } else {
                    // drops onto his map position
                    assert(step.x == barky->x && step.y == barky->y);
                    landIndex = index;
                }
            }
            if (step.type == CutsceneStepType::NpcAnim && step.npc == "npc_barky" && step.key == "drop") {
                dropIndex = index;
            }
            if (step.type == CutsceneStepType::Dialog && dialogIndex < 0) dialogIndex = index;
            if (step.type == CutsceneStepType::StartBattle) battleIndex = index;
        }
        assert(sounds >= 2);  // the howl and its echo
        assert(firstFall >= 0 && hereIndex > firstFall && landIndex > hereIndex);
        assert(dropIndex > landIndex && dialogIndex > dropIndex && battleIndex > dialogIndex);

        const CutsceneStep& line = intro.steps[static_cast<std::size_t>(dialogIndex)];
        assert(line.speaker == "BARKY");
        assert(line.lines.size() == 3);
        assert(line.lines[0] == "Halt, hairless intruder! *[awkward howling]*");
        assert(line.lines[1] == "You have stepped into the domain of the Shadow Fang Pack!");
        assert(line.lines[2] == "Prepare to face my fursona!");

        // the drop animation exists in Barky's sheet
        assert(contains(readFile("assets/sprites/ow/barky.json"), "\"drop\""));

        // rematch entrance (talking to him before the fight resolves) is still a fight
        const Cutscene entrance = loadCutscene("barky_entrance");
        assert(cutsceneStartsBattle(entrance));
        assert(entrance.steps.back().encounter == "barky");
    }

    // --- intro trigger: once-flag, only while Barky is unresolved, covers the spawns the player arrives on
    {
        const TiledObject* trigger = object("trigger_barky_intro");
        assert(trigger->type == "trigger");
        assert(tiledStringProperty(trigger->properties, "cutscene") == "r9_barky_intro");
        assert(tiledStringProperty(trigger->properties, "once") == "ruins.r9.barky_intro");
        assert(visible(*trigger, {}));
        assert(visible(*trigger, {{"ruins.barky.state", 0}}));
        assert(!visible(*trigger, {{"ruins.barky.state", 1}}));
        assert(!visible(*trigger, {{"ruins.barky.state", 2}}));
        for (const char* spawnName : {"spawn", "from_notices"}) {
            const TiledObject* spawn = object(spawnName);
            assert(overlapsPlayer(*trigger, spawn->x, spawn->y));
        }
        // entering from R8 must not land on the door back to R8 (the old y=370 spawn bounced straight out)
        const TiledObject* back = object("to_notices");
        const TiledObject* arrive = object("from_notices");
        assert(!overlapsPlayer(*back, arrive->x, arrive->y));
        assert(!overlapsPlayer(*back, object("spawn")->x, object("spawn")->y));
        // coming back from R10 (from_bakesale) is outside the intro band
        assert(!overlapsPlayer(*trigger, object("from_bakesale")->x, object("from_bakesale")->y));
        // the sealed-gate hitbox and the lamp are untouched
        assert(tiledStringProperty(object("ruins_lamp")->properties, "lines") ==
               "* Barky's natural enemy. Or his moon. He hasn't decided.");
    }

    // --- states: what the room shows for each ruins.barky.state
    {
        const TiledObject* intro = object("trigger_barky_intro");
        const TiledObject* barky = object("npc_barky");
        const TiledObject* spared = object("npc_barky_spared");
        const TiledObject* pile = object("fur_pile");
        const TiledObject* sealed = object("door");
        const TiledObject* gate = object("to_bakesale");
        const TiledObject* sparedTrigger = object("trigger_barky_spared");
        const TiledObject* killedTrigger = object("trigger_barky_killed");

        // before the intro: nobody in the arena, gate sealed
        const Flags fresh = {};
        assert(visible(*intro, fresh));
        assert(!visible(*barky, fresh) && !visible(*spared, fresh) && !visible(*pile, fresh));
        assert(visible(*sealed, fresh) && !visible(*gate, fresh));
        assert(!visible(*sparedTrigger, fresh) && !visible(*killedTrigger, fresh));

        // Barky dropped in, fight not resolved: he is there and the gate is still sealed
        const Flags fighting = {{"ruins.r9.barky_here", 1}, {"ruins.r9.barky_intro", 1}};
        assert(visible(*barky, fighting) && !visible(*spared, fighting) && !visible(*pile, fighting));
        assert(visible(*sealed, fighting) && !visible(*gate, fighting));
        assert(tiledStringProperty(barky->properties, "cutscene") == "barky_entrance");
        assert(tiledStringProperty(barky->properties, "sprite") == "assets/sprites/ow/barky.png");

        // spared (state 1): the recruiting NPC, gate open, post-battle trigger armed
        const Flags sparedFlags = {{"ruins.barky.state", 1}, {"ruins.barky.spared", 1},
                                   {"ruins.r9.barky_here", 1}};
        assert(!visible(*barky, sparedFlags) && visible(*spared, sparedFlags) && !visible(*pile, sparedFlags));
        assert(!visible(*sealed, sparedFlags) && visible(*gate, sparedFlags));
        assert(!visible(*intro, sparedFlags));
        assert(visible(*sparedTrigger, sparedFlags) && !visible(*killedTrigger, sparedFlags));
        assert(tiledStringProperty(spared->properties, "cutscene") == "r9_barky_spared");
        assert(tiledStringProperty(sparedTrigger->properties, "once") == "ruins.r9.barky_spared");

        // killed (state 2): the synthetic fur pile, no NPC, gate open
        const Flags killedFlags = {{"ruins.barky.state", 2}, {"ruins.barky.killed", 1},
                                   {"ruins.r9.barky_here", 1}};
        assert(!visible(*barky, killedFlags) && !visible(*spared, killedFlags) && visible(*pile, killedFlags));
        assert(!visible(*sealed, killedFlags) && visible(*gate, killedFlags));
        assert(visible(*killedTrigger, killedFlags) && !visible(*sparedTrigger, killedFlags));
        assert(tiledStringProperty(killedTrigger->properties, "once") == "ruins.r9.barky_killed");
        assert(pile->type == "prop");
        assert(!tiledStringProperty(pile->properties, "sprite").empty());
        assert(contains(tiledStringProperty(pile->properties, "lines"), "gray fur"));

        // north gate: opens on ruins.barky.state >= 1 and leads on to R10
        assert(gate->type == "door");
        assert(tiledStringProperty(gate->properties, "showIf") == "ruins.barky.state >= 1");
        assert(tiledStringProperty(gate->properties, "targetRoom") == "ruins_bakesale");

        // never two Barkys at once, whatever the flags say
        for (int state = 0; state <= 2; ++state) {
            for (int here = 0; here <= 1; ++here) {
                const Flags flags = {{"ruins.barky.state", state}, {"ruins.r9.barky_here", here}};
                int shown = 0;
                if (visible(*barky, flags)) ++shown;
                if (visible(*spared, flags)) ++shown;
                if (visible(*pile, flags)) ++shown;
                assert(shown <= 1);
            }
        }
    }

    // --- spared scene: FUR >= 40 line first (from FurReactions.hpp), then the Shadow Fang Pack recruiting
    {
        const Cutscene scene = loadCutscene("r9_barky_spared");
        assert(!cutsceneStartsBattle(scene));
        const CutsceneStep* furStep = nullptr;
        const CutsceneStep* recruit = nullptr;
        std::size_t furIndex = 0;
        std::size_t recruitIndex = 0;
        for (std::size_t i = 0; i < scene.steps.size(); ++i) {
            const CutsceneStep& step = scene.steps[i];
            if (step.type != CutsceneStepType::Dialog) continue;
            assert(step.speaker == "BARKY");
            if (!step.condition.empty()) {
                furStep = &step;
                furIndex = i;
            } else {
                recruit = &step;
                recruitIndex = i;
            }
        }
        assert(furStep != nullptr && recruit != nullptr && furIndex < recruitIndex);
        assert(furStep->lines.size() == 1);
        assert(furStep->lines[0] == nundertale::furReactionLine(40, "barky"));
        assert(nundertale::furReactionLine(39, "barky").empty());

        // the fur line is gated on fur >= 40: boundaries
        for (int fur : {0, 39, 40, 41, 100}) {
            const bool shown = evaluateRoomCondition(furStep->condition, [fur](const std::string& key) {
                return key == "fur" ? fur : 0;
            });
            assert(shown == !nundertale::furReactionLine(fur, "barky").empty());
        }

        std::string all;
        for (const std::string& line : recruit->lines) all += line + "\n";
        assert(contains(all, "Shadow Fang Pack"));
        assert(contains(all, "northern gate is unlocked"));
        assert(contains(all, "Thanks for not roasting my fursona"));
    }

    // --- killed scene: the LORE line, the pile flag
    {
        const Cutscene scene = loadCutscene("r9_barky_killed");
        assert(!cutsceneStartsBattle(scene));
        bool flag = false;
        bool line = false;
        for (const CutsceneStep& step : scene.steps) {
            if (step.type == CutsceneStepType::SetFlag && step.key == "ruins.r9.fur_pile") {
                assert(step.value == 1);
                flag = true;
            }
            if (step.type == CutsceneStepType::Dialog) {
                for (const std::string& text : step.lines) {
                    line = line || text == "* Only a pile of synthetic wolf fur remains.";
                }
            }
        }
        assert(flag && line);
    }

    // --- Mt. Dew bowl moved to data: same counter flag, a knocked sprite from the 4th can on
    {
        const TiledObject* bowl = object("bowl");
        assert(bowl->type == "prop");
        assert(tiledStringProperty(bowl->properties, "cutscene") == "r9_dew_bowl");
        assert(tiledStringProperty(bowl->properties, "counter") == "ruins.dew.taken");
        assert(tiledStringProperty(bowl->properties, "altAnim") == "knocked");
        assert(tiledStringProperty(bowl->properties, "altAnimIf") == "ruins.dew.taken >= 4");

        const std::string items = readFile("data/items/items.json");
        const Cutscene dew = loadCutscene("r9_dew_bowl");
        int gives = 0;
        for (const CutsceneStep& step : dew.steps) {
            if (step.type != CutsceneStepType::GiveItem) continue;
            ++gives;
            assert(contains(items, "\"" + step.item + "\""));
            assert(!step.lines.empty() && !step.fullLines.empty());
        }
        assert(gives == 4);
        // exactly one step runs per counter value (1..5+)
        for (int taken = 1; taken <= 7; ++taken) {
            int runs = 0;
            for (const CutsceneStep& step : dew.steps) {
                const bool ok = evaluateRoomCondition(step.condition, [taken](const std::string& key) {
                    return key == "ruins.dew.taken" ? taken : 0;
                });
                if (ok) ++runs;
            }
            assert(runs == 1);
        }
    }

    // --- the sign keeps the R8 notice (BUG-19) and the save point is still a room object
    {
        const TiledObject* sign = object("sign");
        assert(sign->type == "sign");
        assert(contains(tiledStringProperty(sign->properties, "lines"), "Please do not shed in the ventilation shafts"));
        assert(object("savepoint")->type == "savepoint");
    }

    std::cout << "[PASS] C1-009/BUG-18: R9 runs from room objects + cutscenes (howl and drop intro, fight, spared recruiting with the FUR>=40 line, killed fur pile and flag, gate on ruins.barky.state>=1), no legacyBarky." << std::endl;
}
