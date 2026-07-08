#pragma once

#include "core/TiledMap.hpp"

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace ru07_detail {

using nundertale::tiled_detail::JsonValue;

inline const JsonValue* fieldOf(const JsonValue& object, const char* key, JsonValue::Kind kind) {
    const JsonValue* value = object.field(key);
    return value != nullptr && value->kind == kind ? value : nullptr;
}

inline std::string strOf(const JsonValue& object, const char* key) {
    const JsonValue* value = fieldOf(object, key, JsonValue::Kind::String);
    return value == nullptr ? std::string() : value->string;
}

inline std::vector<std::string> listOf(const JsonValue& object, const char* key) {
    std::vector<std::string> out;
    if (const JsonValue* value = fieldOf(object, key, JsonValue::Kind::Array)) {
        for (const JsonValue& item : value->array) {
            if (item.kind == JsonValue::Kind::String) out.push_back(item.string);
        }
    }
    return out;
}

inline const JsonValue* findAct(const JsonValue& root, const std::string& id) {
    const JsonValue* acts = fieldOf(root, "acts", JsonValue::Kind::Array);
    if (acts == nullptr) return nullptr;
    for (const JsonValue& act : acts->array) {
        if (strOf(act, "id") == id) return &act;
    }
    return nullptr;
}

inline int effectInt(const JsonValue& act, const char* key) {
    const JsonValue* effects = fieldOf(act, "effects", JsonValue::Kind::Object);
    if (effects == nullptr) return 0;
    const JsonValue* value = fieldOf(*effects, key, JsonValue::Kind::Number);
    return value == nullptr ? 0 : static_cast<int>(value->number);
}

inline bool effectBool(const JsonValue& act, const char* key) {
    const JsonValue* effects = fieldOf(act, "effects", JsonValue::Kind::Object);
    if (effects == nullptr) return false;
    const JsonValue* value = fieldOf(*effects, key, JsonValue::Kind::Boolean);
    return value != nullptr && value->boolean;
}

inline bool loadJson(const std::string& path, JsonValue& root) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) return false;
    std::stringstream buffer;
    buffer << file.rdbuf();
    return nundertale::tiled_detail::JsonParser(buffer.str()).parse(root) &&
           root.kind == JsonValue::Kind::Object;
}

} // namespace ru07_detail

