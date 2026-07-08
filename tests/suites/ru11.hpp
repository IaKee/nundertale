#pragma once

#include "core/Cutscene.hpp"
#include "core/Endings.hpp"
#include "core/ItemDatabase.hpp"
#include "core/RoomObjects.hpp"
#include "core/TiledMap.hpp"
#include "ui/DialogBox.hpp"

#include <cassert>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace ru11_detail {

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

inline nundertale::Cutscene parseInline(const std::string& json) {
    nundertale::Cutscene cutscene;
    const bool parsed = nundertale::parseCutsceneJson(json, cutscene);
    assert(parsed);
    return cutscene;
}

inline bool contains(const std::string& hay, const std::string& needle) {
    return hay.find(needle) != std::string::npos;
}

// A host with no UI: records what the runner asks for, evaluates conditions on a real GameState
// and answers a "choice" step after `pollsBeforePick` polls with `pick`.
class RecordingHost final : public nundertale::CutsceneHost {
public:
    nundertale::GameState state;
    nundertale::SessionMeta session;
    std::vector<std::string> log;
    std::vector<std::string> dialogLines;
    std::vector<std::string> choicePages;
    std::vector<std::string> choiceOptions;
    std::vector<std::string> hidden;
    int pick = 0;
    int pollsBeforePick = 0;
    int polls = 0;
    int choicesOpened = 0;

    void dialog(const std::string& speaker, const std::vector<std::string>& lines) override {
        log.push_back("dialog:" + speaker);
        for (const std::string& line : lines) dialogLines.push_back(line);
    }
    bool dialogOpen() const override { return false; }
    void sound(const std::string&, float) override {}
    void setFlag(const std::string& key, int value) override { state.setFlag(key, value); }
    void shake(float, float) override {}
    void flash(int, int, int, float) override {}
    void startBattle(const std::string& encounter) override { log.push_back("battle:" + encounter); }
    void setFur(int) override {}
    void healFull() override {}
    void facePlayer(const std::string&) override {}
    void playerAnim(const std::string&, const std::string&, float) override {}
    bool condition(const std::string& text) override {
        return nundertale::evaluateRoomCondition(text, [this](const std::string& key) {
            return state.getFlag(key, 0);
        });
    }
    void giveItem(const std::string& item, const std::vector<std::string>& lines,
                  const std::vector<std::string>&) override {
        log.push_back("give:" + item);
        for (const std::string& line : lines) dialogLines.push_back(line);
    }
    void addFlag(const std::string& key, int value) override { state.incFlag(key, value); }
    void setNpcVisible(const std::string& npc, bool visible) override {
        log.push_back(std::string(visible ? "show:" : "hide:") + npc);
        if (!visible) hidden.push_back(npc);
    }
    void beginChoice(const std::string&, const std::vector<std::string>& pages,
                     const std::vector<std::string>& options) override {
        ++choicesOpened;
        polls = 0;
        choicePages = pages;
        choiceOptions = options;
    }
    int choiceResult() override {
        return polls++ < pollsBeforePick ? -1 : pick;
    }
    void recordKill(const std::string& enemyId) override {
        log.push_back("kill:" + enemyId);
        nundertale::applyCutsceneKill(state, session, enemyId);
    }
    bool loadCutscene(const std::string& id, nundertale::Cutscene& cutscene) override {
        std::ifstream in("data/cutscenes/" + id + ".json", std::ios::binary);
        if (!in.good()) return false;
        std::stringstream ss;
        ss << in.rdbuf();
        return nundertale::parseCutsceneJson(ss.str(), cutscene);
    }

    bool logged(const std::string& entry) const {
        for (const std::string& line : log) {
            if (line == entry) return true;
        }
        return false;
    }
    bool said(const std::string& line) const {
        for (const std::string& said : dialogLines) {
            if (said == line) return true;
        }
        return false;
    }
};

inline void run(nundertale::CutsceneRunner& runner, RecordingHost& host, int frames = 400) {
    for (int i = 0; i < frames && runner.running(); ++i) runner.update(0.1f, host);
}

