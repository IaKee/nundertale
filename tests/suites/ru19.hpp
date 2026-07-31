#pragma once

#include "combat/BattleIntroScript.hpp"
#include "combat/FurHud.hpp"
#include "core/FloofyAnnoyance.hpp"

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace ru19_detail {

using namespace nundertale;

inline IntroScriptDef loadIntro(const std::string& path) {
    std::ifstream in(path);
    assert(in.good());
    std::stringstream ss;
    ss << in.rdbuf();
    tiled_detail::JsonValue root;
    const bool parsed = tiled_detail::JsonParser(ss.str()).parse(root);
    assert(parsed);
    const tiled_detail::JsonValue* intro = root.field("intro");
    assert(intro != nullptr);
    IntroScriptDef def;
    const bool ok = parseIntroScript(*intro, def);
    assert(ok);
    return def;
}

// Reads every text page and answers prompts until the Petals phase (or the end) is reached.
struct Run {
    BattleIntroScript script;
    IntroFlags flags;
    std::vector<std::string> pages;       // every page shown, in order
    std::vector<IntroSpeaker> speakers;
    std::vector<int> beats;               // beat at every page
    std::vector<IntroEffect> effects;

    explicit Run(const IntroScriptDef& def) : script(def) { script.start(flags); }

    void collect() {
        for (const IntroEffect& e : script.takeEffects()) effects.push_back(e);
    }

    // choice: index to pick at a Choice phase
    void playUntilPetalsOrEnd(int choice) {
        for (int guard = 0; guard < 400; ++guard) {
            collect();
            switch (script.phase()) {
            case IntroPhase::Text: {
                const IntroView v = script.view();
                pages.push_back(v.text);
                speakers.push_back(v.speaker);
                beats.push_back(script.beat());
                script.confirm(flags);
                break;
            }
            case IntroPhase::Choice:
                assert(script.chooseOption(choice, flags));
                break;
            case IntroPhase::Pause:
                script.tick(10.0f, flags);
                break;
            case IntroPhase::Petals:
            case IntroPhase::Finished:
                return;
            }
        }
        assert(false && "intro did not settle");
    }
};

inline bool contains(const std::vector<std::string>& v, const std::string& s) {
    for (const std::string& x : v) {
        if (x == s) return true;
    }
    return false;
}

inline int countEffect(const std::vector<IntroEffect>& e, IntroEffectKind k) {
    int n = 0;
    for (const IntroEffect& x : e) {
        if (x.kind == k) ++n;
    }
    return n;
}

}  // namespace ru19_detail

