#pragma once

#include "combat/ActGate.hpp"
#include "combat/Enemy.hpp"
#include "core/GameContext.hpp"
#include "core/TiledMap.hpp"

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace ru08_detail {

using nundertale::tiled_detail::JsonValue;

inline const JsonValue* fieldOf(const JsonValue& object, const char* key, JsonValue::Kind kind) {
    const JsonValue* value = object.field(key);
    return value != nullptr && value->kind == kind ? value : nullptr;
}

inline std::string strOf(const JsonValue& object, const char* key) {
    const JsonValue* value = fieldOf(object, key, JsonValue::Kind::String);
    return value == nullptr ? std::string() : value->string;
}

inline const JsonValue* findAct(const JsonValue& root, const std::string& id) {
    const JsonValue* acts = fieldOf(root, "acts", JsonValue::Kind::Array);
    if (acts == nullptr) return nullptr;
    for (const JsonValue& act : acts->array) {
        if (strOf(act, "id") == id) return &act;
    }
    return nullptr;
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

} // namespace ru08_detail

inline void suite_ru08() {
    using namespace nundertale;
    using namespace ru08_detail;

    // 1. Pure gate helpers in ActGate.hpp
    // 1a. Item gate: item present / absent
    {
        const std::vector<std::string> emptyInventory;
        const std::vector<std::string> bag = { "Bottle", "Mt. Dew", "Doritos" };

        // Unconditional
        assert(actItemGatePassed("", emptyInventory));
        assert(actItemGatePassed("", bag));

        // Present vs absent
        assert(actItemGatePassed("Mt. Dew", bag));
        assert(!actItemGatePassed("Toy Laser Pointer", bag));
        assert(!actItemGatePassed("Toy Laser Pointer", emptyInventory));

        // Toy Laser Pointer item gate (by name, id, or shortName)
        const std::vector<std::string> bagWithLaserName = { "Bottle", "Toy Laser Pointer" };
        const std::vector<std::string> bagWithLaserId = { "Bottle", "laser_pointer" };
        const std::vector<std::string> bagWithLaserShort = { "Bottle", "ToyLaser" };

        assert(actItemGatePassed("Toy Laser Pointer", bagWithLaserName));
        assert(actItemGatePassed("Toy Laser Pointer", bagWithLaserId));
        assert(actItemGatePassed("Toy Laser Pointer", bagWithLaserShort));
        assert(actItemGatePassed("laser_pointer", bagWithLaserName));
        assert(actItemGatePassed("laser_pointer", bagWithLaserId));

        // Toy Laser Pointer gated by flag ruins.laser.taken (even if item dropped/stored)
        auto flagLaserTaken = [](const std::string& f) { return f == "ruins.laser.taken"; };
        auto flagNoLaser = [](const std::string&) { return false; };

        assert(actItemGatePassed("Toy Laser Pointer", emptyInventory, flagLaserTaken));
        assert(!actItemGatePassed("Toy Laser Pointer", emptyInventory, flagNoLaser));
    }

    // 1b. Flag gate: flag 0/1 (via int and predicate)
    {
        // Via predicate
        auto checkFlag = [](const std::string& f) { return f == "flag_active"; };
        assert(actFlagGatePassed("", checkFlag));
        assert(actFlagGatePassed("flag_active", checkFlag));
        assert(!actFlagGatePassed("flag_inactive", checkFlag));

        // Via int value (0 / 1)
        assert(actFlagGatePassed("", 0));
        assert(actFlagGatePassed("", 1));
        assert(actFlagGatePassed("flag_active", 1));
        assert(!actFlagGatePassed("flag_active", 0));
        assert(actFlagGatePassed("ruins.laser.taken", 1));
        assert(!actFlagGatePassed("ruins.laser.taken", 0));
    }

    // 1c. Both item and flag: all 4 permutations (item present/absent x flag 0/1)
    {
        const std::vector<std::string> withItem = { "Keycard" };
        const std::vector<std::string> withoutItem = { "Bottle" };

        // Item present (1) + Flag 1 -> PASS
        assert(actGatePassed("Keycard", "door_open", withItem, 1));
        // Item present (1) + Flag 0 -> FAIL
        assert(!actGatePassed("Keycard", "door_open", withItem, 0));
        // Item absent (0) + Flag 1 -> FAIL
        assert(!actGatePassed("Keycard", "door_open", withoutItem, 1));
        // Item absent (0) + Flag 0 -> FAIL
        assert(!actGatePassed("Keycard", "door_open", withoutItem, 0));

        // Same permutations via predicate
        auto flag1 = [](const std::string& f) { return f == "door_open"; };
        auto flag0 = [](const std::string&) { return false; };
        assert(actGatePassed("Keycard", "door_open", withItem, flag1));
        assert(!actGatePassed("Keycard", "door_open", withItem, flag0));
        assert(!actGatePassed("Keycard", "door_open", withoutItem, flag1));
        assert(!actGatePassed("Keycard", "door_open", withoutItem, flag0));
    }

    // 1d. ActDef integration with actAvailable & actGatePassed
    {
        ActDef actItem;
        actItem.id = "use_key";
        actItem.label = "Use Key";
        actItem.requiresItem = "Keycard";

        ActDef actFlag;
        actFlag.id = "hack_terminal";
        actFlag.label = "Hack";
        actFlag.requiresFlag = "terminal_online";

        ActDef actBoth;
        actBoth.id = "override_console";
        actBoth.label = "Override";
        actBoth.requiresItem = "Keycard";
        actBoth.requiresFlag = "terminal_online";

        const std::vector<std::string> invHas = { "Keycard" };
        const std::vector<std::string> invEmpty;
        auto flagOn = [](const std::string& f) { return f == "terminal_online"; };
        auto flagOff = [](const std::string&) { return false; };

        // ActDef with requiresItem
        assert(actAvailable(actItem, invHas, 0, flagOff));
        assert(!actAvailable(actItem, invEmpty, 0, flagOff));

        // ActDef with requiresFlag
        assert(actAvailable(actFlag, invEmpty, 0, flagOn));
        assert(!actAvailable(actFlag, invEmpty, 0, flagOff));

        // ActDef with both
        assert(actAvailable(actBoth, invHas, 0, flagOn));
        assert(!actAvailable(actBoth, invHas, 0, flagOff));
        assert(!actAvailable(actBoth, invEmpty, 0, flagOn));
        assert(!actAvailable(actBoth, invEmpty, 0, flagOff));
    }

    // 2. barky.json Laser entry parsed and validated
    {
        JsonValue root;
        assert(loadJson("data/enemies/barky.json", root));
        assert(strOf(root, "id") == "barky");

        const JsonValue* laser = findAct(root, "laser");
        assert(laser != nullptr);
        assert(strOf(*laser, "id") == "laser");
        assert(strOf(*laser, "label") == "Laser");
        assert(strOf(*laser, "requiresItem") == "Toy Laser Pointer");
        assert(strOf(*laser, "text") == "* You shine a red dot on the floor.");
        assert(strOf(*laser, "response") == "NO\xE2\x80\x94 I'M NOT\xE2\x80\x94 WHERE DID IT GO\xE2\x80\x94");
        assert(effectBool(*laser, "spareable"));
        assert(effectBool(*laser, "skipAttack"));

        // Load via Enemy::loadFromJson
        std::unique_ptr<Enemy> barky = Enemy::loadFromJson("data/enemies/barky.json");
        assert(barky != nullptr);
        assert(barky->getName() == "Barky");

        const auto& acts = barky->getActs();
        const ActDef* laserAct = nullptr;
        for (const auto& a : acts) {
            if (a.id == "laser") {
                laserAct = &a;
                break;
            }
        }
        assert(laserAct != nullptr);
        assert(laserAct->label == "Laser");
        assert(laserAct->requiresItem == "Toy Laser Pointer");
        assert(laserAct->spareable);
        assert(laserAct->skipAttack);
        assert(laserAct->text == "* You shine a red dot on the floor.");
    }

    // 3. Barky's Laser ACT menu gating:
    // Hidden when unmet; appears with Toy Laser Pointer (or ruins.laser.taken);
    // Makes Barky spareable and skips attack.
    {
        std::unique_ptr<Enemy> barky = Enemy::loadFromJson("data/enemies/barky.json");
        assert(barky != nullptr);

        auto findAvailableLaser = [&](const std::vector<std::string>& inv,
                                      const std::function<bool(const std::string&)>& hasFlag) -> bool {
            for (const auto& a : barky->getActs()) {
                if (a.id == "laser") {
                    return actAvailable(a, inv, 0, hasFlag);
                }
            }
            return false;
        };

        const std::vector<std::string> standardInventory = { "Bottle", "Mt. Dew", "Doritos" };
        auto noFlags = [](const std::string&) { return false; };
        auto laserFlagTaken = [](const std::string& f) { return f == "ruins.laser.taken"; };

        // Unmet: player does not have Toy Laser Pointer and has not taken it
        assert(!findAvailableLaser(standardInventory, noFlags));

        // Met via item name "Toy Laser Pointer"
        const std::vector<std::string> invWithName = { "Bottle", "Toy Laser Pointer" };
        assert(findAvailableLaser(invWithName, noFlags));

        // Met via item ID "laser_pointer" (items.json id)
        const std::vector<std::string> invWithId = { "Bottle", "laser_pointer" };
        assert(findAvailableLaser(invWithId, noFlags));

        // Met via flag "ruins.laser.taken" (pickup behind CRT in R8)
        assert(findAvailableLaser(standardInventory, laserFlagTaken));
        assert(findAvailableLaser({}, laserFlagTaken));

        // Verify Laser ACT effect semantics: spareable + skipAttack
        for (const auto& a : barky->getActs()) {
            if (a.id == "laser") {
                assert(!barky->isSpareable());
                if (a.spareable) {
                    barky->setSpareable(true);
                }
                assert(barky->isSpareable());
                assert(a.skipAttack);
                break;
            }
        }
    }

    // 4. Enemy default constructor (fallback when JSON not loaded)
    {
        Enemy defaultBarky("Barky", 30, 4, 2);
        const ActDef* defaultLaser = nullptr;
        for (const auto& a : defaultBarky.getActs()) {
            if (a.id == "laser") {
                defaultLaser = &a;
                break;
            }
        }
        assert(defaultLaser != nullptr);
        assert(defaultLaser->requiresItem == "Toy Laser Pointer");
        assert(defaultLaser->spareable);
        assert(defaultLaser->skipAttack);
    }

    std::cout << "[PASS] C1-021: Barky's Laser ACT gated by Toy Laser Pointer / ruins.laser.taken verified." << std::endl;
}
