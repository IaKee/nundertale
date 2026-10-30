#pragma once

#include "combat/BeatMap.hpp"
#include "combat/Enemy.hpp"
#include "combat/FloofyScript.hpp"
#include "core/RoomObjects.hpp"
#include "core/SaveText.hpp"
#include "core/TiledMap.hpp"

#include <cassert>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace ru54_detail {

inline std::string readJsonFile(const std::string& path) {
    std::ifstream stream(path);
    assert(stream.is_open());
    std::ostringstream ss;
    ss << stream.rdbuf();
    return ss.str();
}

} // namespace ru54_detail

// C1-028 v3.5: R12a VIP Lounge + Floofy Lair fight suite
inline void suite_ru54() {
    using namespace nundertale;

    // -------------------------------------------------------------------------
    // 1. Map chain: R12 (basement) -> R12a (viplounge) -> R13 (exit) and back
    // -------------------------------------------------------------------------
    TiledMap basement, viplounge, exit;
    assert(loadTiledMapFromFile("data/maps/ruins_basement.json", basement));
    assert(loadTiledMapFromFile("data/maps/ruins_viplounge.json", viplounge));
    assert(loadTiledMapFromFile("data/maps/ruins_exit.json", exit));

    assert(viplounge.width == 16 && viplounge.height == 18);
    assert(viplounge.pixelWidth() == 640 && viplounge.pixelHeight() == 720);

    // Spawns in viplounge
    const TiledObject* spawnBasement = findRoomSpawn(viplounge, "from_basement");
    assert(spawnBasement != nullptr);
    assert(spawnBasement->x >= 0.0f && spawnBasement->x <= viplounge.pixelWidth());
    assert(spawnBasement->y >= 0.0f && spawnBasement->y <= viplounge.pixelHeight());

    const TiledObject* spawnExit = findRoomSpawn(viplounge, "from_exit");
    assert(spawnExit != nullptr);
    assert(spawnExit->x >= 0.0f && spawnExit->x <= viplounge.pixelWidth());
    assert(spawnExit->y >= 0.0f && spawnExit->y <= viplounge.pixelHeight());

    const TiledObject* spawnDefault = findRoomSpawn(viplounge, "spawn");
    assert(spawnDefault != nullptr);

    // Spawns in basement and exit for viplounge returns
    const TiledObject* bFromVip = findRoomSpawn(basement, "from_viplounge");
    assert(bFromVip != nullptr);
    assert(bFromVip->x >= 0.0f && bFromVip->x <= basement.pixelWidth());

    const TiledObject* eFromVip = findRoomSpawn(exit, "from_viplounge");
    assert(eFromVip != nullptr);
    assert(eFromVip->x >= 0.0f && eFromVip->x <= exit.pixelWidth());

    // Save point in viplounge
    const TiledObject* vipSave = findRoomObject(viplounge, "savepoint");
    assert(vipSave != nullptr);
    assert(std::abs(vipSave->x - 200.0f) < 0.01f);
    assert(std::abs(vipSave->y - 580.0f) < 0.01f);
    const std::string saveLine = tiledStringProperty(vipSave->properties, "lines");
    assert(saveLine == "* A bass line thumps through the wall. It fills you with DETERMINATION. Against your will.");
    assert(savePlaceLine("ruins_viplounge") == "* A bass line thumps through the wall. It fills you with DETERMINATION. Against your will.");

    // Lair props exist and are wired
    assert(findRoomObject(viplounge, "prop", "prop_headrack") != nullptr);
    assert(findRoomObject(viplounge, "prop", "prop_rope") != nullptr);
    assert(findRoomObject(viplounge, "prop", "prop_beanbag") != nullptr);
    assert(findRoomObject(viplounge, "prop", "prop_throne") != nullptr);
    assert(findRoomObject(viplounge, "prop", "flowerwall_left") != nullptr);
    assert(findRoomObject(viplounge, "prop", "flowerwall_right") != nullptr);
    assert(findRoomObject(viplounge, "prop", "prop_lights_left") != nullptr);

    // Examine objects exist
    assert(findRoomObject(viplounge, "examine", "examine_headrack") != nullptr);
    assert(findRoomObject(viplounge, "examine", "examine_rope") != nullptr);
    assert(findRoomObject(viplounge, "examine", "examine_beanbag") != nullptr);
    assert(findRoomObject(viplounge, "examine", "examine_throne") != nullptr);
    assert(findRoomObject(viplounge, "examine", "examine_flowerwall") != nullptr);
    assert(findRoomObject(viplounge, "examine", "examine_lights") != nullptr);

    // -------------------------------------------------------------------------
    // 2. Encounter & Enemy data: floofy_lair
    // -------------------------------------------------------------------------
    const auto floofy = Enemy::loadFromJson("data/enemies/floofy_lair.json");
    assert(floofy != nullptr);
    assert(floofy->getId() == "floofy_lair");
    assert(floofy->getName() == "Floofy");
    assert(floofy->isInvulnerable());
    assert(!floofy->canBeSpared());
    assert(floofy->getScriptedEndAfterTurns() == 6);
    assert(floofy->getScriptedEnd().endCutscene == "r12a_lair_end");
    assert(floofy->getTurnOffsetFlag() == "ruins.floofy.lair_deaths");
    assert(floofy->getFleeBlockedText() == "* The petals block the exit.");
    assert(floofy->getFurCap() == 99 && floofy->suppressesFurMirror());
    assert(floofy->getInvulnerableBubble() == "Aww~ was that a hug?");

    // ACTs: Check, Plead, Compliment the Crown
    std::vector<std::string> actIds;
    for (const ActDef& act : floofy->getActs()) actIds.push_back(act.id);
    assert(std::find(actIds.begin(), actIds.end(), "check") != actIds.end());
    assert(std::find(actIds.begin(), actIds.end(), "plead") != actIds.end());
    assert(std::find(actIds.begin(), actIds.end(), "compliment_crown") != actIds.end());

    // Turn reduction formula (min 3, from 3rd death one turn fewer)
    assert(effectiveScriptedEndTurn(6, 0) == 6);
    assert(effectiveScriptedEndTurn(6, 1) == 6);
    assert(effectiveScriptedEndTurn(6, 2) == 6);
    assert(effectiveScriptedEndTurn(6, 3) == 5);
    assert(effectiveScriptedEndTurn(6, 4) == 4);
    assert(effectiveScriptedEndTurn(6, 5) == 3);
    assert(effectiveScriptedEndTurn(6, 99) == 3);

    // Encounter JSON
    const std::string encJson = ru54_detail::readJsonFile("data/encounters/floofy_lair.json");
    assert(encJson.find("\"enemies\": [\"floofy_lair\"]") != std::string::npos);
    assert(encJson.find("\"introText\": \"* ...DANCE.\"") != std::string::npos);
    assert(encJson.find("\"background\": \"black\"") != std::string::npos);
    assert(encJson.find("\"music\": \"battle.bass_shake\"") != std::string::npos);
    assert(encJson.find("\"canFlee\": false") != std::string::npos);

    // -------------------------------------------------------------------------
    // 3. BeatMap data: floofy_lair
    // -------------------------------------------------------------------------
    BeatMap map;
    assert(loadBeatMap("data/beatmaps/floofy_lair.json", map));
    assert(map.track == "battle.bass_shake");
    assert(map.notes.size() == 10U && map.shots.size() == 2U && map.passesPerTurn == 3);
    assert(map.expressions.size() == 4U);
    assert(map.closeupSprite == "floofy_close");
    assert(map.bulletSprite == "assets/sprites/bullets/thorn_petal.png");

    // -------------------------------------------------------------------------
    // 4. Sprite sheets: floofy_close (4x3 frames) and floofy_wide (60x80)
    // -------------------------------------------------------------------------
    assert(std::filesystem::exists("assets/sprites/battle/floofy_close.png"));
    assert(std::filesystem::exists("assets/sprites/battle/floofy_close.json"));
    assert(std::filesystem::exists("assets/sprites/battle/floofy_wide.png"));
    assert(std::filesystem::exists("assets/sprites/battle/floofy_wide.json"));

    const std::string closeJsonStr = ru54_detail::readJsonFile("assets/sprites/battle/floofy_close.json");
    tiled_detail::JsonValue closeJson;
    assert(tiled_detail::JsonParser(closeJsonStr).parse(closeJson));
    const auto* closeCell = closeJson.field("cell");
    assert(closeCell != nullptr && closeCell->array.size() == 2U);
    assert(closeCell->array[0].number == 120.0 && closeCell->array[1].number == 120.0);
    const auto* closeAnims = closeJson.field("anims");
    assert(closeAnims != nullptr);
    for (const char* expr : {"smug", "wink", "pout", "smirk"}) {
        const auto* anim = closeAnims->field(expr);
        assert(anim != nullptr);
        const auto* frames = anim->field("frames");
        assert(frames != nullptr && frames->number == 3.0);
    }

    const std::string wideJsonStr = ru54_detail::readJsonFile("assets/sprites/battle/floofy_wide.json");
    tiled_detail::JsonValue wideJson;
    assert(tiled_detail::JsonParser(wideJsonStr).parse(wideJson));
    const auto* wideCell = wideJson.field("cell");
    assert(wideCell != nullptr && wideCell->array.size() == 2U);
    assert(wideCell->array[0].number == 60.0 && wideCell->array[1].number == 80.0);

    std::cout << "[PASS] C1-028/ru54: R12a VIP Lounge map chain, floofy_lair fight data, and sprite frames verified."
              << std::endl;
}
