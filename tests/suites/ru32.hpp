#pragma once

#include "core/Cutscene.hpp"
#include "core/RoomObjects.hpp"
#include "core/TiledMap.hpp"

#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace ru32_detail {

inline std::string readText(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    assert(in.good());
    std::stringstream buffer;
    buffer << in.rdbuf();
    return buffer.str();
}

inline nundertale::Cutscene loadCutscene(const std::string& id) {
    nundertale::Cutscene cutscene;
    const std::string text = readText("data/cutscenes/" + id + ".json");
    assert(!text.empty());
    assert(nundertale::parseCutsceneJson(text, cutscene));
    assert(!cutscene.steps.empty());
    return cutscene;
}

class PokeTestHost final : public nundertale::CutsceneHost {
public:
    explicit PokeTestHost(nundertale::GameState& state) : m_state(state) {}

    void dialog(const std::string& speaker, const std::vector<std::string>& lines) override {
        lastSpeaker = speaker;
        lastLines = lines;
        dialogsShown += 1;
        m_dialogOpen = true;
    }

    bool dialogOpen() const override { return m_dialogOpen; }
    void closeDialog() { m_dialogOpen = false; }

    void sound(const std::string& path, float) override { soundsPlayed.push_back(path); }
    void setFlag(const std::string& key, int value) override { m_state.setFlag(key, value); }
    void addFlag(const std::string& key, int value) override { m_state.incFlag(key, value); }
    void shake(float, float) override {}
    void flash(int, int, int, float) override {}
    void startBattle(const std::string&) override {}
    void setFur(int value) override { m_state.furry.value = value; }
    void healFull() override {}
    void facePlayer(const std::string&) override {}
    void playerAnim(const std::string&, const std::string&, float) override {}

    bool condition(const std::string& expr) override {
        bool parsed = false;
        const bool ok = nundertale::evaluateRoomCondition(
            expr,
            [this](const std::string& key) { return m_state.getFlag(key, 0); },
            &parsed);
        assert(parsed);
        return ok;
    }

    std::string lastSpeaker;
    std::vector<std::string> lastLines;
    std::vector<std::string> soundsPlayed;
    int dialogsShown = 0;

private:
    nundertale::GameState& m_state;
    bool m_dialogOpen = false;
};

inline void runCutsceneToCompletion(const nundertale::Cutscene& cutscene, PokeTestHost& host) {
    nundertale::CutsceneRunner runner;
    runner.start(cutscene);
    int safety = 0;
    while (runner.running() && safety < 32) {
        runner.update(0.1f, host);
        if (host.dialogOpen()) {
            host.closeDialog();
        }
        ++safety;
    }
    assert(!runner.running());
}

} // namespace ru32_detail

