#pragma once

#include "combat/Enemy.hpp"
#include "core/Cutscene.hpp"

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace ru04_detail {

inline nundertale::Cutscene load(const std::string& id) {
    std::ifstream in("data/cutscenes/" + id + ".json");
    assert(in.good());
    std::stringstream buffer;
    buffer << in.rdbuf();
    nundertale::Cutscene cutscene;
    const bool parsed = nundertale::parseCutsceneJson(buffer.str(), cutscene);
    assert(parsed);
    return cutscene;
}

inline bool clean(const std::string& line) {
    return !line.empty() && line.find("Pepe") == std::string::npos &&
           line.find("my child") == std::string::npos;
}

// Counts Floofy dialog steps (optionally only those gated by `condition`) and checks every line.
inline std::size_t floofyDialogs(const nundertale::Cutscene& cutscene, const std::string& condition,
                                 bool& hasMotto, bool& hasOwO) {
    std::size_t count = 0;
    for (const nundertale::CutsceneStep& step : cutscene.steps) {
        if (step.type != nundertale::CutsceneStepType::Dialog || step.speaker != "FLOOFY") continue;
        if (!condition.empty() && step.condition != condition) continue;
        assert(!step.lines.empty());
        for (const std::string& line : step.lines) {
            assert(clean(line));
            if (line.find("YIFF or be YIFFED") != std::string::npos) hasMotto = true;
            if (line.find("OwO") != std::string::npos) hasOwO = true;
        }
        ++count;
    }
    return count;
}

} // namespace ru04_detail

inline void suite_ru04() {
    using namespace nundertale;
    using namespace ru04_detail;

    // R2: every line (Floofy and Furiell) is non-empty, v3.1 voice, no Pepe / "my child"
    for (const char* id : {"r2_floofy", "r2_floofy_battle", "r2_floofy_rescue"}) {
        const Cutscene cutscene = load(id);
        assert(!cutscene.steps.empty());
        for (const CutsceneStep& step : cutscene.steps) {
            if (step.type != CutsceneStepType::Dialog) continue;
            assert(!step.lines.empty());
            for (const std::string& line : step.lines) {
                assert(clean(line));
                assert(line.find("SHAME or be SHAMED") == std::string::npos);
            }
        }
    }
    {
        const Cutscene r2 = load("r2_floofy");
        bool motto = false, owo = false;
        assert(floofyDialogs(r2, "", motto, owo) == 2U);
        assert(motto && owo);
        bool flag = false;
        for (const CutsceneStep& step : r2.steps) {
            if (step.type == CutsceneStepType::SetFlag && step.key == "ruins.floofy.met") flag = true;
        }
        assert(flag);
        const Cutscene battle = load("r2_floofy_battle");
        owo = false;
        assert(floofyDialogs(battle, "", motto, owo) == 1U && owo);
        assert(cutsceneStartsBattle(battle));
    }

    // R13: all three flag variants kept, each non-empty
    {
        const Cutscene r13 = load("r13_floofy");
        bool motto = false, owo = false;
        assert(floofyDialogs(r13, "ruins.furiell.hugged >= 1", motto, owo) == 1U);
        assert(floofyDialogs(r13, "ruins.furiell.hugged == 0 && ruins.furiell.spared >= 1", motto, owo) == 1U);
        assert(floofyDialogs(r13, "ruins.furiell.killed >= 1", motto, owo) == 1U);
        assert(floofyDialogs(r13, "", motto, owo) == 3U);
        assert(motto && owo);
        bool spared = false;
        for (const CutsceneStep& step : r13.steps) {
            for (const std::string& line : step.lines) {
                if (line == "You spared her? Wow. No shame. ...I respect that.") spared = true;
            }
        }
        assert(spared); // exact LORE §12 R13 string
    }

    // Enemy file: still valid, v3.1 flavor
    {
        const auto floofy = Enemy::loadFromJson("data/enemies/floofy.json");
        assert(floofy != nullptr && floofy->getId() == "floofy");
        std::ifstream in("data/enemies/floofy.json");
        std::stringstream buffer;
        buffer << in.rdbuf();
        assert(buffer.str().find("YIFF or be YIFFED") != std::string::npos);
        assert(buffer.str().find("Pepe") == std::string::npos);
    }

    std::cout << "[PASS] NAR-034: Floofy R2/R13 lines are v3.1 voice, variants kept by flag." << std::endl;
}