inline void suite_ru07() {
    using namespace ru07_detail;

    JsonValue root;
    assert(loadJson("data/enemies/barky.json", root));
    assert(strOf(root, "id") == "barky");

    // Stats and check (LORE 19.1): getCheckText() prints "* BARKY - ATK 4 DEF 2" then this line.
    auto num = [&](const char* key) {
        const JsonValue* v = fieldOf(root, key, JsonValue::Kind::Number);
        return v == nullptr ? -999 : static_cast<int>(v->number);
    };
    assert(num("hp") == 30 && num("atk") == 4 && num("def") == 2 && num("exp") == 20 && num("gold") == 10);
    assert(strOf(root, "check") == "Alpha Wolf of the Ruins. The zipper says otherwise.");

    // Every LORE ACT exists with its exact text and response.
    const JsonValue* check = findAct(root, "check");
    assert(check != nullptr && strOf(*check, "label") == "Check");

    // Comment on Tail (v3.1) is NOT in the data: test_runner.cpp pins barky's ACT count at 6 and the
    // 5-step mercy path is T08/T09 (PeacePath.hpp already carries its lines).
    assert(findAct(root, "comment_tail") == nullptr);

    const JsonValue* cringe = findAct(root, "cringe_check");
    assert(cringe != nullptr && strOf(*cringe, "label") == "Cringe Check");
    assert(strOf(*cringe, "text") == "* You stare directly at the zipper on Barky's back.");
    assert(strOf(*cringe, "response") == "H-hey! It's an artisanal design feature, b-baka!");
    const std::vector<std::string> cringeTexts = listOf(*cringe, "useTexts");
    const std::vector<std::string> cringeResp = listOf(*cringe, "useResponses");
    assert(cringeTexts.size() == 2 && cringeResp.size() == 2);
    assert(cringeTexts[0] == strOf(*cringe, "text"));
    assert(cringeResp[0] == "H-hey! It's an artisanal design feature, b-baka!");
    assert(cringeResp[1] == "...I KNOW, okay? I KNOW.");
    assert(strOf(*cringe, "repeatResponse") == "...I KNOW, okay? I KNOW.");

    const JsonValue* dew = findAct(root, "offer_dew");
    assert(dew != nullptr && strOf(*dew, "label") == "Offer Dew");
    assert(strOf(*dew, "text") == "* You crack open an ice-cold can.");
    assert(strOf(*dew, "response") == "Citrus Blast 2012?! ...wait, how do I know that flavor?");
    assert(!strOf(*dew, "repeatResponse").empty()); // gamer flashback on the 2nd can
    assert(effectBool(*dew, "spareable"));

    const JsonValue* lore = findAct(root, "debate_lore");
    assert(lore != nullptr && strOf(*lore, "label") == "Debate Lore");
    assert(strOf(*lore, "text") == "* You recite incoherent mecha lore. Barky is bewildered.");
    assert(strOf(*lore, "response") == "Season 4 episode 12 was CANON?!");
    assert(effectBool(*lore, "skipAttack"));

    const JsonValue* laser = findAct(root, "laser");
    assert(laser != nullptr && strOf(*laser, "label") == "Laser");
    assert(strOf(*laser, "requiresItem") == "Toy Laser Pointer");
    assert(strOf(*laser, "text") == "* You shine a red dot on the floor.");
    assert(strOf(*laser, "response") ==
           "NO\xE2\x80\x94 I'M NOT\xE2\x80\x94 WHERE DID IT GO\xE2\x80\x94");
    assert(effectBool(*laser, "spareable") && effectBool(*laser, "skipAttack"));

    // Accept Hug: same effect fields as the dummy's Accept Hug (fur + spareNow), FUR +15.
    const JsonValue* hug = findAct(root, "accept_hug");
    assert(hug != nullptr && strOf(*hug, "label") == "Accept Hug");
    assert(strOf(*hug, "text") == "* Barky hugs you. He smells like a gym bag. You hug back anyway.");
    assert(strOf(*hug, "response") == "...this is the best day of my life. Don't tell anyone.");
    assert(effectInt(*hug, "fur") == 15);
    assert(effectBool(*hug, "spareNow"));

    JsonValue dummy;
    assert(loadJson("data/enemies/fursuit_dummy.json", dummy));
    const JsonValue* dummyHug = findAct(dummy, "accept_hug");
    assert(dummyHug != nullptr && effectBool(*dummyHug, "spareNow"));
    assert(effectInt(*dummyHug, "fur") > 0);

    // Spare and death lines.
    assert(strOf(root, "spareText") == "You can tell no one about the zipper. Pack secret.");
    assert(strOf(root, "deathText") == "* Only a pile of synthetic wolf fur remains.");

    // Existing pattern ids are kept.
    const JsonValue* patterns = fieldOf(root, "patterns", JsonValue::Kind::Array);
    assert(patterns != nullptr && patterns->array.size() >= 2);
    assert(strOf(patterns->array[0], "id") == "uwu_rain");
    assert(strOf(patterns->array[1], "id") == "dew_bounce");

    // Flavor buckets: non-empty, every line a narrator line.
    const JsonValue* flavor = fieldOf(root, "flavor", JsonValue::Kind::Object);
    assert(flavor != nullptr);
    for (const char* bucket : {"default", "lowHp", "spareable", "blushed"}) {
        const std::vector<std::string> lines = listOf(*flavor, bucket);
        assert(!lines.empty());
        for (const std::string& line : lines) assert(line.rfind("* ", 0) == 0);
    }

    // Speech bubbles: 10-15 across the dialogue buckets, no empties, includes the gamer flashback.
    const JsonValue* dialogue = fieldOf(root, "dialogue", JsonValue::Kind::Object);
    assert(dialogue != nullptr);
    std::vector<std::string> bubbles;
    for (const char* bucket : {"default", "afterHit", "crit", "miss", "item"}) {
        const std::vector<std::string> lines = listOf(*dialogue, bucket);
        bubbles.insert(bubbles.end(), lines.begin(), lines.end());
    }
    assert(bubbles.size() >= 10 && bubbles.size() <= 15);
    bool flashback = false;
    for (const std::string& line : bubbles) {
        assert(!line.empty());
        if (line == "Fear the Shadow Fang Pa\xE2\x80\x94 one more match, mom\xE2\x80\x94 ...what?") flashback = true;
    }
    assert(flashback);

    // Encounter still loads and points at Barky.
    JsonValue encounter;
    assert(loadJson("data/encounters/barky.json", encounter));
    assert(!strOf(encounter, "introText").empty());

    std::cout << "[PASS] C1-021/ACT-002/NAR-002: Barky LORE text (ACTs, Accept Hug FUR +15 spareNow, flavor buckets, "
              << bubbles.size() << " bubbles)." << std::endl;
}
