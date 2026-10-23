#pragma once

#include "combat/Enemy.hpp"
#include "core/Cutscene.hpp"
#include "core/RoomObjects.hpp"
#include "core/TiledMap.hpp"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace ru38_detail {

inline std::string readFile(const std::string& path) {
    std::ifstream in(path);
    assert(in.good());
    std::stringstream buffer;
    buffer << in.rdbuf();
    return buffer.str();
}

inline nundertale::Cutscene load(const std::string& id) {
    nundertale::Cutscene cutscene;
    const bool parsed = nundertale::parseCutsceneJson(readFile("data/cutscenes/" + id + ".json"), cutscene);
    assert(parsed);
    assert(!cutscene.steps.empty());
    return cutscene;
}

// Plays a cutscene to its end (or to the battle hand-over) against a flag table, like the Overworld host.
struct ChainHost final : nundertale::CutsceneHost {
    std::map<std::string, int> flags;
    int choicePick = 0;  // 0-based
    std::vector<std::string> speakers;
    std::vector<std::string> lines;
    std::vector<std::string> battles;
    std::vector<std::string> steps;  // coarse log: "waterBurst:floofy", "hideNpc:floofy", "healFull", ...
    std::vector<std::string> flagWrites;
    int fur = 77;
    bool healed = false;

    void dialog(const std::string& speaker, const std::vector<std::string>& text) override {
        for (const std::string& line : text) {
            speakers.push_back(speaker);
            lines.push_back(line);
            steps.push_back("line:" + line);
        }
    }
    bool dialogOpen() const override { return false; }
    void sound(const std::string&, float) override {}
    void setFlag(const std::string& key, int value) override {
        flags[key] = value;
        flagWrites.push_back(key);
        steps.push_back("setFlag:" + key);
    }
    void addFlag(const std::string& key, int value) override {
        flags[key] += value;
        flagWrites.push_back(key);
    }
    void shake(float, float) override {}
    void flash(int, int, int, float) override {}
    void startBattle(const std::string& encounter) override { battles.push_back(encounter); }
    void setFur(int value) override { fur = value; steps.push_back("setFur"); }
    void healFull() override { healed = true; steps.push_back("healFull"); }
    void facePlayer(const std::string&) override {}
    void playerAnim(const std::string&, const std::string&, float) override {}
    void hideNpcMarker(const std::string& npc) { steps.push_back("hideNpc:" + npc); }
    void setNpcVisible(const std::string& npc, bool visible) override {
        if (!visible) hideNpcMarker(npc);
    }
    void waterBurst(const std::string& target) override { steps.push_back("waterBurst:" + target); }
    void beginChoice(const std::string&, const std::vector<std::string>&,
                     const std::vector<std::string>&) override {}
    int choiceResult() override { return choicePick; }
    bool condition(const std::string& text) override {
        bool parsed = false;
        const bool result = nundertale::evaluateRoomCondition(text, [this](const std::string& key) {
            const auto it = flags.find(key);
            return it == flags.end() ? 0 : it->second;
        }, &parsed);
        assert(parsed);
        return result;
    }
    bool loadCutscene(const std::string& id, nundertale::Cutscene& cutscene) override {
        cutscene = load(id);
        return true;
    }
    bool movePlayer(float x, float y, float, float) override {
        lastPlayer = { x, y };
        return true;
    }
    struct Point { float x = 0.0f; float y = 0.0f; };
    Point lastPlayer;
};

inline void runToEnd(const std::string& id, ChainHost& host) {
    nundertale::CutsceneRunner runner;
    runner.start(load(id));
    for (int tick = 0; tick < 2000 && runner.running(); ++tick) runner.update(0.1f, host);
    assert(!runner.running());
}

inline bool contains(const std::vector<std::string>& values, const std::string& wanted) {
    for (const std::string& value : values) {
        if (value == wanted) return true;
    }
    return false;
}

inline std::size_t indexOf(const std::vector<std::string>& values, const std::string& wanted) {
    for (std::size_t i = 0; i < values.size(); ++i) {
        if (values[i] == wanted) return i;
    }
    return values.size();
}

} // namespace ru38_detail