using Flags = std::map<std::string, int>;

// Same rule as OverworldScene::roomObjectVisible: showIf true and hideIf false.
inline bool visible(const nundertale::TiledObject& object, const Flags& flags) {
    const auto lookup = [&flags](const std::string& key) {
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

} // namespace ru11_detail

inline void suite_ru11() {
    using namespace nundertale;
    using namespace ru11_detail;

    // --- the "choice" step parses: type, prompt, 2-3 options, flag key, and the "if" on the next steps
    {
        const Cutscene cutscene = parseInline(R"({"steps":[
            {"type":"choice","options":["Sacrifice","Refuse"],"key":"ruins.booth.choice","lines":["* Pick."]},
            {"type":"setFlag","key":"a","value":1,"if":"ruins.booth.choice == 1"}]})");
        assert(cutscene.steps.size() == 2);
        const CutsceneStep& choice = cutscene.steps[0];
        assert(choice.type == CutsceneStepType::Choice);
        assert(choice.key == "ruins.booth.choice");
        assert(choice.options.size() == 2 && choice.options[0] == "Sacrifice" && choice.options[1] == "Refuse");
        assert(choice.lines.size() == 1 && choice.lines[0] == "* Pick.");
        assert(cutsceneChoiceValid(choice));
        assert(cutscene.steps[1].condition == "ruins.booth.choice == 1");
        assert(cutsceneStepTypeFromString("recordKill") == CutsceneStepType::RecordKill);
        assert(cutsceneStepTypeFromString("cutscene") == CutsceneStepType::PlayCutscene);
    }

    // --- option count and text limits: 2 and 3 are fine; 0, 1, 4, empty, '|' and '}' are not
    {
        CutsceneStep step;
        assert(!cutsceneChoiceValid(step));
        step.options = { "Only" };
        assert(!cutsceneChoiceValid(step));
        step.options = { "A", "B" };
        assert(cutsceneChoiceValid(step));
        step.options = { "A", "B", "C" };
        assert(cutsceneChoiceValid(step));
        step.options = { "A", "B", "C", "D" };
        assert(!cutsceneChoiceValid(step));
        step.options = { "A", "" };
        assert(!cutsceneChoiceValid(step));
        step.options = { "A|B", "C" };
        assert(!cutsceneChoiceValid(step));
        step.options = { "A", "C}" };
        assert(!cutsceneChoiceValid(step));
    }

    // --- the dialog box gets the options through its {choice:...} token on the last prompt page
    {
        CutsceneStep step;
        step.lines = { "* First page.", "* Which?" };
        step.options = { "Sacrifice", "Refuse" };
        std::vector<std::string> pages = cutsceneChoicePages(step);
        assert(pages.size() == 2 && pages[0] == "* First page.");
        assert(pages[1] == "* Which?{choice:Sacrifice|Refuse}");
        const ParsedChoicePage parsed = parseChoiceToken(pages[1]);
        assert(parsed.text == "* Which?");
        assert(parsed.options.size() == 2 && parsed.options[0] == "Sacrifice" && parsed.options[1] == "Refuse");
        assert(parseChoiceToken(pages[0]).options.empty());

        step.lines.clear();
        step.options = { "A", "B", "C" };
        pages = cutsceneChoicePages(step);
        assert(pages.size() == 1);
        assert(parseChoiceToken(pages[0]).options.size() == 3);
    }

    // --- running a choice: it waits for the player, stores the 1-based pick, and only the matching branch runs
    {
        const std::string json = R"({"steps":[
            {"type":"choice","options":["A","B","C"],"key":"pick","lines":["* ?"]},
            {"type":"setFlag","key":"ran.a","value":1,"if":"pick == 1"},
            {"type":"setFlag","key":"ran.b","value":1,"if":"pick == 2"},
            {"type":"setFlag","key":"ran.c","value":1,"if":"pick == 3"},
            {"type":"setFlag","key":"after","value":1}]})";
        for (int pick = 0; pick < 3; ++pick) {
            RecordingHost host;
            host.pick = pick;
            host.pollsBeforePick = 2;
            CutsceneRunner runner;
            runner.start(parseInline(json));
            runner.update(0.1f, host);
            assert(runner.running());                          // still choosing
            assert(host.state.getFlag("pick", 0) == 0);
            assert(host.state.getFlag("after", 0) == 0);
            assert(host.choicesOpened == 1 && host.choiceOptions.size() == 3);
            run(runner, host);
            assert(!runner.running());
            assert(host.choicesOpened == 1);                   // the prompt opened once, not per frame
            assert(host.state.getFlag("pick", 0) == pick + 1);
            assert(host.state.getFlag("ran.a", 0) == (pick == 0 ? 1 : 0));
            assert(host.state.getFlag("ran.b", 0) == (pick == 1 ? 1 : 0));
            assert(host.state.getFlag("ran.c", 0) == (pick == 2 ? 1 : 0));
            assert(host.state.getFlag("after", 0) == 1);
        }

        // a host that answers out of range is clamped to a real option
        RecordingHost wild;
        wild.pick = 9;
        CutsceneRunner runner;
        runner.start(parseInline(json));
        run(runner, wild);
        assert(wild.state.getFlag("pick", 0) == 3);

        // a choice with one option is a data error: skipped, no flag, the next steps still run
        RecordingHost bad;
        CutsceneRunner badRunner;
        badRunner.start(parseInline(R"({"steps":[
            {"type":"choice","options":["Lonely"],"key":"pick"},
            {"type":"setFlag","key":"after","value":1}]})"));
        run(badRunner, bad);
        assert(!badRunner.running());
        assert(bad.choicesOpened == 0 && bad.state.getFlag("pick", 0) == 0 && bad.state.getFlag("after", 0) == 1);
    }

    // --- "cutscene" hands over to another file; a missing one is skipped
    {
        RecordingHost host;
        CutsceneRunner runner;
        runner.start(parseInline(R"({"steps":[
            {"type":"cutscene","key":"does_not_exist"},
            {"type":"setFlag","key":"after","value":1},
            {"type":"cutscene","key":"r9a_panel"},
            {"type":"setFlag","key":"never","value":1}]})"));
        run(runner, host);
        assert(!runner.running());
        assert(host.state.getFlag("after", 0) == 1);
        assert(host.state.getFlag("ruins.booth.panel", 0) == 1);   // the panel scene ran
        assert(host.state.getFlag("never", 0) == 0);               // and replaced the rest
    }

    // --- a kill outside a battle sets what a battle kill sets, and the Clean Run is lost
    {
        GameState state;
        SessionMeta session;
        assert(decideEnding(state) == EndingType::CleanRun);
        assert(state.getRoute() == GameState::RouteType::Wholesome);
        assert(applyCutsceneKill(state, session, "barky"));
        assert(state.getFlag("ruins.barky.killed", 0) == 1);
        assert(state.getFlag("ruins.barky.state", 0) == 2);
        assert(state.getFlag("battle.barky.killed", 0) == 1);
        assert(state.getFlag("kills.ruins", 0) == 1);
        assert(state.killsTotal == 1 && state.killsByArea["ruins"] == 1);
        assert(state.getFlag("toxic.track", 0) == 1 && state.getFlag("toxic.kills", 0) == 1);
        assert(session.killsSinceSave == 1);
        assert(decideEnding(state) != EndingType::CleanRun);
        assert(state.getRoute() != GameState::RouteType::Wholesome);

        // the track already ended for good (first spare): the kill still counts, the track does not restart
        GameState ended;
        SessionMeta endedSession;
        ended.setFlag("toxic.track", 2);
        assert(!applyCutsceneKill(ended, endedSession, "barky"));
        assert(ended.getFlag("kills.ruins", 0) == 1 && ended.killsTotal == 1);
        assert(ended.getFlag("toxic.track", 0) == 2 && ended.getFlag("toxic.kills", 0) == 0);

        // nothing to kill: nothing changes
        GameState idle;
        SessionMeta idleSession;
        assert(!applyCutsceneKill(idle, idleSession, ""));
        assert(idle.killsTotal == 0 && idle.getFlag("kills.ruins", 0) == 0);
    }

    // --- the Apurrture Cake exists with the LORE text
    {
        ItemDatabase items;
        assert(items.parse(readFile("data/items/items.json")));
        const ItemDef* cake = items.findItemByName("Apurrture Cake");
        assert(cake != nullptr);
        assert(cake->id == "apurrture_cake" && cake->type == "consumable");
        assert(cake->heal >= 99);                              // full HP at the LV 20 cap
        assert(cake->useText == "* The cake was real. It's delicious.\n* You hate that it's delicious.");
        assert(!cake->info.empty());
    }

    // --- the booth cutscenes parse and use only known steps
    for (const char* id : {"r9a_booth_entry", "r9a_sacrifice", "r9a_refuse", "r9a_panel"}) {
        const Cutscene cutscene = loadCutscene(id);
        for (const CutsceneStep& step : cutscene.steps) {
            assert(step.type != CutsceneStepType::Unknown && step.type != CutsceneStepType::Unsupported);
        }
    }
    {
        const Cutscene entry = loadCutscene("r9a_booth_entry");
        const CutsceneStep* choice = nullptr;
        for (const CutsceneStep& step : entry.steps) {
            if (step.type == CutsceneStepType::Choice) choice = &step;
        }
        assert(choice != nullptr && cutsceneChoiceValid(*choice));
        assert(choice->key == "ruins.booth.choice");
        assert(choice->options.size() == 2 && choice->options[0] == "Sacrifice" && choice->options[1] == "Refuse");
        assert(!cutsceneStartsBattle(entry));                  // the fight sits in r9a_refuse
        assert(cutsceneStartsBattle(loadCutscene("r9a_refuse")));
    }

    // --- Sacrifice: Barky's kill, the cake lines, the Apurrture Cake
    {
        RecordingHost host;
        host.state.setFlag("ruins.barky.state", 1);
        host.pick = 0;
        host.pollsBeforePick = 1;
        CutsceneRunner runner;
        runner.start(loadCutscene("r9a_booth_entry"));
        run(runner, host);
        assert(!runner.running());
        assert(host.choicesOpened == 1);
        assert(host.state.getFlag("ruins.booth.choice", 0) == 1);
        assert(host.state.getFlag("ruins.booth.state", 0) == 2);
        assert(host.logged("kill:barky") && host.logged("hide:npc_barky_booth"));
        assert(host.state.getFlag("ruins.barky.killed", 0) == 1 && host.state.getFlag("ruins.barky.state", 0) == 2);
        assert(host.state.getFlag("battle.barky.killed", 0) == 1 && host.state.getFlag("kills.ruins", 0) == 1);
        assert(host.state.getFlag("toxic.track", 0) == 1);
        assert(host.state.killsTotal == 1);
        assert(host.logged("give:Apurrture Cake"));
        assert(!host.logged("battle:gladowos"));
        assert(host.said("* You put down your faithful companion faster than any test subject on record."));
        assert(host.said("* The cake was not a lie."));
        assert(host.said("* That's the worst part."));
        assert(host.said("...if it's for science, bro."));
        assert(host.said("The Enrichment Center requires you to place your companion into the Emergency Fursuit Incinerator."));
        assert(host.choicePages.size() == 1 && contains(host.choicePages[0], "{choice:Sacrifice|Refuse}"));
    }

    // --- Refuse: the other test (startBattle gladowos), Barky lives, nothing counts as a kill
    {
        RecordingHost host;
        host.state.setFlag("ruins.barky.state", 1);
        host.pick = 1;
        CutsceneRunner runner;
        runner.start(loadCutscene("r9a_booth_entry"));
        run(runner, host);
        assert(!runner.running());
        assert(host.state.getFlag("ruins.booth.choice", 0) == 2);
        assert(host.logged("battle:gladowos"));
        assert(host.said("Disappointing.") && host.said("Initiating... the other test."));
        assert(host.state.getFlag("ruins.booth.state", 0) == 1);   // entered, nobody sacrificed
        assert(host.state.getFlag("ruins.booth.refused", 0) == 1);
        assert(!host.logged("kill:barky") && !host.logged("give:Apurrture Cake"));
        assert(host.state.killsTotal == 0 && host.state.getFlag("ruins.barky.state", 0) == 1);
        assert(host.state.getFlag("kills.ruins", 0) == 0 && host.state.getFlag("toxic.track", 0) == 0);
        assert(decideEnding(host.state) == EndingType::CleanRun);

        // taking the test again after a lost fight: no second intro, the choice comes straight back
        const std::size_t firstLines = host.dialogLines.size();
        host.dialogLines.clear();
        host.log.clear();
        host.pick = 1;
        host.state.setFlag("ruins.booth.choice", 0);
        runner.start(loadCutscene("r9a_booth_entry"));
        run(runner, host);
        assert(host.dialogLines.size() < firstLines);
        assert(!host.said("Welcome to the Apurrture Science Enrichment Center fan booth. :3"));
        assert(host.logged("battle:gladowos"));
        assert(host.state.getFlag("ruins.booth.refused", 0) == 2);
        assert(host.state.getFlag("ruins.booth.state", 0) == 1);
    }

    // --- the loose panel: THE HUG IS A LIE, first time and when read again
    {
        RecordingHost host;
        CutsceneRunner runner;
        runner.start(loadCutscene("r9a_panel"));
        run(runner, host);
        assert(host.said("* THE HUG IS A LIE"));
        assert(host.state.getFlag("ruins.booth.panel", 0) == 1);
        const std::size_t first = host.dialogLines.size();
        runner.start(loadCutscene("r9a_panel"));
        run(runner, host);
        assert(host.dialogLines.size() > first && host.dialogLines.back() == "* THE HUG IS A LIE");    }

    // --- the rooms: R9 doors and the booth (only reachable if Barky was spared; powered down if he is dead)
    {
        TiledMap barky;
        TiledMap booth;
        assert(loadTiledMapFromFile("data/maps/ruins_barky.json", barky));
        assert(loadTiledMapFromFile("data/maps/ruins_booth.json", booth));
        assert(booth.findLayer("ground") != nullptr && booth.findLayer("collision") != nullptr);
        assert(booth.width == 16 && booth.height == 12);

        const TiledObject* toBooth = barky.findObject("to_booth");
        const TiledObject* toBoothAgain = barky.findObject("to_booth_again");
        const TiledObject* toBarky = booth.findObject("to_barky");
        assert(toBooth != nullptr && toBoothAgain != nullptr && toBarky != nullptr);
        assert(tiledObjectType(*toBooth) == "door" && tiledObjectType(*toBoothAgain) == "door");
        assert(tiledStringProperty(toBooth->properties, "targetRoom") == "ruins_booth");
        assert(tiledStringProperty(toBoothAgain->properties, "targetRoom") == "ruins_booth");
        assert(tiledStringProperty(toBarky->properties, "targetRoom") == "ruins_barky");
        // every door lands on a real spawn
        assert(booth.findObject(tiledStringProperty(toBooth->properties, "targetSpawn")) != nullptr);
        assert(booth.findObject(tiledStringProperty(toBoothAgain->properties, "targetSpawn")) != nullptr);
        assert(barky.findObject(tiledStringProperty(toBarky->properties, "targetSpawn")) != nullptr);
        assert(booth.findObject("spawn") != nullptr);
        // the doors sit on the floor of the room, not in a wall
        assert(toBooth->x >= 40.0f && toBooth->y >= 80.0f);

        const Flags unresolved = { { "ruins.barky.state", 0 } };
        const Flags spared = { { "ruins.barky.state", 1 } };
        const Flags killedInFight = { { "ruins.barky.state", 2 } };
        const Flags visited = { { "ruins.barky.state", 1 }, { "ruins.booth.state", 1 } };
        const Flags sacrificed = { { "ruins.barky.state", 2 }, { "ruins.booth.state", 2 } };

        assert(!visible(*toBooth, unresolved) && !visible(*toBoothAgain, unresolved));
        assert(visible(*toBooth, spared) && !visible(*toBoothAgain, spared));
        assert(!visible(*toBooth, killedInFight) && !visible(*toBoothAgain, killedInFight));
        assert(!visible(*toBooth, visited) && visible(*toBoothAgain, visited));
        assert(!visible(*toBooth, sacrificed) && visible(*toBoothAgain, sacrificed));

        // the test starts by walking up to the host, once, only for a spared Barky
        const TiledObject* trigger = booth.findObject("trigger_booth_test");
        assert(trigger != nullptr && tiledStringProperty(trigger->properties, "cutscene") == "r9a_booth_entry");
        assert(visible(*trigger, spared) && !visible(*trigger, visited) && !visible(*trigger, sacrificed));
        assert(!visible(*trigger, killedInFight));

        // host: active for a living Barky, powered down for a dead one
        const TiledObject* host = booth.findObject("npc_glad");
        const TiledObject* hostOff = booth.findObject("npc_glad_off");
        assert(host != nullptr && hostOff != nullptr);
        assert(visible(*host, visited) && !visible(*hostOff, visited));
        assert(!visible(*host, sacrificed) && visible(*hostOff, sacrificed));
        assert(tiledStringProperty(hostOff->properties, "lines") == "* There's nobody left to test.");
        assert(tiledStringProperty(host->properties, "cutscene") == "r9a_booth_entry");

        // Barky in the booth vanishes with the sacrifice; the dust appears
        const TiledObject* barkyNpc = booth.findObject("npc_barky_booth");
        const TiledObject* dust = booth.findObject("booth_dust");
        assert(barkyNpc != nullptr && dust != nullptr);
        assert(visible(*barkyNpc, visited) && !visible(*barkyNpc, sacrificed));
        assert(!visible(*dust, visited) && visible(*dust, sacrificed));

        // save point at the entrance (south half, near the door), examinables with their LORE text
        const TiledObject* save = booth.findObject("savepoint");
        assert(save != nullptr && tiledObjectType(*save) == "savepoint");
        assert(save->y > 240.0f && save->y < toBarky->y);
        const TiledObject* cube = booth.findObject("cube");
        const TiledObject* sign = booth.findObject("booth_sign");
        const TiledObject* panel = booth.findObject("panel");
        assert(cube != nullptr && sign != nullptr && panel != nullptr);
        assert(tiledStringProperty(cube->properties, "lines") ==
               "* A cube with a paw print on it. Someone has been hugging it.");
        assert(tiledStringProperty(sign->properties, "lines") ==
               "* \"APURRTURE SCIENCE. Testing tomorrow's fandom, today.\"||* Someone added \":3\".");
        assert(tiledStringProperty(panel->properties, "cutscene") == "r9a_panel");

        // every cutscene the booth refers to exists and drives only NPCs of the booth
        std::vector<std::string> ids = { "r9a_sacrifice", "r9a_refuse" };
        for (const TiledLayer& layer : booth.layers) {
            for (const TiledObject& obj : layer.objects) {
                const std::string id = tiledStringProperty(obj.properties, "cutscene");
                if (!id.empty()) ids.push_back(id);
            }
        }
        for (const std::string& id : ids) {
            const Cutscene cutscene = loadCutscene(id);
            for (const CutsceneStep& step : cutscene.steps) {
                if (!step.npc.empty()) assert(booth.findObject(step.npc) != nullptr);
            }
        }

        // the builder owns the map: the new room is generated, not hand-written
        assert(contains(readFile("tools/maps/build_ruins_maps.py"), "\"ruins_booth.json\": build_ruins_booth"));
    }

    std::cout << "[PASS] C1-014: choice step (2-3 options, 1-based flag, branches), cutscene/recordKill steps, "
                 "R9a booth rooms and doors, Sacrifice (kill + cake) and Refuse (gladowos), Apurrture Cake."
              << std::endl;
}
