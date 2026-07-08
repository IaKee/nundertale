#pragma once

#include "combat/EncounterModel.hpp"

#include <cassert>
#include <filesystem>
#include <iostream>

inline void suite_ru40() {
    using namespace nundertale;

    // 1. Every data/encounters file parses, as 1..3 enemies that exist in data/enemies
    {
        std::size_t files = 0;
        for (const auto& entry : std::filesystem::directory_iterator("data/encounters")) {
            if (entry.path().extension() != ".json") continue;
            ++files;
            Encounter enc;
            assert(loadEncounterFile(entry.path().string(), enc));
            assert(!enc.enemies.empty() && enc.enemies.size() <= kMaxEncounterEnemies);
            assert(!enc.id.empty());
            assert(!enc.introText.empty());
            for (const std::string& enemy : enc.enemies) {
                assert(std::filesystem::exists("data/enemies/" + enemy + ".json"));
            }
        }
        assert(files >= 11);

        // Legacy single-enemy shape is read unchanged
        Encounter barky;
        assert(loadEncounterFile("data/encounters/barky.json", barky));
        assert(barky.id == "encounter_barky");
        assert(barky.enemies.size() == 1U && barky.enemies[0] == "barky");
        assert(barky.introText == "* Barky blocked your path!");
        assert(barky.background == "grid_tube");
        assert(barky.music == "boss_barky");
        assert(!barky.canFlee);
        assert(barky.routeOverrides.empty());
        Encounter dummy;
        assert(loadEncounterFile("data/encounters/fursuit_dummy.json", dummy));
        assert(dummy.canFlee && dummy.music == "battle.bass_shake");
        Encounter missing;
        assert(!loadEncounterFile("data/encounters/does_not_exist.json", missing));
    }

    // 2. Parse boundaries
    {
        Encounter enc;
        assert(parseEncounter(R"({"enemies":["a","b","c"]})", enc));
        assert(enc.enemies.size() == 3U && enc.introText.empty() && !enc.canFlee);
        assert(!parseEncounter(R"({"enemies":[]})", enc));
        assert(!parseEncounter(R"({"enemies":["a","b","c","d"]})", enc));
        assert(!parseEncounter(R"({"enemies":"barky"})", enc));
        assert(!parseEncounter(R"({"enemies":[1]})", enc));
        assert(!parseEncounter(R"({"enemies":["../x"]})", enc));
        assert(!parseEncounter(R"({"enemies":[""]})", enc));
        assert(!parseEncounter(R"({"introText":"hi"})", enc));
        assert(!parseEncounter(R"({"enemies":["a"],"canFlee":"yes"})", enc));
        assert(!parseEncounter(R"({"enemies":["a"],"music":5})", enc));
        assert(!parseEncounter(R"([1,2])", enc));
        assert(!parseEncounter("{not json", enc));
        // a failed parse leaves the output untouched
        assert(enc.enemies.size() == 3U);
    }

    // 3. Route overrides
    {
        Encounter enc;
        assert(parseEncounter(R"({
            "id":"pair","enemies":["dogsona","dogsonissa"],"introText":"* Woof.",
            "background":"grid","music":"battle.default","canFlee":true,
            "routeOverrides":{
              "toxic":{"introText":"* They flinch.","canFlee":false,"enemies":["dogsona"]},
              "furry":{"music":"battle.pup"}
            }})", enc));
        assert(enc.routeOverrides.size() == 2U);

        const Encounter gamer = resolveEncounterForRoute(enc, "gamer");
        assert(gamer.enemies.size() == 2U && gamer.introText == "* Woof." && gamer.canFlee);
        assert(gamer.routeOverrides.empty());
        const Encounter toxic = resolveEncounterForRoute(enc, "toxic");
        assert(toxic.enemies.size() == 1U && toxic.introText == "* They flinch." && !toxic.canFlee);
        assert(toxic.background == "grid" && toxic.music == "battle.default");
        const Encounter furry = resolveEncounterForRoute(enc, "furry");
        assert(furry.music == "battle.pup" && furry.canFlee && furry.enemies.size() == 2U);

        assert(!parseEncounter(R"({"enemies":["a"],"routeOverrides":[]})", enc));
        assert(!parseEncounter(R"({"enemies":["a"],"routeOverrides":{"toxic":1}})", enc));
        assert(!parseEncounter(R"({"enemies":["a"],"routeOverrides":{"toxic":{"enemies":[]}}})", enc));
        assert(!parseEncounter(R"({"enemies":["a"],"routeOverrides":{"toxic":{"canFlee":1}}})", enc));
    }

    // 4. "X and co." naming
    {
        assert(encounterGroupName({}).empty());
        assert(encounterGroupName({ "Barky" }) == "Barky");
        assert(encounterGroupName({ "Reply Guy", "Lurkeye" }) == "Reply Guy and co.");
        assert(encounterGroupName({ "Jerry", "Punsona", "Lesser Doge" }) == "Jerry and co.");

        Encounter generated;
        assert(parseEncounter(R"({"enemies":["jerry","punsona"]})", generated));
        assert(encounterIntroText(generated, { "Jerry", "Punsona" }) == "* Jerry and co. blocked your path!");
        assert(encounterIntroText(generated, { "Jerry" }) == "* Jerry blocked your path!");
        assert(encounterIntroText(generated, {}) == "* Something blocked your path!");
        Encounter authored;
        assert(parseEncounter(R"({"enemies":["a","b"],"introText":"* Custom."})", authored));
        assert(encounterIntroText(authored, { "A", "B" }) == "* Custom.");
    }

    // 5. Turn aggregation, victory, alone
    {
        Encounter enc;
        assert(parseEncounter(R"({"enemies":["reply_guy","lurkeye","jerry"],"canFlee":true})", enc));
        EncounterState state(enc, { "Reply Guy", "Lurkeye", "Jerry" });
        assert(state.size() == 3U && state.activeCount() == 3U);
        assert(!state.isVictory());
        assert(state.introText() == "* Reply Guy and co. blocked your path!");
        assert(state.activeGroupName() == "Reply Guy and co.");
        assert(state.enemyId(1) == "lurkeye" && state.name(2) == "Jerry");

        // everyone attacks, in roster order, each with its own pattern
        const std::vector<std::string> patterns = { "well_actually", "seen_rings", "brb" };
        auto attacks = state.aggregateTurn(patterns);
        assert(attacks.size() == 3U);
        assert(attacks[0].enemyIndex == 0U && attacks[0].patternId == "well_actually");
        assert(attacks[2].enemyIndex == 2U && attacks[2].patternId == "brb");
        // skip flags and empty patterns drop attacks; short vectors do not crash
        attacks = state.aggregateTurn(patterns, { false, true });
        assert(attacks.size() == 2U && attacks[1].enemyIndex == 2U);
        attacks = state.aggregateTurn({ "well_actually", "", "brb" });
        assert(attacks.size() == 2U && attacks[1].enemyIndex == 2U);
        assert(state.aggregateTurn({ "well_actually" }).size() == 1U);
        assert(state.aggregateTurn({}).empty());

        assert(!state.isAlone(0) && state.companionCount(0) == 2U);
        assert(!state.isAloneById("reply_guy"));

        // dead / spared / fled enemies stop attacking
        assert(state.kill(1));
        assert(!state.kill(1) && !state.spare(1) && !state.flee(1)); // already resolved
        assert(state.status(1) == EnemyStatus::Dead);
        attacks = state.aggregateTurn(patterns);
        assert(attacks.size() == 2U && attacks[0].enemyIndex == 0U && attacks[1].enemyIndex == 2U);
        assert(state.activeGroupName() == "Reply Guy and co.");
        assert(!state.isVictory());

        assert(state.spare(2));
        assert(state.status(2) == EnemyStatus::Spared);
        attacks = state.aggregateTurn(patterns);
        assert(attacks.size() == 1U && attacks[0].enemyIndex == 0U);
        assert(!state.isVictory());

        // alone condition: Reply Guy is the last active one
        assert(state.isAlone(0) && state.isAloneById("reply_guy"));
        assert(!state.isAloneById("lurkeye") && !state.isAlone(1) && !state.isAlone(2));
        assert(state.companionCount(0) == 0U);
        assert(state.activeGroupName() == "Reply Guy");

        // victory once the last one is spared; nobody attacks
        assert(state.spare(0));
        assert(state.isVictory() && state.activeCount() == 0U);
        assert(state.aggregateTurn(patterns).empty());
        assert(!state.isAlone(0) && state.activeGroupName().empty());
        assert(state.anyKilled() && state.countWith(EnemyStatus::Spared) == 2U);

        // out of range is harmless
        assert(!state.kill(9) && !state.isActive(9) && state.status(9) == EnemyStatus::Dead);
    }

    // 6. Flee rules and Jerry's Ditch
    {
        Encounter canFlee;
        assert(parseEncounter(R"({"enemies":["a","b"],"canFlee":true})", canFlee));
        EncounterState fleeing(canFlee, { "A", "B" });
        assert(fleeing.spare(0));
        assert(fleeing.fleeAll());
        assert(fleeing.status(0) == EnemyStatus::Spared && fleeing.status(1) == EnemyStatus::Fled);
        assert(fleeing.isVictory() && !fleeing.anyKilled());
        assert(!fleeing.fleeAll()); // nobody left

        Encounter boss;
        assert(parseEncounter(R"({"enemies":["a","b"],"canFlee":false})", boss));
        EncounterState stuck(boss, { "A", "B" });
        assert(!stuck.fleeAll() && stuck.activeCount() == 2U);

        // one of the group runs away on its own: victory needs the rest
        assert(stuck.flee(0));
        assert(!stuck.isVictory() && stuck.isAlone(1));
        assert(stuck.kill(1) && stuck.isVictory());

        Encounter jerryFight;
        assert(parseEncounter(R"({"enemies":["jerry","punsona","lesser_doge"]})", jerryFight));
        EncounterState ditch(jerryFight, { "Jerry", "Punsona", "Lesser Doge" });
        assert(!ditch.companionsSpareable());
        assert(ditch.ditch(0));
        assert(ditch.status(0) == EnemyStatus::Fled && ditch.companionsSpareable());
        assert(ditch.activeCount() == 2U && !ditch.isVictory());
        assert(!ditch.ditch(0)); // already gone

        // an empty state is never a victory
        EncounterState empty;
        assert(empty.size() == 0U && !empty.isVictory() && empty.activeGroupName().empty());
    }

    // 7. Names list shorter than the roster is padded, not UB
    {
        Encounter enc;
        assert(parseEncounter(R"({"enemies":["a","b"]})", enc));
        EncounterState state(enc, { "A" });
        assert(state.name(1).empty() && state.activeGroupName() == "A and co.");
    }

    std::cout << "[PASS] BFL-004: multi-enemy Encounter model parses all encounter files, aggregates turns, "
                 "resolves victory/flee/alone and \"X and co.\" naming." << std::endl;
}
