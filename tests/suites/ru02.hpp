#pragma once

// T01 / BUG-40 + RAD-003: Grub lies on the plush pile from the first frame (a spawn property, not a cutscene
// step), the squad message comes while he is still lying, then he gets up, then the thought line; the squad
// clock starts at the message and not before.

#include "core/Cutscene.hpp"
#include "core/RaidClock.hpp"
#include "core/RoomObjects.hpp"
#include "core/TiledMap.hpp"

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace ru02_detail {

inline std::string slurp(const std::string& path) {
    std::ifstream file(path);
    assert(file && "file must exist");
    std::ostringstream text;
    text << file.rdbuf();
    return text.str();
}

inline bool dialogHas(const nundertale::CutsceneStep& step, const std::string& needle) {
    if (step.type != nundertale::CutsceneStepType::Dialog) return false;
    for (const std::string& line : step.lines) {
        if (line.find(needle) != std::string::npos) return true;
    }
    return false;
}

} // namespace ru02_detail

inline void suite_ru02() {
    using namespace nundertale;
    using namespace ru02_detail;

    // --- 1. the spawn carries the pose; it is applied from the spawn, with the position ---------------
    TiledMap map;
    const bool loaded = loadTiledMapFromFile("data/maps/ruins_fall.json", map);
    assert(loaded);
    const TiledObject* spawn = findRoomSpawn(map, "spawn");
    assert(spawn != nullptr);
    const SpawnPose pose = spawnPoseOf(*spawn);
    assert(pose.anim == "lying");  // keyed on the frame name; the sheet is just data
    assert(pose.sheet == "assets/sprites/ow/pepe_wakeup.png");
    assert(pose.untilFlag == "ruins.r1.woke");
    assert(spawnPoseApplies(pose, 0));   // new game: lying before the first update and draw
    assert(!spawnPoseApplies(pose, 1));  // already woke: coming back never lies him down again
    assert(!spawnPoseApplies(SpawnPose{}, 0));
    assert(!spawnPoseApplies(SpawnPose{ "lying", "", "" }, 0));  // no sheet, no pose
    assert(spawnPoseApplies(SpawnPose{ "lying", "sheet.png", "" }, 5));  // no gate flag: always
    const TiledObject* back = findRoomSpawn(map, "from_floofy");
    assert(back != nullptr);
    assert(spawnPoseOf(*back).anim.empty());  // other spawns stay standing

    // the sheet really has both frames the pose and the get-up use
    const std::string sheet = slurp("assets/sprites/ow/pepe_wakeup.json");
    assert(sheet.find("\"lying\"") != std::string::npos);
    assert(sheet.find("\"get_up\"") != std::string::npos);

    // the spawn sits inside the wake trigger, so r1_wake starts on the first update
    const TiledObject* trigger = map.findObject("trigger_wake");
    assert(trigger != nullptr);
    assert(tiledStringProperty(trigger->properties, "cutscene") == "r1_wake");
    const TiledRect box = trigger->bounds();
    assert(spawn->x >= box.x && spawn->x <= box.x + box.width);
    assert(spawn->y >= box.y && spawn->y <= box.y + box.height);

    // --- 2. cutscene order: (nothing standing) -> buzz + squad message -> get up -> thought ------------
    Cutscene wake;
    const bool parsed = parseCutsceneJson(slurp("data/cutscenes/r1_wake.json"), wake);
    assert(parsed);
    int firstFlag = -1;
    int squadDialog = -1;
    int getUp = -1;
    int clear = -1;
    int thought = -1;
    int buzz = -1;
    for (std::size_t index = 0; index < wake.steps.size(); ++index) {
        const CutsceneStep& step = wake.steps[index];
        const int at = static_cast<int>(index);
        // the lying pose is the spawn's job: no step may (re)set it, and nothing may precede it
        assert(!(step.type == CutsceneStepType::PlayerAnim && step.key == "lying"));
        if (step.type == CutsceneStepType::Sound && step.path == "assets/sfx/phone_buzz.ogg" && buzz < 0) buzz = at;
        if (step.type == CutsceneStepType::SetFlag && step.key == "ruins.r1.squad_msg" && firstFlag < 0) {
            assert(step.value == 1);
            firstFlag = at;
        }
        if (dialogHas(step, "SQUAD QUEUES IN 30:00. get on or we queue without u. and bring the dew -xX_Moderator_Xx")) {
            squadDialog = at;
        }
        if (step.type == CutsceneStepType::PlayerAnim && step.key == "get_up") getUp = at;
        if (step.type == CutsceneStepType::PlayerAnim && step.key.empty() && step.path.empty()) clear = at;
        if (dialogHas(step, "* (Ugh... where's my wireless mouse? And why does it smell like body spray and wet dog?)")) {
            thought = at;
        }
    }
    assert(buzz >= 0 && firstFlag >= 0 && squadDialog >= 0 && getUp >= 0 && clear >= 0 && thought >= 0);
    assert(wake.steps[0].type != CutsceneStepType::PlayerAnim);       // first step is not a pose
    assert(buzz < squadDialog && firstFlag <= squadDialog);            // buzz + clock flag, then the text
    assert(squadDialog < getUp);                                       // the message comes while lying
    assert(getUp < clear && clear < thought);                          // get up, back to normal, then think
    assert(wake.steps[static_cast<std::size_t>(getUp)].seconds > 0.0f);
    assert(!cutsceneStartsBattle(wake));

    // --- 3. the squad clock: not running before the message flag, running after ------------------------
    {
        RaidClock clock;
        int shown = 0;
        // entering R1 on a new game does not start it
        assert(!raidShouldStartOnEnter(clock, "ruins_fall", 0));
        // ...entering any later room (old saves, --room launches) still does
        assert(raidShouldStartOnEnter(clock, "ruins_entrance", 0));
        // ticking before the message changes nothing and shows no label
        raidTick(clock, 5.0f, false);
        assert(!clock.started && clock.timeLeft == 0.0f && raidLabel(clock).empty());
        assert(!raidStartFromSquadMessage(clock, 0, 1320.0f, shown));
        assert(!clock.started && shown == 0);

        // the flag (set by r1_wake at the buzz) starts it, once, and marks message 0 as shown
        assert(raidStartFromSquadMessage(clock, 1, 1320.0f, shown));
        assert(clock.started && clock.timeLeft == 1320.0f);
        assert((shown & 1) == 1);
        assert(raidPendingMessage(1320.0f, 1319.0f, shown) == -1);  // no duplicate "RAID STARTS" popup
        assert(!raidStartFromSquadMessage(clock, 1, 1320.0f, shown));
        raidTick(clock, 10.0f, false);
        assert(clock.timeLeft == 1310.0f);
        assert(!raidShouldStartOnEnter(clock, "ruins_fall", 1));  // already running: never restarted

        // a save made after the message (flag set, clock saved) reloads running; flag without clock restarts
        RaidClock fresh;
        assert(raidShouldStartOnEnter(fresh, "ruins_fall", 1));
    }

    std::cout << "[PASS] BUG-40/RAD-003: spawn pose lying from frame 0, squad message while lying, get-up, "
                 "thought, clock starts at the message." << std::endl;
}
