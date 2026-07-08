#pragma once

#include "combat/Enemy.hpp"

#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

namespace ru13_detail {

inline const nundertale::ActDef* actById(const nundertale::Enemy& enemy, const std::string& id) {
    for (const nundertale::ActDef& act : enemy.getMenuActs()) {
        if (act.id == id) return &act;
    }
    return nullptr;
}

inline std::string readFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    assert(file.good());
    return { std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
}

} // namespace ru13_detail

inline void suite_ru13() {
    using namespace nundertale;
    using namespace ru13_detail;

    auto furiell = Enemy::loadFromJson("data/enemies/furiell.json");
    assert(furiell != nullptr && furiell->getId() == "furiell");
    assert(furiell->hasPeacePath());
    assert(furiell->getPeacePath().goal == 5);
    assert(furiell->getPeacePath().toxicExtraSteps == 3);
    assert(furiell->getPeacePath().rules.size() == 5);
    // Legacy lines remain loadable for the old round-10 test, but BattleSystem ignores them for peace paths.
    assert(furiell->getSpareLines().size() == 13);
    assert(furiell->getSpareFlag() == "ruins.furiell.spared");
    assert(furiell->getKillFlag() == "ruins.furiell.killed");

    const ActDef* promise = actById(*furiell, "promise_visit");
    const ActDef* insult = actById(*furiell, "insult");
    const ActDef* hug = actById(*furiell, "accept_hug");
    assert(promise != nullptr && promise->label == "Promise to Visit");
    assert(insult != nullptr && insult->text == "* Furiell is enjoying this." && insult->skipAttack);
    assert(hug != nullptr && hug->fur == 30 && hug->spareNow);
    assert(hug->setFlag == "ruins.furiell.hugged");

    // Promise to Visit is absent from the available ACT set until two completed steps.
    assert(!furiell->peaceActAvailable("Promise to Visit"));
    PeaceStepResult locked = furiell->applyPeaceAct("Promise to Visit", false);
    assert(!locked.known && locked.progressAdded == 0);
    PeaceStepResult letGo = furiell->applyPeaceAct("Let Go of Her Paw", false);
    assert(letGo.known && letGo.progressAdded == 1);
    assert(!furiell->peaceActAvailable("Promise to Visit"));
    PeaceStepResult games = furiell->applyPeaceAct("Talk About Games", false);
    assert(games.known && games.progressAdded == 1);
    assert(furiell->peaceActAvailable("Promise to Visit"));

    // Insult does not advance the path, but it flusters Furiell and skips the next attack.
    const int beforeInsult = furiell->getPeaceState().progress;
    PeaceStepResult flustered = furiell->applyPeaceAct("Insult", false);
    assert(flustered.known && flustered.progressAdded == 0 && flustered.skipsAttack);
    assert(flustered.line == "...say that again, pup. Slower.");
    assert(furiell->getPeaceState().progress == beforeInsult);

    // A valid five-step sequence opens MERCY; Accept Hug is one step and carries FUR +15.
    PeaceStepResult visit = furiell->applyPeaceAct("Promise to Visit", false);
    assert(visit.known && visit.progressAdded == 1);
    PeaceStepResult accepted = furiell->applyPeaceAct("Accept Hug", false);
    assert(accepted.known && accepted.progressAdded == 1 && accepted.furGained == 15);
    assert(!furiell->isSpareable() && furiell->peaceStepsLeft(false) == 1);
    PeaceStepResult finalStep = furiell->applyPeaceAct("Let Go of Her Paw", false);
    assert(finalStep.goalReached && furiell->isSpareable() && furiell->peaceMercyAllowed(false));

    const std::vector<std::string>& death = furiell->getDeathLines();
    assert(death.size() == 2);
    assert(death[0] == "...hehe... harder... no, wait—");
    assert(death[1] == "...that one... actually hurt, pup...");

    const std::string encounter = readFile("data/encounters/furiell.json");
    assert(encounter.find("You're weawwy leaving? Just like that? ...Fine. Pwove you can survive up there, pup. Hit me. ...Please?") !=
           std::string::npos);

    std::cout << "[PASS] C1-022/ACT-008 (ru13): Furiell five-step peace path, two-step Promise unlock, "
                 "Insult skip, Accept Hug FUR +15, and v3.1 opening/death lines verified." << std::endl;
}
