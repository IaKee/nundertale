#pragma once

#include "core/Cutscene.hpp"
#include "core/RoomObjects.hpp"
#include "core/TiledMap.hpp"

#include <cassert>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace ru06_detail {

inline std::string readFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    assert(file && "file must exist");
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

struct MockCutsceneHost final : nundertale::CutsceneHost {
    std::map<std::string, int> flags;
    std::vector<std::string> hiddenNpcs;
    std::vector<std::string> dialogSpeakers;
    std::vector<std::vector<std::string>> dialogTexts;
    std::string lastMovedNpc;
    float lastMovedX = 0.0f;
    float lastMovedY = 0.0f;
    int moveCalls = 0;

    void dialog(const std::string& speaker, const std::vector<std::string>& lines) override {
        dialogSpeakers.push_back(speaker);
        dialogTexts.push_back(lines);
    }
    bool dialogOpen() const override { return false; }
    void sound(const std::string&, float) override {}
    void setFlag(const std::string& key, int value) override {
        flags[key] = value;
    }
    void shake(float, float) override {}
    void flash(int, int, int, float) override {}
    void startBattle(const std::string&) override {}
    void setFur(int) override {}
    void healFull() override {}
    void facePlayer(const std::string&) override {}
    void playerAnim(const std::string&, const std::string&, float) override {}

    void setNpcVisible(const std::string& npc, bool visible) override {
        if (!visible) {
            hiddenNpcs.push_back(npc);
        }
    }
    bool moveNpc(const std::string& npc, float x, float y, float, float) override {
        lastMovedNpc = npc;
        lastMovedX = x;
        lastMovedY = y;
        ++moveCalls;
        return true;
    }
};

} // namespace ru06_detail

