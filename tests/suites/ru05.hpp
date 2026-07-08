#pragma once

#include <cassert>
#include <iostream>
#include <limits>
#include <string>

#include "combat/ActUses.hpp"
#include "combat/Enemy.hpp"

inline void suite_ru05() {
    using namespace nundertale;

    assert(actUseIndex(-1, 3U) == 0U);
    assert(actUseIndex(0, 3U) == 0U);
    assert(actUseIndex(1, 3U) == 0U);
    assert(actUseIndex(2, 3U) == 1U);
    assert(actUseIndex(3, 3U) == 2U);
    assert(actUseIndex(4, 3U) == 2U);
    assert(actUseIndex(std::numeric_limits<int>::max(), 3U) == 2U);
    assert(actUseIndex(1, 0U) == 0U);

    const auto findAct = [](const Enemy& enemy, const std::string& id) -> const ActDef* {
        for (const ActDef& act : enemy.getActs()) {
            if (act.id == id) return &act;
        }
        return nullptr;
    };

    const auto dummy = Enemy::loadFromJson("data/enemies/fursuit_dummy.json");
    assert(dummy != nullptr);
    const ActDef* shake = findAct(*dummy, "shake");
    assert(shake != nullptr);
    assert(shake->useTexts.size() == 3U);
    assert(shake->useResponses.size() == 3U);
    assert(actTextForUse(*shake, 1) == "* You bob your head to the bass. The dummy sways along.");
    assert(actTextForUse(*shake, 2) == "* You shake harder. The coat rack rattles in rhythm.");
    assert(actTextForUse(*shake, 3) == "* You go all in. Furiell joins in, bouncing on her paws.");
    assert(actTextForUse(*shake, 4) == actTextForUse(*shake, 3));
    assert(actResponseForUse(*shake, 1) == "* thump... thump...");
    assert(actResponseForUse(*shake, 2) == "* THUMP. THUMP.");
    assert(actResponseForUse(*shake, 3) == "uwu~ pump the bass, pup!");
    assert(actResponseForUse(*shake, 4) == actResponseForUse(*shake, 3));

    const auto napstablurk = Enemy::loadFromJson("data/enemies/napstablurk.json");
    assert(napstablurk != nullptr);
    const ActDef* cheer = findAct(*napstablurk, "cheer");
    assert(cheer != nullptr);
    assert(actTextForUse(*cheer, 1) == "* Like their post.");
    assert(actTextForUse(*cheer, 2) == "* Leave a comment.");
    assert(actTextForUse(*cheer, 3) == "* Share.");
    assert(actTextForUse(*cheer, 4) == "* Share.");
    assert(actResponseForUse(*cheer, 1) == "oh... a like...?");
    assert(actResponseForUse(*cheer, 2) == "a comment... thank you...");
    assert(actResponseForUse(*cheer, 3) == "you shared it...?");
    assert(actResponseForUse(*cheer, 4) == "you shared it...?");

    std::cout << "[PASS] C1-005: exact per-use ACT text/response selection and clamping verified."
              << std::endl;
}