inline void suite_ru32() {
    using namespace nundertale;
    using namespace ru32_detail;

    // 1. Load ruins_fall.json and verify cave tileset, layers, and savePlace
    TiledMap map;
    assert(loadTiledMapFromFile("data/maps/ruins_fall.json", map));
    assert(map.width == 16 && map.height == 12);
    assert(map.tilesets.size() == 1U);
    assert(map.tilesets[0].name == "landing");
    assert(map.tilesets[0].image == "assets/tiles/landing.png");
    assert(tiledStringProperty(map.properties, "savePlace") ==
           "* The scent of lukewarm energy drinks and stale Doritos fills you with DETERMINATION.");

    const TiledLayer* ground = map.findLayer("ground");
    const TiledLayer* above = map.findLayer("above");
    assert(ground != nullptr && ground->data.size() == 16U * 12U);
    assert(above != nullptr && above->data.size() == 16U * 12U);
    // BUG-46 (2nd pass): the Landing is one painted background (cave, nacho stratum, the pool of daylight);
    // the tile layers stay empty so nothing draws over it.
    const TiledLayer* background = map.findLayer("background");
    assert(background != nullptr && background->type == TiledLayerType::Image);
    assert(background->image == "assets/tiles/landing_bg.png");
    for (int gid : ground->data) assert(gid == 0);
    for (int gid : above->data) assert(gid == 0);

    // 2. Spawns: spawn (lying pose) and from_registration (returning from R1a)
    const TiledObject* spawn = findRoomSpawn(map, "spawn");
    assert(spawn != nullptr);
    assert(std::fabs(spawn->x - 323.0f) < 0.01f && std::fabs(spawn->y - 232.0f) < 0.01f);
    assert(tiledStringProperty(spawn->properties, "pose") == "lying");
    assert(tiledStringProperty(spawn->properties, "poseSheet") == "assets/sprites/ow/grub.png");
    assert(tiledStringProperty(spawn->properties, "poseUntil") == "ruins.r1.woke");

    const TiledObject* fromReg = findRoomSpawn(map, "from_registration");
    assert(fromReg != nullptr);
    assert(std::fabs(fromReg->x - 300.0f) < 0.01f && std::fabs(fromReg->y - 140.0f) < 0.01f);
    assert(tiledStringProperty(fromReg->properties, "facing") == "down");

    // 3. North exit to ruins_registration (from_landing) and Con doors prop/cutscene
    const TiledObject* doorReg = map.findObject("door_to_registration");
    assert(doorReg != nullptr);
    assert(tiledObjectType(*doorReg) == "door");
    assert(tiledStringProperty(doorReg->properties, "targetRoom") == "ruins_registration");
    assert(tiledStringProperty(doorReg->properties, "targetSpawn") == "from_landing");

    const TiledObject* conDoors = map.findObject("con_doors");
    assert(conDoors != nullptr);
    assert(tiledObjectType(*conDoors) == "prop");
    assert(tiledStringProperty(conDoors->properties, "sprite") == "assets/props/ruins_con_doors.png");
    assert(tiledStringProperty(conDoors->properties, "cutscene") == "r1_con_doors");

    const Cutscene conDoorsCs = loadCutscene("r1_con_doors");
    {
        GameState state;
        PokeTestHost host(state);
        runCutsceneToCompletion(conDoorsCs, host);
        assert(host.dialogsShown == 1);
        assert(host.lastSpeaker.empty());
        assert(host.lastLines.size() == 2U);
        assert(host.lastLines[0] ==
               "* \"FURNATION CON '14 \xE2\x80\x94 REGISTRATION \xE2\x86\x92\". There are claw marks on the doors.");
        assert(host.lastLines[1] == "* On the inside.");
    }

    // 4. Flower bed (LORE v3.4), Floofy knocked out, leaving sets ruins.floofy.left_landing, dent on return
    const TiledObject* floofy = map.findObject("floofy");
    assert(floofy != nullptr);
    assert(tiledObjectType(*floofy) == "npc");
    assert(tiledStringProperty(floofy->properties, "sprite") == "assets/sprites/ow/floofy.png");
    assert(tiledStringProperty(floofy->properties, "anim") == "knocked_out");
    assert(tiledBoolProperty(floofy->properties, "noTurn", false));
    assert(!tiledBoolProperty(floofy->properties, "solid", true));
    assert(tiledStringProperty(floofy->properties, "hideIf") == "ruins.floofy.left_landing >= 1");
    assert(tiledStringProperty(floofy->properties, "cutscene") == "r1_floofy_poke");

    const TiledObject* pile = map.findObject("flower_bed");
    assert(pile != nullptr);
    assert(tiledObjectType(*pile) == "prop");
    assert(tiledStringProperty(pile->properties, "sprite") == "assets/props/ruins_landing_flowerbed.png");
    assert(tiledStringProperty(pile->properties, "anim") == "floofy");
    assert(tiledStringProperty(pile->properties, "altAnim") == "dent");
    assert(tiledStringProperty(pile->properties, "altAnimIf") == "ruins.floofy.left_landing >= 1");
    assert(tiledStringProperty(pile->properties, "lines") == "* Forty-seven kinds of flowers. Alphabetized. Were.");
    assert(tiledStringProperty(pile->properties, "lines2") == "* There's a fox-shaped dent in the flowers.");

    const TiledObject* dent = map.findObject("flower_dent");
    assert(dent != nullptr);
    assert(tiledStringProperty(dent->properties, "showIf") == "ruins.floofy.left_landing >= 1");
    assert(tiledStringProperty(dent->properties, "lines") == "* There's a fox-shaped dent in the flowers.");

    const TiledObject* leaveTrigger = map.findObject("trigger_leave_landing");
    assert(leaveTrigger != nullptr);
    assert(tiledObjectType(*leaveTrigger) == "trigger");
    assert(tiledStringProperty(leaveTrigger->properties, "once") == "ruins.floofy.left_landing");
    assert(tiledStringProperty(leaveTrigger->properties, "cutscene") == "r1_landing_items");

    {
        GameState state;
        const auto flagLookup = [&state](const std::string& key) { return state.getFlag(key, 0); };
        assert(!evaluateRoomCondition(tiledStringProperty(floofy->properties, "hideIf"), flagLookup));
        assert(selectRoomAnimation("floofy", "dent", "ruins.floofy.left_landing >= 1", flagLookup) == "floofy");

        const Cutscene itemsCs = loadCutscene("r1_landing_items");
        // Examining the flower bed before leaving shows the 47 flowers line
        state.setFlag("ruins.r1.pile_check", 1);
        PokeTestHost hostBefore(state);
        runCutsceneToCompletion(itemsCs, hostBefore);
        assert(hostBefore.dialogsShown == 1);
        assert(hostBefore.lastSpeaker.empty());
        assert(hostBefore.lastLines.size() == 1U);
        assert(hostBefore.lastLines[0] == "* Forty-seven kinds of flowers. Alphabetized. Were.");
        assert(state.getFlag("ruins.floofy.left_landing", 0) == 0);

        // Trigger on leaving sets ruins.floofy.left_landing = 1 without opening dialog
        PokeTestHost hostLeave(state);
        runCutsceneToCompletion(itemsCs, hostLeave);
        assert(hostLeave.dialogsShown == 0);
        assert(state.getFlag("ruins.floofy.left_landing", 0) == 1);
        assert(evaluateRoomCondition(tiledStringProperty(floofy->properties, "hideIf"), flagLookup));
        assert(selectRoomAnimation("floofy", "dent", "ruins.floofy.left_landing >= 1", flagLookup) == "dent");

        // Examining the flower bed after returning shows the fox-shaped dent line
        state.setFlag("ruins.r1.pile_check", 1);
        PokeTestHost hostAfter(state);
        runCutsceneToCompletion(itemsCs, hostAfter);
        assert(hostAfter.dialogsShown == 1);
        assert(hostAfter.lastSpeaker.empty());
        assert(hostAfter.lastLines.size() == 1U);
        assert(hostAfter.lastLines[0] == "* There's a fox-shaped dent in the flowers.");
    }

    // 5. Other LORE §12 R1 examinables: Citrus Blast can, Tide Pod, light beam, AFK floor message, Save Point
    const TiledObject* canDecal = map.findObject("can_decal");
    assert(canDecal != nullptr);
    assert(tiledStringProperty(canDecal->properties, "lines") == "* Citrus Blast, 2014 edition. A vintage.");

    const TiledObject* lightBeam = map.findObject("light_beam");
    assert(lightBeam != nullptr);
    assert(tiledStringProperty(lightBeam->properties, "lines") ==
           "* Light from the surface. One bar of signal. Sometimes.");

    const TiledObject* tidePod = map.findObject("tide_pod");
    assert(tidePod != nullptr);
    assert(tiledStringProperty(tidePod->properties, "sprite") == "assets/props/tide_pod.png");
    assert(tiledStringProperty(tidePod->properties, "cutscene") == "r1_tide_pod");
    assert(tiledStringProperty(tidePod->properties, "hideIf") == "item.tide_pod.taken >= 1");
    assert(tiledStringProperty(tidePod->properties, "lines") ==
           "* The Tide Pod you slipped on. It fell in with you.\n* (Forbidden candy.)");

    const TiledObject* floorMsg = map.findObject("floor_message");
    assert(floorMsg != nullptr);
    assert(tiledStringProperty(floorMsg->properties, "sprite") == "assets/props/floor_message.png");
    assert(tiledStringProperty(floorMsg->properties, "cutscene") == "floormsg_r1");
    assert(tiledStringProperty(floorMsg->properties, "lines") ==
           "* \"hole ahead\" - xX_AFK_Xx. Rated: 47.");

    const TiledObject* savepoint = map.findObject("savepoint");
    assert(savepoint != nullptr);
    assert(std::fabs(savepoint->x - 180.0f) < 0.01f && std::fabs(savepoint->y - 320.0f) < 0.01f);

    // 6. Floofy 4-poke sequence & ruins.floofy.pokes counter (never starts a dialog with speaker FLOOFY)
    const Cutscene pokeCs = loadCutscene("r1_floofy_poke");
    for (const CutsceneStep& step : pokeCs.steps) {
        assert(step.speaker != "FLOOFY");
        assert(step.speaker.empty());
    }
    {
        GameState state;
        assert(state.getFlag("ruins.floofy.pokes", 0) == 0);

        // Poke #1
        {
            PokeTestHost host(state);
            runCutsceneToCompletion(pokeCs, host);
            assert(host.dialogsShown == 1 && host.lastSpeaker.empty());
            assert(host.lastLines == (std::vector<std::string>{
                "* A fox fursuiter covered in flowers. You landed on him.",
                "* He's out cold. He's smiling anyway."
            }));
            assert(state.getFlag("ruins.floofy.pokes", 0) == 1);
        }

        // Poke #2
        {
            PokeTestHost host(state);
            runCutsceneToCompletion(pokeCs, host);
            assert(host.dialogsShown == 1 && host.lastSpeaker.empty());
            assert(host.lastLines == (std::vector<std::string>{
                "* (He broke your fall.)",
                "* (You broke his flower crown.)"
            }));
            assert(state.getFlag("ruins.floofy.pokes", 0) == 2);
        }

        // Poke #3 (with faint buzz sfx)
        {
            PokeTestHost host(state);
            runCutsceneToCompletion(pokeCs, host);
            assert(host.dialogsShown == 1 && host.lastSpeaker.empty());
            assert(!host.soundsPlayed.empty());
            assert(host.lastLines == (std::vector<std::string>{
                "* Something in his paw is buzzing faintly.",
                "* (You decide it's a phone.)"
            }));
            assert(state.getFlag("ruins.floofy.pokes", 0) == 3);
        }

        // Poke #4
        {
            PokeTestHost host(state);
            runCutsceneToCompletion(pokeCs, host);
            assert(host.dialogsShown == 1 && host.lastSpeaker.empty());
            assert(host.lastLines == (std::vector<std::string>{
                "* You poke him again. He giggles in his sleep."
            }));
            assert(state.getFlag("ruins.floofy.pokes", 0) == 4);
        }

        // Poke #5+ repeats #4
        {
            PokeTestHost host(state);
            runCutsceneToCompletion(pokeCs, host);
            assert(host.dialogsShown == 1 && host.lastSpeaker.empty());
            assert(host.lastLines == (std::vector<std::string>{
                "* You poke him again. He giggles in his sleep."
            }));
            assert(state.getFlag("ruins.floofy.pokes", 0) == 5);
        }
    }

    std::cout << "[PASS] C1-001 (suite_ru32): ruins_fall Landing cave, Con doors to R1a, Floofy 4 pokes, and return dent verified."
              << std::endl;
}