// C1-002 / BUG-42: the R2 scene chain (intro -> battle by variant -> scripted end -> flee -> Furiell -> R3)
inline void suite_ru38() {
    using namespace nundertale;
    using namespace ru38_detail;

    // --- 1. the chain only references things that exist ---------------------------------------------
    for (const char* id : {"r2_lair_thought", "r2_floofy_scene", "r2_floofy_intro", "r2_floofy_retry",
                           "r2_floofy_end"}) {
        const Cutscene cutscene = load(id);
        for (const CutsceneStep& step : cutscene.steps) {
            if (step.type == CutsceneStepType::PlayCutscene) {
                assert(std::filesystem::exists("data/cutscenes/" + step.key + ".json"));
            }
            if (step.type == CutsceneStepType::StartBattle) {
                assert(std::filesystem::exists("data/encounters/" + step.encounter + ".json"));
            }
        }
    }
    for (const char* id : {"floofy", "floofy_bass"}) {
        const auto enemy = Enemy::loadFromJson(std::string("data/enemies/") + id + ".json");
        assert(enemy != nullptr);
        assert(enemy->getScriptedEnd().endCutscene == "r2_floofy_end");
        assert(enemy->getScriptedEndAfterTurns() == 6);
        assert(enemy->getTurnOffsetFlag() == "ruins.floofy.deaths");
        const std::string encounter = readFile(std::string("data/encounters/") + id + ".json");
        assert(encounter.find("\"skipFlag\": \"ruins.floofy.deaths\"") != std::string::npos);
    }

    // --- 2. the room: thought step-in, the scene trigger re-arms (no once flag), Furiell is ready -----
    TiledMap room;
    assert(loadTiledMapFromFile("data/maps/ruins_floofy.json", room));
    const TiledObject* sceneTrigger = nullptr;
    const TiledObject* thoughtTrigger = nullptr;
    const TiledObject* furiell = nullptr;
    const TiledObject* flowerbed = nullptr;
    const TiledObject* toEntrance = nullptr;
    for (const TiledLayer& layer : room.layers) {
        if (layer.type != TiledLayerType::Objects) continue;
        for (const TiledObject& object : layer.objects) {
            if (object.name == "trigger_floofy_scene") sceneTrigger = &object;
            if (object.name == "trigger_thought") thoughtTrigger = &object;
            if (object.name == "furiell") furiell = &object;
            if (object.name == "flowerbed") flowerbed = &object;
            if (object.name == "door_to_entrance") toEntrance = &object;
        }
    }
    assert(sceneTrigger != nullptr && thoughtTrigger != nullptr && furiell != nullptr);
    assert(flowerbed != nullptr && toEntrance != nullptr);
    assert(tiledStringProperty(sceneTrigger->properties, "cutscene") == "r2_floofy_scene");
    assert(tiledStringProperty(sceneTrigger->properties, "once").empty());  // BUG-43: retries replay it
    assert(tiledStringProperty(sceneTrigger->properties, "hideIf") == "ruins.floofy.met >= 1");
    assert(tiledStringProperty(thoughtTrigger->properties, "cutscene") == "r2_lair_thought");
    assert(tiledStringProperty(thoughtTrigger->properties, "once") == "ruins.r2.thought");
    assert(thoughtTrigger->y > sceneTrigger->y);  // the step-in thought comes before the center trigger
    assert(tiledStringProperty(furiell->properties, "showIf") == "ruins.furiell.r2_arrived >= 1");
    assert(tiledStringProperty(toEntrance->properties, "targetRoom") == "ruins_entrance");

    // --- 3. first visit (no deaths): thought -> intro with choice 1 -> the floofy encounter ----------
    {
        ChainHost host;
        runToEnd("r2_lair_thought", host);
        assert(host.flags["ruins.r2.thought"] == 1);
        assert(host.lines.size() == 1U && host.lines[0].rfind("* (Fairy lights.", 0) == 0);
    }
    {
        ChainHost host;
        host.choicePick = 0;  // "Sorry."
        runToEnd("r2_floofy_scene", host);
        assert(host.battles.size() == 1U && host.battles[0] == "floofy");  // the battle picks the variant
        assert(host.lines.front() == "Forty-seven flowers.");
        assert(contains(host.lines, "Oh. My. FLUFF."));
        assert(contains(host.lines, "Aww~ manners! On a GAMER! I'm keeping you."));
        assert(!contains(host.lines, "Cute."));
        assert(host.flags["ruins.floofy.choice1"] == 1);
        assert(host.flags["ruins.floofy.annoyed"] == 0);
        assert(host.lines.back() == "I guess little old me will have to do.");
        assert(host.flags.find("ruins.floofy.met") == host.flags.end());
    }
    {
        ChainHost host;
        host.choicePick = 1;  // "You broke my fall."
        host.flags["ruins.floofy.pokes"] = 4;
        runToEnd("r2_floofy_scene", host);
        assert(host.battles.size() == 1U && host.battles[0] == "floofy");
        assert(contains(host.lines, "Also? I felt the poking."));
        assert(contains(host.lines, "Cute."));
        assert(host.flags["ruins.floofy.annoyed"] == 2);  // pokes + the second answer: the Bass Shake
        assert(host.flags.find("ruins.floofy.met") == host.flags.end());
    }

    // --- 4. retry after a WELCOME HOME: straight to the fight, no intro, nothing re-annoys him -------
    {
        ChainHost host;
        host.flags["ruins.floofy.deaths"] = 1;
        host.flags["ruins.floofy.annoyed"] = 2;
        host.flags["ruins.floofy.fight"] = 2;
        runToEnd("r2_floofy_scene", host);
        assert(host.battles.size() == 1U && host.battles[0] == "floofy");
        assert(host.lines.size() == 2U);  // only the retry opener: no beats 1-7, no choice 1
        assert(host.lines[0] == "Back already, darling? I KNEW you'd miss me~");
        assert(host.lines[1] == "Same SOUL. Same smell. Let's go again.");
        assert(host.flags.find("ruins.floofy.choice1") == host.flags.end());
        assert(host.flags["ruins.floofy.annoyed"] == 2);
        assert(host.flags["ruins.floofy.fight"] == 2);  // the locked variant is untouched by the opener
    }

    // --- 5. the end: SPRITZ on Floofy, EEP!!, SAVE line, flowerbed, Furiell, heal, hand, R3 -----------
    {
        ChainHost host;
        host.flags["ruins.floofy.deaths"] = 0;
        host.fur = 63;
        runToEnd("r2_floofy_end", host);
        assert(host.battles.empty());
        const std::size_t burst = indexOf(host.steps, "waterBurst:floofy");
        const std::size_t eep = indexOf(host.steps, "line:EEP!!");
        const std::size_t save = indexOf(host.steps, "line:This isn't over, gamer! I know where you SAVE~!");
        const std::size_t dive = indexOf(host.steps, "hideNpc:floofy");
        const std::size_t shoo = indexOf(host.steps, "line:Bad flower! BAD! *spritz* *spritz* Shoo!");
        const std::size_t heal = indexOf(host.steps, "healFull");
        const std::size_t hand = indexOf(host.steps,
            "line:*grabs your hand* Hewwo~! I'm Furiell! I take care of the Ruins.");
        const std::size_t met = indexOf(host.steps, "setFlag:ruins.floofy.met");
        assert(burst < eep && eep < save && save < dive && dive < shoo && shoo < heal && heal < hand);
        assert(hand < met && met == host.steps.size() - 1U);  // met is the very last thing
        assert(host.healed && host.fur == 0);
        assert(host.flags["ruins.furiell.r2_arrived"] == 1);
        assert(host.flags["ruins.floofy.met"] == 1);
        assert(contains(host.lines, "Oh, you poor pup... did that nasty weed get you all fluffy? >w<"));
        assert(contains(host.lines, "It's just water and a little lavender, hun. It washes the fluff right off~"));
        assert(contains(host.lines, "...I'm not letting go, by the way."));

        // Grub ends the walk on the door north to R3, so the room change starts when the scene is over
        const float boxLeft = host.lastPlayer.x + 5.0f;
        const float boxRight = boxLeft + 28.0f;
        const float boxTop = host.lastPlayer.y + 38.0f;
        const float boxBottom = boxTop + 18.0f;
        assert(boxRight > toEntrance->x && boxLeft < toEntrance->x + toEntrance->width);
        assert(boxBottom > toEntrance->y && boxTop < toEntrance->y + toEntrance->height);
    }

    // --- 6. `met` is set only at the very end of R2, by nothing else in the chain -------------------
    for (const char* id : {"r2_lair_thought", "r2_floofy_scene", "r2_floofy_intro", "r2_floofy_retry"}) {
        for (const CutsceneStep& step : load(id).steps) {
            assert(!((step.type == CutsceneStepType::SetFlag || step.type == CutsceneStepType::AddFlag) &&
                     step.key == "ruins.floofy.met"));
        }
    }
    {
        const Cutscene end = load("r2_floofy_end");
        int writes = 0;
        for (const CutsceneStep& step : end.steps) {
            if (step.type == CutsceneStepType::SetFlag && step.key == "ruins.floofy.met") ++writes;
        }
        assert(writes == 1);
        assert(end.steps.back().type == CutsceneStepType::SetFlag && end.steps.back().key == "ruins.floofy.met");
    }

    // --- 7. the old cutscene-only R2 is retired: nothing in data/ plays it any more -----------------
    for (const auto& entry : std::filesystem::recursive_directory_iterator("data")) {
        if (!entry.is_regular_file() || entry.path().extension() != ".json") continue;
        const std::string text = readFile(entry.path().string());
        for (const char* retired : {"\"r2_floofy\"", "\"r2_floofy_battle\"", "\"r2_floofy_rescue\""}) {
            assert(text.find(retired) == std::string::npos);
        }
    }

    std::cout << "[PASS] C1-002/BUG-42: R2 chain (thought, intro + choice 1, battle by variant, SPRITZ/EEP end, "
                 "Furiell heal + hand to R3), retry skips the intro, met only at the end, old r2_floofy retired."
              << std::endl;
}
