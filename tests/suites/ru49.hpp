#pragma once

#include "core/Cutscene.hpp"
#include "core/ItemDatabase.hpp"
#include "core/RoomObjects.hpp"
#include "core/TiledMap.hpp"

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace ru49_detail {

inline std::string readFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    assert(file && "C1-015 data file must exist");
    std::ostringstream text;
    text << file.rdbuf();
    return text.str();
}

inline nundertale::Cutscene loadCutscene(const std::string& id) {
    nundertale::Cutscene scene;
    assert(nundertale::parseCutsceneJson(readFile("data/cutscenes/" + id + ".json"), scene));
    assert(!scene.steps.empty());
    return scene;
}

} // namespace ru49_detail

inline void suite_ru49() {
    using namespace nundertale;
    using namespace ru49_detail;

    TiledMap r1;
    TiledMap r1a;
    assert(loadTiledMapFromFile("data/maps/ruins_fall.json", r1));
    assert(loadTiledMapFromFile("data/maps/ruins_registration.json", r1a));

    const TiledObject* farDoor = r1a.findObject("door_to_panelhall");
    const TiledObject* locked = r1a.findObject("turnstile_locked");
    const TiledObject* opened = r1a.findObject("turnstile_open");
    const TiledObject* badgeBox = r1a.findObject("badge_box");
    const TiledObject* invalidBadgeBox = r1a.findObject("badge_box_invalid");
    assert(farDoor != nullptr && locked != nullptr && opened != nullptr);
    assert(badgeBox != nullptr && invalidBadgeBox != nullptr);
    assert(tiledStringProperty(farDoor->properties, "showIf") == "ruins.r1a.turnstile_open >= 1");
    assert(tiledObjectType(*locked) == "prop");
    assert(tiledStringProperty(locked->properties, "sprite") == "assets/props/ruins_reg_turnstile.png");
    assert(tiledStringProperty(locked->properties, "anim") == "locked");
    assert(tiledBoolProperty(locked->properties, "solid", false));
    assert(tiledStringProperty(locked->properties, "hideIf") == "ruins.r1a.turnstile_open >= 1");
    assert(tiledStringProperty(locked->properties, "cutscene") == "r1a_turnstile");
    assert(tiledStringProperty(locked->properties, "bumpCutscene") == "r1a_turnstile");
    assert(tiledStringProperty(opened->properties, "anim") == "open");
    assert(!tiledBoolProperty(opened->properties, "solid", true));
    assert(tiledStringProperty(opened->properties, "showIf") == "ruins.r1a.turnstile_open >= 1");
    assert(tiledStringProperty(badgeBox->properties, "hideIf") == "ruins.r1a.turnstile_tried >= 1");
    assert(tiledStringProperty(invalidBadgeBox->properties, "showIf") ==
           "ruins.r1a.turnstile_tried >= 1");
    assert(tiledStringProperty(invalidBadgeBox->properties, "lines") ==
           "* \"INVALID: EVENT ENDED\"");

    const TiledObject* floofy = r1.findObject("floofy");
    const TiledObject* bed = r1.findObject("flower_bed");
    const TiledObject* under = r1.findObject("badge_under_floofy");
    const TiledObject* badge = r1.findObject("floofy_badge");
    assert(floofy != nullptr && bed != nullptr && under != nullptr && badge != nullptr);
    assert(tiledStringProperty(floofy->properties, "hideIf") == "ruins.floofy.left_landing >= 1");
    assert(tiledStringProperty(bed->properties, "altAnimIf") == "ruins.floofy.left_landing >= 1");
    assert(tiledStringProperty(under->properties, "hideIf") == "ruins.r1a.turnstile_tried >= 1");
    assert(tiledStringProperty(under->properties, "cutscene") == "r1_badge");
    assert(tiledStringProperty(badge->properties, "sprite") == "assets/props/floofy_badge.png");
    assert(tiledStringProperty(badge->properties, "showIf") ==
           "ruins.r1a.turnstile_tried >= 1 && item.floofy_badge.taken == 0");
    assert(tiledStringProperty(badge->properties, "cutscene") == "r1_badge");

    const Cutscene turnstile = loadCutscene("r1a_turnstile");
    const Cutscene badgeScene = loadCutscene("r1_badge");
    bool setsTried = false;
    bool setsOpen = false;
    bool setsLeft = false;
    bool givesBadge = false;
    for (const CutsceneStep& step : turnstile.steps) {
        assert(step.type != CutsceneStepType::Unknown && step.type != CutsceneStepType::Unsupported);
        setsTried = setsTried || (step.type == CutsceneStepType::SetFlag &&
                                  step.key == "ruins.r1a.turnstile_tried");
        setsOpen = setsOpen || (step.type == CutsceneStepType::SetFlag &&
                                step.key == "ruins.r1a.turnstile_open");
        setsLeft = setsLeft || (step.type == CutsceneStepType::SetFlag &&
                                step.key == "ruins.floofy.left_landing");
    }
    for (const CutsceneStep& step : badgeScene.steps) {
        assert(step.type != CutsceneStepType::Unknown && step.type != CutsceneStepType::Unsupported);
        givesBadge = givesBadge || (step.type == CutsceneStepType::GiveItem &&
                                    step.item == "Floofy's Badge");
    }
    assert(setsTried && setsOpen && setsLeft && givesBadge);

    const std::string turnstileText = readFile("data/cutscenes/r1a_turnstile.json");
    const std::string badgeText = readFile("data/cutscenes/r1_badge.json");
    assert(turnstileText.find("BADGE REQUIRED. NO BADGE, NO CON.") != std::string::npos);
    assert(turnstileText.find("WELCOME, FLOOFY!") != std::string::npos);
    assert(turnstileText.find("muffled thud and a squeaky pawstep") != std::string::npos);
    assert(badgeText.find("There's a badge under him. You're not reaching under him.") != std::string::npos);
    assert(badgeText.find("FLOOFY — STAFF — WELCOME COMMITTEE") != std::string::npos);

    ItemDatabase items;
    assert(items.parse(readFile("data/items/items.json")));
    const ItemDef* badgeItem = items.findItemByName("Floofy's Badge");
    assert(badgeItem != nullptr);
    assert(badgeItem->id == "floofy_badge");
    assert(badgeItem->type == "key");
    assert(badgeItem->noDrop);
    assert(badgeItem->info.find("FLOOFY — STAFF — WELCOME COMMITTEE") != std::string::npos);

    std::cout << "[PASS] C1-015: badge detour, persistent turnstile, and empty Landing return." << std::endl;
}