inline void suite_ru19() {
    using namespace nundertale;
    using namespace ru19_detail;

    const IntroScriptDef petalsDef = loadIntro("data/encounters/floofy.json");
    const IntroScriptDef bassDef = loadIntro("data/encounters/floofy_bass.json");

    // Both encounter files carry the same intro block.
    assert(petalsDef.steps.size() == bassDef.steps.size());
    assert(petalsDef.music == "floofy_lounge");
    assert(petalsDef.skipFlag == "ruins.floofy.deaths");

    // ---- retry skip: ruins.floofy.deaths >= 1 -> the intro does not run
    {
        IntroFlags f;
        assert(introShouldRun(petalsDef, f));
        f["ruins.floofy.deaths"] = 0;
        assert(introShouldRun(petalsDef, f));
        f["ruins.floofy.deaths"] = 1;
        assert(!introShouldRun(petalsDef, f));
        f["ruins.floofy.deaths"] = 3;
        assert(!introShouldRun(bassDef, f));
        assert(!introShouldRun(IntroScriptDef{}, IntroFlags{}));
    }

    // ---- polite run: choice 2 "...Okay.", the player catches the first petal -> petal fight
    {
        Run run(petalsDef);
        run.playUntilPetalsOrEnd(0);
        assert(run.script.phase() == IntroPhase::Petals);
        assert(run.script.beat() == 13 || run.script.beat() == 12);
        assert(run.flags["ruins.floofy.choice2"] == 1);
        assert(run.flags["ruins.floofy.annoyed"] == 0);
        assert(contains(run.pages, "See that heart? That's your SOUL. The very culmination of your being!"));
        assert(contains(run.pages, "That's the spirit!"));
        assert(!contains(run.pages, "You KNOW what these are?"));
        assert(run.speakers.front() == IntroSpeaker::Floofy);

        // the vibrating petals show before the narrator box text
        assert(countEffect(run.effects, IntroEffectKind::ShowPetals) == 1);
        std::size_t vibrating = 0;
        for (std::size_t i = 0; i < run.pages.size(); ++i) {
            if (run.pages[i] == "* The petals are vibrating.") {
                vibrating = i;
                assert(run.speakers[i] == IntroSpeaker::Narrator);
                assert(run.beats[i] == 10);
            }
        }
        assert(vibrating > 0);
        assert(run.pages[vibrating + 1] == "* (Why are they vibrating.)");

        // the first round: tutorial_petals, slow, dodgeable
        const IntroPetalRound r1 = run.script.round();
        assert(r1.kind == IntroRoundKind::Tutorial);
        assert(r1.pattern == "tutorial_petals");
        assert(r1.dodgeable);
        assert(r1.speed > 0.0f);
        assert(run.script.options().empty());

        // a petal touches the soul: ~25% of 20 HP, FUR +20, the bar shows for the first time
        const IntroHitResult hit = run.script.petalsHit(run.flags, 20, 0);
        assert(hit.damage == 5);
        assert(hit.furDelta == 20);
        assert(hit.furBarFirst);
        run.collect();
        assert(countEffect(run.effects, IntroEffectKind::FurBarFirst) == 1);
        assert(countEffect(run.effects, IntroEffectKind::Wilt) == 1);
        assert(run.flags["ruins.floofy.dodges"] == 0);

        const std::size_t before = run.pages.size();
        run.playUntilPetalsOrEnd(0);
        assert(run.script.finished());
        assert(countEffect(run.effects, IntroEffectKind::StopMusic) == 1);

        // twist beats 14-17 in order, then beat 18
        const std::vector<std::string> twist = {
            "You idiot.",
            "You really thought I'd share my petals for FREE?",
            "In this world, it's YIFF or be YIFFED.",
            "Why would ANYONE pass up a chance like this?",
            "A gamer. Fresh. Unfluffed.",
            "You fell on me. You crushed my babies.",
            "And now? You're going to STAY.",
            "Now hold still, darling~ you'd make a FABULOUS fox. Let's get you fluffy!"
        };
        assert(run.pages.size() - before == twist.size());
        for (std::size_t i = 0; i < twist.size(); ++i) assert(run.pages[before + i] == twist[i]);
        assert(run.beats[before] == 14);
        assert(run.beats[before + 1] == 15);
        assert(run.beats[before + 3] == 16);
        assert(run.beats[before + 5] == 17);
        assert(run.script.beat() == 18);

        // variant: annoyed 0 -> petals, locked in ruins.floofy.fight
        assert(run.script.fightVariant() == FLOOFY_FIGHT_PETALS);
        assert(run.flags["ruins.floofy.fight"] == FLOOFY_FIGHT_PETALS);
        assert(introFightEncounter(run.script.fightVariant()) == "floofy");
    }

    // ---- rude choice 2: annoyance +1, the hint lines, ruins.floofy.choice2 = 2
    {
        Run run(petalsDef);
        run.playUntilPetalsOrEnd(1);
        assert(run.flags["ruins.floofy.choice2"] == 2);
        assert(run.flags["ruins.floofy.annoyed"] == 1);
        const std::size_t at = [&] {
            for (std::size_t i = 0; i < run.pages.size(); ++i) {
                if (run.pages[i] == "You KNOW what these are?") return i;
            }
            return run.pages.size();
        }();
        assert(at < run.pages.size());
        assert(run.pages[at - 1] == "...");
        assert(run.pages[at + 1] == "* (You don't know what these are.)");
        assert(run.speakers[at + 1] == IntroSpeaker::Narrator);
        assert(run.pages[at + 2] == "* (...You know exactly what these are.)");
        assert(run.pages[at + 3] == "Oh, you're FUN.");
        assert(run.speakers[at + 3] == IntroSpeaker::Floofy);
        assert(!contains(run.pages, "That's the spirit!"));
    }

    // ---- dodge once: "Hey, buddy..." and a slower retry, no extra annoyance
    {
        Run run(petalsDef);
        run.playUntilPetalsOrEnd(0);
        const float firstSpeed = run.script.round().speed;
        const std::size_t before = run.pages.size();
        assert(run.script.petalsDodged(run.flags));
        run.playUntilPetalsOrEnd(0);
        assert(run.script.phase() == IntroPhase::Petals);
        assert(run.pages.size() - before == 2U);
        assert(run.pages[before] == "Hey, buddy. You missed them.");
        assert(run.pages[before + 1] == "Let's try again, okay?");
        const IntroPetalRound retry = run.script.round();
        assert(retry.kind == IntroRoundKind::Retry);
        assert(retry.dodgeable);
        assert(retry.speed < firstSpeed);
        assert(run.flags["ruins.floofy.dodges"] == 1);
        assert(run.flags["ruins.floofy.annoyed"] == 0);
        assert(countEffect(run.effects, IntroEffectKind::Flash) == 0);

        // caught on the retry: same forced-hit numbers
        const IntroHitResult hit = run.script.petalsHit(run.flags, 20, 0);
        assert(hit.damage == 5 && hit.furDelta == 20 && hit.furBarFirst);
        run.playUntilPetalsOrEnd(0);
        assert(run.script.finished());
        assert(run.flags["ruins.floofy.fight"] == FLOOFY_FIGHT_PETALS);
    }

    // ---- dodge twice: sinister flash, annoyance +1, fast undodgeable petals, the hit lands
    {
        Run run(petalsDef);
        run.playUntilPetalsOrEnd(0);
        assert(run.script.petalsDodged(run.flags));
        run.playUntilPetalsOrEnd(0);
        const float retrySpeed = run.script.round().speed;
        const std::size_t before = run.pages.size();
        assert(run.script.petalsDodged(run.flags));
        run.collect();
        assert(countEffect(run.effects, IntroEffectKind::Flash) == 1);
        bool sinister = false;
        for (const IntroEffect& e : run.effects) {
            if (e.kind == IntroEffectKind::Flash) {
                sinister = e.id == "sinister";
                assert(e.seconds > 0.0f);
            }
        }
        assert(sinister);
        run.playUntilPetalsOrEnd(0);
        assert(run.pages[before] == "Is this a JOKE, darling? Are you ALLERGIC?");
        assert(run.pages[before + 1] == "RUN. INTO. THE. PETALS!!!");
        const IntroPetalRound forced = run.script.round();
        assert(forced.kind == IntroRoundKind::Forced);
        assert(forced.pattern == "forced_petal");
        assert(!forced.dodgeable);
        assert(forced.speed > retrySpeed);
        assert(run.flags["ruins.floofy.dodges"] == 2);
        assert(run.flags["ruins.floofy.annoyed"] == 1);

        // it can't be dodged: the call is refused and the phase stays
        assert(!run.script.petalsDodged(run.flags));
        assert(run.script.phase() == IntroPhase::Petals);
        assert(run.flags["ruins.floofy.dodges"] == 2);

        const IntroHitResult hit = run.script.petalsHit(run.flags, 20, 0);
        assert(hit.damage == 5);
        run.playUntilPetalsOrEnd(0);
        assert(run.script.finished());
        // annoyed 1 only -> still the petal fight
        assert(run.script.fightVariant() == FLOOFY_FIGHT_PETALS);
    }

    // ---- annoyed >= 2 -> the Bass Shake variant: rude choice 2 + two dodges
    {
        Run run(bassDef);
        run.playUntilPetalsOrEnd(1);
        assert(run.script.petalsDodged(run.flags));
        run.playUntilPetalsOrEnd(1);
        assert(run.script.petalsDodged(run.flags));
        run.playUntilPetalsOrEnd(1);
        assert(run.flags["ruins.floofy.annoyed"] == 2);
        assert(run.script.petalsHit(run.flags, 20, 0).damage == 5);
        const std::size_t before = run.pages.size();
        run.playUntilPetalsOrEnd(1);
        assert(run.script.finished());
        assert(run.script.fightVariant() == FLOOFY_FIGHT_BASS);
        assert(run.flags["ruins.floofy.fight"] == FLOOFY_FIGHT_BASS);
        assert(introFightEncounter(FLOOFY_FIGHT_BASS) == "floofy_bass");
        const std::vector<std::string> dance = {
            "...",
            "You think you're FUNNY?",
            "I've been nice. I've been SO nice.",
            "Now hold still, darling~ ...no. Actually? Don't.",
            "DANCE."
        };
        assert(run.pages.size() >= before + dance.size());
        const std::size_t tail = run.pages.size() - dance.size();
        for (std::size_t i = 0; i < dance.size(); ++i) assert(run.pages[tail + i] == dance[i]);
        assert(!contains(run.pages, "Now hold still, darling~ you'd make a FABULOUS fox. Let's get you fluffy!"));
        // the record scratch lands after "DANCE."
        bool scratch = false;
        for (const IntroEffect& e : run.effects) {
            if (e.kind == IntroEffectKind::Sound && e.id == "record_scratch") scratch = true;
        }
        assert(scratch);
    }

    // ---- annoyance carried in from the overworld (pokes, choice 1) counts at beat 18
    {
        Run run(petalsDef);
        run.flags["ruins.floofy.annoyed"] = 1;  // choice 1 "You broke my fall."
        run.playUntilPetalsOrEnd(1);            // + "Those are NOT petals."
        assert(run.flags["ruins.floofy.annoyed"] == 2);
        run.script.petalsHit(run.flags, 20, 0);
        run.playUntilPetalsOrEnd(1);
        assert(run.script.fightVariant() == FLOOFY_FIGHT_BASS);
    }

    // ---- the lock: a retry-style second pick keeps the first variant
    {
        IntroFlags f;
        f["ruins.floofy.annoyed"] = 0;
        assert(floofyLockFight(f, 0) == FLOOFY_FIGHT_PETALS);
        f["ruins.floofy.annoyed"] = 4;
        assert(floofyLockFight(f, 4) == FLOOFY_FIGHT_PETALS);
        assert(f["ruins.floofy.fight"] == FLOOFY_FIGHT_PETALS);
    }

    // ---- forced-hit numbers
    {
        assert(introHitDamage(20, 25) == 5);
        assert(introHitDamage(1, 25) == 1);
        assert(introHitDamage(40, 25) == 10);
        assert(introHpAfterHit(20, 5) == 15);
        assert(introHpAfterHit(4, 5) == 1);  // the tutorial can't kill
        IntroFlags f;
        BattleIntroScript script(petalsDef);
        script.start(f);
        // FUR cap 99 and an already-seen bar
        Run run(petalsDef);
        run.playUntilPetalsOrEnd(0);
        run.flags["fur_hud_seen"] = 1;
        const IntroHitResult seen = run.script.petalsHit(run.flags, 20, 0);
        assert(seen.furDelta == 20 && !seen.furBarFirst);
        Run capped(petalsDef);
        capped.playUntilPetalsOrEnd(0);
        const IntroHitResult hi = capped.script.petalsHit(capped.flags, 20, 90);
        assert(hi.furDelta == 9);  // capped at 99 in R2
        assert(!hi.furBarFirst);
    }

    // ---- FUR-003 tremble clock for the intro HUD
    {
        FurHudTrembleClock clock;
        assert(!clock.running());
        assert(clock.jitter() == 0);
        clock.start();
        assert(clock.running());
        bool ended = false;
        int ticks = 0;
        for (int i = 0; i < 100 && !ended; ++i) {
            ended = clock.update(1.0f / 30.0f);
            ++ticks;
        }
        assert(ended);
        assert(ticks >= 44 && ticks <= 46);
        assert(!clock.running());
        assert(clock.jitter() == 0);
        assert(!clock.update(1.0f));  // reports the end exactly once
    }

    // ---- malformed data is refused
    {
        tiled_detail::JsonValue bad;
        assert(tiled_detail::JsonParser(R"({"beats":[{"type":"nope"}]})").parse(bad));
        IntroScriptDef out;
        assert(!parseIntroScript(bad, out));
        assert(out.empty());
        tiled_detail::JsonValue noLines;
        assert(tiled_detail::JsonParser(R"({"beats":[{"type":"say"}]})").parse(noLines));
        assert(!parseIntroScript(noLines, out));
    }

    std::cout << "[PASS] C1-002/ru19: battle intro script (beats 8-18, choice 2, dodge branches, forced hit, "
                 "FUR bar tremble, twist, variant pick and lock, retry skip)." << std::endl;
}