inline void suite_ru06() {
    using namespace nundertale;
    using namespace ru06_detail;

    struct ReactionCase {
        std::string id;
        int expectedResult;
    };

    const std::vector<ReactionCase> cases = {
        { "r5_dummy_hug", 5 },
        { "r5_dummy_talk", 1 },
        { "r5_dummy_fight", 2 },
        { "r5_dummy_other", 3 },
        { "r5_dummy_spare", 4 }
    };

    for (const auto& testCase : cases) {
        const std::string path = "data/cutscenes/" + testCase.id + ".json";
        const std::string jsonText = readFile(path);
        assert(!jsonText.empty());

        // NAR-035 content checks: no "my child", no "Pepe"
        assert(jsonText.find("my child") == std::string::npos);
        assert(jsonText.find("My child") == std::string::npos);
        assert(jsonText.find("Pepe") == std::string::npos);

        Cutscene cutscene;
        const bool parsed = parseCutsceneJson(jsonText, cutscene);
        assert(parsed);
        assert(cutscene.steps.size() >= 4U);

        // Verify dummy result flag
        bool foundResultFlag = false;
        for (const CutsceneStep& step : cutscene.steps) {
            if (step.type == CutsceneStepType::SetFlag && step.key == "ruins.dummy.result") {
                assert(step.value == testCase.expectedResult);
                foundResultFlag = true;
            }
        }
        assert(foundResultFlag);

        // Verify Furiell dialog with uwu voice and pup/hun
        bool foundVoicedDialog = false;
        for (const CutsceneStep& step : cutscene.steps) {
            if (step.type == CutsceneStepType::Dialog && step.speaker == "FURIELL") {
                for (const std::string& line : step.lines) {
                    if ((line.find("pup") != std::string::npos || line.find("hun") != std::string::npos) &&
                        line.find("uwu") != std::string::npos) {
                        foundVoicedDialog = true;
                    }
                }
            }
        }
        assert(foundVoicedDialog);

        // Verify the cutscene ends with moveNpc -> hideNpc -> setFlag
        const std::size_t n = cutscene.steps.size();
        const CutsceneStep& stepMove = cutscene.steps[n - 3U];
        const CutsceneStep& stepHide = cutscene.steps[n - 2U];
        const CutsceneStep& stepFlag = cutscene.steps[n - 1U];

        // 1. moveNpc to north door
        assert(stepMove.type == CutsceneStepType::MoveNpc);
        assert(stepMove.npc == "furiell");
        // Door is at x: [280, 360], y: [0, 80]
        assert(stepMove.x >= 280.0f && stepMove.x <= 360.0f);
        assert(stepMove.y >= 0.0f && stepMove.y <= 80.0f);
        assert(stepMove.speed > 0.0f);

        // 2. hideNpc
        assert(stepHide.type == CutsceneStepType::HideNpc);
        assert(stepHide.npc == "furiell");

        // 3. setFlag ruins.r5.walked 1
        assert(stepFlag.type == CutsceneStepType::SetFlag);
        assert(stepFlag.key == "ruins.r5.walked");
        assert(stepFlag.value == 1);

        // Test runner execution
        CutsceneRunner runner;
        MockCutsceneHost host;
        runner.start(cutscene);
        for (int tick = 0; tick < 20 && runner.running(); ++tick) {
            runner.update(1.0f / 30.0f, host);
        }
        assert(!runner.running());
        assert(host.flags["ruins.r5.walked"] == 1);
        assert(host.flags["ruins.dummy.result"] == testCase.expectedResult);
        assert(host.lastMovedNpc == "furiell");
        assert(host.lastMovedX >= 280.0f && host.lastMovedX <= 360.0f);
        assert(host.lastMovedY >= 0.0f && host.lastMovedY <= 80.0f);
        assert(!host.hiddenNpcs.empty() && host.hiddenNpcs.back() == "furiell");
    }

    // Verify map ruins_dummy: furiell npc has hideIf ruins.r5.walked >= 1
    TiledMap map;
    const bool mapLoaded = loadTiledMapFromFile("data/maps/ruins_dummy.json", map);
    assert(mapLoaded);

    const TiledObject* furiellNpc = nullptr;
    const TiledObject* doorNorth = nullptr;
    for (const TiledLayer& layer : map.layers) {
        if (layer.type != TiledLayerType::Objects) continue;
        for (const TiledObject& obj : layer.objects) {
            if (obj.name == "furiell" && tiledObjectType(obj) == "npc") {
                furiellNpc = &obj;
            }
            if (obj.name == "door_to_cables" && tiledObjectType(obj) == "door") {
                doorNorth = &obj;
            }
        }
    }
    assert(furiellNpc != nullptr);
    assert(doorNorth != nullptr);

    const std::string hideIf = tiledStringProperty(furiellNpc->properties, "hideIf");
    assert(hideIf == "ruins.r5.walked >= 1");

    // Door geometry check
    const TiledRect doorBounds = doorNorth->bounds();
    assert(doorBounds.x == 280.0f && doorBounds.y == 40.0f);
    assert(doorBounds.width == 80.0f && doorBounds.height == 40.0f);

    // Verify condition evaluation for re-entry semantics
    bool condParsed = false;
    auto makeLookup = [](int walkedFlag) {
        return [walkedFlag](const std::string& key) -> int {
            if (key == "ruins.r5.walked") return walkedFlag;
            return 0;
        };
    };

    // Before walking (flag = 0): condition is false, Furiell is NOT hidden
    assert(!evaluateRoomCondition(hideIf, makeLookup(0), &condParsed));
    assert(condParsed);

    // After walking (flag = 1): condition is true, Furiell IS hidden on re-entry
    assert(evaluateRoomCondition(hideIf, makeLookup(1), &condParsed));
    assert(condParsed);

    // Flag > 1: still hidden
    assert(evaluateRoomCondition(hideIf, makeLookup(2), &condParsed));
    assert(condParsed);

    // Negative flag: not hidden
    assert(!evaluateRoomCondition(hideIf, makeLookup(-1), &condParsed));
    assert(condParsed);

    std::cout << "[PASS] C1-005: Furiell walks to north door, hides, and stays hidden via ruins.r5.walked."
              << std::endl;
}
