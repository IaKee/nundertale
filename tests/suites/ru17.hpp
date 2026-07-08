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

namespace ru17_detail {

inline std::string slurp(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    assert(file.good() && "File must exist and be readable");
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

inline nundertale::Cutscene loadCutscene(const std::string& cutsceneId) {
    nundertale::Cutscene cutscene;
    const std::string json = slurp("data/cutscenes/" + cutsceneId + ".json");
    const bool parsed = nundertale::parseCutsceneJson(json, cutscene);
    assert(parsed && "Cutscene JSON must parse successfully");
    assert(!cutscene.steps.empty() && "Cutscene must contain steps");
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

inline bool containsNoBlockedTerms(const std::string& text) {
    if (text.find("my child") != std::string::npos) return false;
    if (text.find("My child") != std::string::npos) return false;
    if (text.find("Pepe") != std::string::npos) return false;
    return true;
}

} // namespace ru17_detail

inline void suite_ru17() {
    using namespace nundertale;
    using namespace ru17_detail;

    // 1. Map check: ruins_house wires Furiell NPC to r11_talk
    TiledMap mapHouse;
    const bool loadedHouse = loadTiledMapFromFile("data/maps/ruins_house.json", mapHouse);
    assert(loadedHouse && "Failed to load data/maps/ruins_house.json");

    const TiledObject* furiellNpc = nullptr;
    for (const auto& layer : mapHouse.layers) {
        for (const auto& obj : layer.objects) {
            if (obj.name == "furiell" && tiledStringProperty(obj.properties, "cutscene") == "r11_talk") {
                furiellNpc = &obj;
                break;
            }
        }
        if (furiellNpc != nullptr) break;
    }
    assert(furiellNpc != nullptr && "furiell NPC with r11_talk must exist in ruins_house");
    assert(tiledObjectType(*furiellNpc) == "npc");
    assert(tiledStringProperty(furiellNpc->properties, "cutscene") == "r11_talk");

    // The showIf condition must check the basement-send flag and avoid conflicts with r12.walk
    const std::string houseShowIf = tiledStringProperty(furiellNpc->properties, "showIf");
    assert(houseShowIf.find("ruins.house.talk_home == 0") != std::string::npos);
    assert(houseShowIf.find("ruins.r12.walk == 0") != std::string::npos);
    assert(houseShowIf.find("ruins.furiell.killed == 0") != std::string::npos);

    // Door to basement exists and connects to ruins_basement
    const TiledObject* doorBasement = mapHouse.findObject("door_to_basement");
    assert(doorBasement != nullptr && "door_to_basement must exist in ruins_house");
    assert(tiledStringProperty(doorBasement->properties, "targetRoom") == "ruins_basement");
    assert(tiledStringProperty(doorBasement->properties, "targetSpawn") == "from_house");

    // 2. Main talk cutscene: r11_talk.json
    const Cutscene csTalk = loadCutscene("r11_talk");
    const std::string rawTalk = slurp("data/cutscenes/r11_talk.json");
    assert(containsNoBlockedTerms(rawTalk));

    // Find the Choice step
    const CutsceneStep* choiceStep = nullptr;
    for (const auto& step : csTalk.steps) {
        if (step.type == CutsceneStepType::Choice) {
            choiceStep = &step;
            break;
        }
    }
    assert(choiceStep != nullptr && "r11_talk must contain a Choice step");
    assert(cutsceneChoiceValid(*choiceStep) && "Choice step must satisfy CUTSCENE_CHOICE rules");
    assert(choiceStep->options.size() == 3U && "Must offer 3 dialogue choices");
    assert(choiceStep->options[0] == "When can I go home?");
    assert(choiceStep->options[1] == "Talk about games");
    assert(choiceStep->options[2] == "Ask about the fur");
    assert(choiceStep->key == "ruins.house.talk_choice");

    // Must transition to each branch via 'cutscene' step with appropriate 'if' condition
    bool hasHomeBranch = false;
    bool hasGamesBranch = false;
    bool hasFurBranch = false;

    for (const auto& step : csTalk.steps) {
        if (step.type == CutsceneStepType::PlayCutscene) {
            if (step.key == "r11_talk_home" && step.condition.find("talk_choice == 1") != std::string::npos) {
                hasHomeBranch = true;
            }
            if (step.key == "r11_talk_games" && step.condition.find("talk_choice == 2") != std::string::npos) {
                hasGamesBranch = true;
            }
            if (step.key == "r11_talk_fur" && step.condition.find("talk_choice == 3") != std::string::npos) {
                hasFurBranch = true;
            }
        }
    }
    assert(hasHomeBranch && "r11_talk must lead to r11_talk_home for choice 1");
    assert(hasGamesBranch && "r11_talk must lead to r11_talk_games for choice 2");
    assert(hasFurBranch && "r11_talk must lead to r11_talk_fur for choice 3");

    // 3. Branch: r11_talk_home.json ("When can I go home?")
    const Cutscene csHome = loadCutscene("r11_talk_home");
    const std::string rawHome = slurp("data/cutscenes/r11_talk_home.json");
    assert(containsNoBlockedTerms(rawHome));

    // Voice checks (v3.1: pup/hun, uwu, >w<)
    assert(rawHome.find("pup") != std::string::npos || rawHome.find("hun") != std::string::npos);
    assert(rawHome.find("uwu") != std::string::npos || rawHome.find(">w<") != std::string::npos);

    // FUR variant checks: has both low and high FUR dialog conditions
    bool homeHasLowFur = false;
    bool homeHasHighFur = false;
    for (const auto& step : csHome.steps) {
        if (step.type == CutsceneStepType::Dialog) {
            if (step.condition.find("fur <") != std::string::npos) homeHasLowFur = true;
            if (step.condition.find("fur >=") != std::string::npos) homeHasHighFur = true;
        }
    }
    assert(homeHasLowFur && "r11_talk_home must have low FUR variant");
    assert(homeHasHighFur && "r11_talk_home must have high FUR variant (cozier)");

    // High FUR line is cozier (acknowledges player's fluff/shedding)
    assert(hasLineContaining(csHome, "fluff") || hasLineContaining(csHome, "cozy"));

    // Home branch sets the flag sending her to the basement
    assert(setsFlag(csHome, "ruins.house.talk_home"));

    // Moves Furiell towards the stairs before disappearing
    bool homeMovesFuriell = false;
    bool homeHidesFuriell = false;
    for (const auto& step : csHome.steps) {
        if (step.type == CutsceneStepType::MoveNpc && step.npc == "furiell") {
            homeMovesFuriell = true;
            assert(step.y <= 60.0f && "Must move towards the stairs at north wall");
        }
        if (step.type == CutsceneStepType::HideNpc && step.npc == "furiell") {
            homeHidesFuriell = true;
        }
    }
    assert(homeMovesFuriell && "Furiell must walk toward basement stairs");
    assert(homeHidesFuriell && "Furiell must be hidden upon reaching stairs");

    // 4. Branch: r11_talk_games.json ("Talk about games")
    const Cutscene csGames = loadCutscene("r11_talk_games");
    const std::string rawGames = slurp("data/cutscenes/r11_talk_games.json");
    assert(containsNoBlockedTerms(rawGames));
    assert(rawGames.find("pup") != std::string::npos || rawGames.find("hun") != std::string::npos);
    assert(rawGames.find("uwu") != std::string::npos || rawGames.find(">w<") != std::string::npos);

    bool gamesHasLowFur = false;
    bool gamesHasHighFur = false;
    for (const auto& step : csGames.steps) {
        if (step.type == CutsceneStepType::Dialog) {
            if (step.condition.find("fur <") != std::string::npos) gamesHasLowFur = true;
            if (step.condition.find("fur >=") != std::string::npos) gamesHasHighFur = true;
        }
    }
    assert(gamesHasLowFur && "r11_talk_games must have low FUR variant");
    assert(gamesHasHighFur && "r11_talk_games must have high FUR variant (cozier)");
    assert(hasLineContaining(csGames, "Fort-night") || hasLineContaining(csGames, "victory royale"));

    // 5. Branch: r11_talk_fur.json ("Ask about the fur")
    const Cutscene csFur = loadCutscene("r11_talk_fur");
    const std::string rawFur = slurp("data/cutscenes/r11_talk_fur.json");
    assert(containsNoBlockedTerms(rawFur));
    assert(rawFur.find("pup") != std::string::npos || rawFur.find("hun") != std::string::npos);
    assert(rawFur.find("uwu") != std::string::npos || rawFur.find(">w<") != std::string::npos);

    bool furHasLowFur = false;
    bool furHasHighFur = false;
    for (const auto& step : csFur.steps) {
        if (step.type == CutsceneStepType::Dialog) {
            if (step.condition.find("fur <") != std::string::npos) furHasLowFur = true;
            if (step.condition.find("fur >=") != std::string::npos) furHasHighFur = true;
        }
    }
    assert(furHasLowFur && "r11_talk_fur must have low FUR variant");
    assert(furHasHighFur && "r11_talk_fur must have high FUR variant (cozier)");
    assert(hasLineContaining(csFur, "zipper") || hasLineContaining(csFur, "cringe"));
    assert(hasLineContaining(csFur, "fluff") || hasLineContaining(csFur, "cuddly"));

    std::cout << "[PASS] NAR-003 (ru17): R11 living-room conversation with choices and FUR variants verified." << std::endl;
}
