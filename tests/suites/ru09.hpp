#pragma once

#include "combat/Enemy.hpp"
#include "core/PeacePath.hpp"

#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace ru09_detail {

// Plays the ACT labels in order on a fresh Barky; returns the enemy so the caller can look at the state.
inline std::unique_ptr<nundertale::Enemy> play(const std::vector<std::string>& labels, bool toxic = false) {
    auto barky = nundertale::Enemy::loadFromJson("data/enemies/barky.json");
    assert(barky != nullptr && barky->hasPeacePath());
    for (const std::string& label : labels) barky->applyPeaceAct(label, toxic);
    return barky;
}

inline const nundertale::ActDef* menuAct(const nundertale::Enemy& enemy, const std::string& id) {
    for (const nundertale::ActDef& act : enemy.getMenuActs()) {
        if (act.id == id) return &act;
    }
    return nullptr;
}

} // namespace ru09_detail

inline void suite_ru09() {
    using namespace nundertale;
    using namespace ru09_detail;

    // --- data: the block parses, the 6 authored ACTs stay, Comment on Tail joins the menu after Check
    {
        auto barky = Enemy::loadFromJson("data/enemies/barky.json");
        assert(barky->hasPeacePath());
        assert(barky->getPeacePath().goal == 5 && barky->getPeacePath().toxicExtraSteps == 2);
        assert(barky->getPeacePath().rules.size() == 6);
        assert(barky->getActs().size() == 6);
        assert(barky->getMenuActs().size() == 7);
        assert(barky->getMenuActs()[0].id == "check" && barky->getMenuActs()[1].id == "comment_tail");
        const ActDef* tail = menuAct(*barky, "comment_tail");
        assert(tail != nullptr && tail->label == "Comment on Tail");
        assert(tail->useTexts.size() == 4 && tail->useResponses.size() == 4);
        assert(actTextForUse(*tail, 1) == "* You point at the safety pin holding his tail on.");
        assert(actTextForUse(*tail, 1) != actTextForUse(*tail, 2));
        assert(actTextForUse(*tail, 4) != actTextForUse(*tail, 3));
        assert(actResponseForUse(*tail, 1) == "It's... a design choice.");
        assert(actResponseForUse(*tail, 2) == "Stop LOOKING at it.");
        assert(actResponseForUse(*tail, 3) == "It's load-bearing, okay?!");
        assert(actResponseForUse(*tail, 4) == "...fine. It falls off sometimes. Happy?");
        assert(actResponseForUse(*tail, 9) == actResponseForUse(*tail, 4));
        // every rule names a real menu ACT
        for (const PeaceActRule& rule : barky->getPeacePath().rules) {
            bool found = false;
            for (const ActDef& act : barky->getMenuActs()) found = found || act.label == rule.act;
            assert(found);
        }
    }

    // --- 4 steps are not enough, the 5th opens MERCY (any mix)
    {
        auto four = play({ "Comment on Tail", "Comment on Tail", "Comment on Tail", "Comment on Tail" });
        assert(four->getPeaceState().progress == 4 && !four->isSpareable());
        assert(!four->peaceMercyAllowed(false) && four->peaceStepsLeft(false) == 1);

        auto five = play({ "Comment on Tail", "Comment on Tail", "Comment on Tail", "Comment on Tail", "Cringe Check" });
        assert(five->getPeaceState().progress == 5 && five->isSpareable() && five->peaceMercyAllowed(false));

        auto mix = play({ "Cringe Check", "Offer Dew", "Laser", "Accept Hug", "Comment on Tail" });
        assert(mix->isSpareable());

        auto noTail = play({ "Cringe Check", "Offer Dew", "Laser", "Accept Hug" });
        assert(noTail->getPeaceState().progress == 4 && !noTail->isSpareable());
    }

    // --- each counting ACT is +1 and nothing else counts
    {
        for (const char* label : { "Comment on Tail", "Cringe Check", "Offer Dew", "Laser", "Accept Hug" }) {
            auto barky = play({});
            const PeaceStepResult step = barky->applyPeaceAct(label, false);
            assert(step.known && step.progressAdded == 1);
            assert(barky->getPeaceState().progress == 1 && !barky->isSpareable());
        }
        auto lore = play({ "Debate Lore", "Debate Lore", "Check", "Nonsense", "Spare" });
        assert(lore->getPeaceState().progress == 0 && !lore->isSpareable());
        auto skip = play({});
        const PeaceStepResult debate = skip->applyPeaceAct("Debate Lore", false);
        assert(debate.known && debate.progressAdded == 0 && debate.skipsAttack);
        // Hug: +1 step and a hug counted (FUR comes from the ACT's own fur effect)
        auto hug = play({ "Accept Hug", "Accept Hug" });
        assert(hug->getPeaceState().progress == 2 && hug->getPeaceState().hugs == 2 && !hug->isSpareable());
    }

    // --- repeated Comment on Tail counts exactly 4 times
    {
        auto barky = play({});
        for (int i = 1; i <= 8; ++i) {
            const PeaceStepResult step = barky->applyPeaceAct("Comment on Tail", false);
            assert(step.known);
            assert(step.progressAdded == (i <= 4 ? 1 : 0));
        }
        assert(barky->getPeaceState().progress == 4 && !barky->isSpareable());
        barky->applyPeaceAct("Offer Dew", false);
        assert(barky->isSpareable());
        // past the goal nothing moves and nothing un-spares
        const PeaceStepResult extra = barky->applyPeaceAct("Offer Dew", false);
        assert(extra.progressAdded == 0 && !extra.goalReached && barky->isSpareable());
        assert(barky->getPeaceState().progress == 5);
    }

    // --- goalReached fires once, on the 5th step
    {
        auto barky = play({ "Offer Dew", "Offer Dew", "Offer Dew", "Offer Dew" });
        const PeaceStepResult last = barky->applyPeaceAct("Cringe Check", false);
        assert(last.goalReached && last.progressAdded == 1);
        assert(!barky->applyPeaceAct("Cringe Check", false).goalReached);
    }

    // --- toxic track: PeacePath's toxic bonus (+2) moves the goal to 7
    {
        auto barky = play({ "Comment on Tail", "Comment on Tail", "Comment on Tail", "Comment on Tail", "Cringe Check" },
                          true);
        assert(barky->getPeaceState().progress == 5 && !barky->isSpareable());
        assert(!barky->peaceMercyAllowed(true) && barky->peaceStepsLeft(true) == 2);
        barky->applyPeaceAct("Offer Dew", true);
        assert(!barky->isSpareable() && barky->peaceStepsLeft(true) == 1);
        barky->applyPeaceAct("Laser", true);
        assert(barky->isSpareable() && barky->getPeaceState().progress == 7);
        assert(peaceGoal(barky->getPeacePath(), true) == 7 && peaceGoal(barky->getPeacePath(), false) == 5);
    }

    // --- MERCY gate: mercy points and ACT spareable flags no longer open it; Enemy::addMercy is inert
    {
        auto barky = play({});
        barky->addMercy(100);
        assert(!barky->isSpareable() && barky->getMercy() == 0);
        const ActDef* dew = menuAct(*barky, "offer_dew");
        assert(dew != nullptr && dew->spareable);  // authored flag stays in the data, the path ignores it
        assert(!barky->isSpareable());
    }

    // --- flavor text counts down the steps taken
    {
        auto barky = play({ "Cringe Check" });
        assert(barky->getFlavorText("cringe_check") ==
               "* Barky tugs his tail straight. It slides crooked again. Four to go.");
        barky->applyPeaceAct("Offer Dew", false);
        assert(barky->getFlavorText("offer_dew").find("Three to go.") != std::string::npos);
        barky->applyPeaceAct("Offer Dew", false);
        barky->applyPeaceAct("Offer Dew", false);
        assert(barky->getFlavorText("offer_dew").find("One to go.") != std::string::npos);
        barky->applyPeaceAct("Offer Dew", false);
        assert(barky->isSpareable());
        assert(barky->getFlavorText("offer_dew").find("to go") == std::string::npos);  // spareable pool now

        auto toxic = play({ "Cringe Check" }, true);
        assert(toxic->getFlavorText("cringe_check").find("Six to go.") != std::string::npos);

        PeacePathDef bare;
        PeacePathState untouched;
        assert(peaceFlavorLine(bare, untouched, false).empty());
        assert(peaceNumberWord(0) == "No" && peaceNumberWord(13) == "13");
    }

    // --- no peacePath block: the old behaviour (mercy, spareable ACTs, spareNow) is untouched
    {
        auto dummy = Enemy::loadFromJson("data/enemies/fursuit_dummy.json");
        assert(!dummy->hasPeacePath());
        assert(dummy->getMenuActs().size() == dummy->getActs().size());
        assert(dummy->peaceMercyAllowed(false) && dummy->peaceStepsLeft(false) == 0);
        assert(!dummy->applyPeaceAct("Accept Hug", false).known);
        assert(!dummy->isSpareable());
        dummy->addMercy(100);
        assert(dummy->getMercy() == 100 && dummy->isSpareable());

        Enemy plain("Plain", 10, 1, 1);
        assert(!plain.hasPeacePath() && plain.getMenuActs().size() == plain.getActs().size());
        plain.setSpareable(true);
        assert(plain.isSpareable());
    }

    std::cout << "[PASS] ACT-008/C1-021 (ru09): Barky peacePath data, 5 steps, Comment on Tail x4, toxic goal 7, "
                 "MERCY gate and step-count flavor verified." << std::endl;
}
