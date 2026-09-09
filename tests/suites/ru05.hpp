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
    // v3.2: a plain dummy. Talk (spareable) and Accept Hug are the only live ACTs; the old Bass Shake
    // ACTs stay in data only as flag-gated aliases (the inline ACT-009 suite still reads them).
    const ActDef* talk = findAct(*dummy, "talk");
    const ActDef* hug = findAct(*dummy, "accept_hug");
    assert(talk != nullptr && talk->spareable && !talk->stopsBeat);
    assert(hug != nullptr && hug->spareNow && hug->fur == 10);
    for (const char* legacy : { "shake", "turn_down" }) {
        const ActDef* act = findAct(*dummy, legacy);
        assert(act != nullptr);
        assert(!act->requiresFlag.empty());
        assert(!act->spareable && !act->stopsBeat && !act->spareNow);
    }
    assert(dummy->getPatterns().size() == 1U);
    const EnemyPatternDef& tufts = dummy->getPatterns()[0];
    assert(tufts.kind == EnemyPatternKind::Rain);
    assert(tufts.kind != EnemyPatternKind::Beat);
    assert(tufts.damage == 0 && tufts.furry == 2 && tufts.color == AttackColor::Green);
    assert(tufts.speed <= 40.0f);

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
