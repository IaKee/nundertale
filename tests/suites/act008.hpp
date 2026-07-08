#pragma once

#include "core/PeacePath.hpp"

#include <cassert>
#include <iostream>

namespace act008_detail {

// Barky (LORE §19.1): 5 steps, in any mix.
inline nundertale::PeacePathDef barkyDef() {
    nundertale::PeacePathDef def;
    def.goal = 5;
    def.rules = {
        { "Comment on Tail", 1, "", 0, false },
        { "Cringe Check", 1, "H-hey! It's an artisanal design feature, b-baka!", 0, false },
        { "Offer Dew", 1, "Citrus Blast 2012?! ...wait, how do I know that flavor?", 0, false },
        { "Debate Lore", 0, "Season 4 episode 12 was CANON?!", 0, true },
        { "Laser", 1, "NO\xE2\x80\x94 I'M NOT\xE2\x80\x94 WHERE DID IT GO\xE2\x80\x94", 0, true },
        { "Accept Hug", 1, "...this is the best day of my life. Don't tell anyone.", 0, false },
    };
    def.stepLines = {
        "It's... a design choice.",
        "Stop LOOKING at it.",
        "It's load-bearing, okay?!",
        "...fine. It falls off sometimes. Happy?",
    };
    def.furrierPath = true;
    def.furPerHug = 15;
    def.toxicExtraSteps = 2;
    return def;
}

// Furiell (LORE §19.2): 5 steps, get her to let go of your hand.
inline nundertale::PeacePathDef furiellDef() {
    nundertale::PeacePathDef def;
    def.goal = 5;
    def.rules = {
        { "Let Go of Her Paw", 1, "*grabs your hand again* nuh-uh~", 0, false },
        { "Talk About Games", 1, "* You talk about Fortnite. She nods. She is staring at your ears.", 0, false },
        { "Promise to Visit", 1, "...you pwomise? Pinky pwomise?", 2, false },
        { "Insult", 0, "...say that again, pup. Slower.", 0, true },
        { "Accept Hug", 1, "", 0, false },
    };
    def.furrierPath = true;
    def.furPerHug = 15;
    def.toxicExtraSteps = 3;
    return def;
}

} // namespace act008_detail

inline void suite_act008() {
    using namespace nundertale;
    using namespace act008_detail;

    // goal clamping to 4..8, toxic bonus added after the clamp
    PeacePathDef clamp;
    clamp.goal = 1;
    assert(peaceGoal(clamp, false) == 4);
    clamp.goal = 0;
    assert(peaceGoal(clamp, false) == 4);
    clamp.goal = -3;
    assert(peaceGoal(clamp, false) == 4);
    clamp.goal = 4;
    assert(peaceGoal(clamp, false) == 4);
    clamp.goal = 6;
    assert(peaceGoal(clamp, false) == 6);
    clamp.goal = 8;
    assert(peaceGoal(clamp, false) == 8);
    clamp.goal = 99;
    assert(peaceGoal(clamp, false) == 8);
    clamp.toxicExtraSteps = 2;
    assert(peaceGoal(clamp, false) == 8);
    assert(peaceGoal(clamp, true) == 10);
    clamp.goal = 2;
    assert(peaceGoal(clamp, true) == 6);
    clamp.toxicExtraSteps = -5;
    assert(peaceGoal(clamp, true) == 4);

    // Barky: unknown ACT, ACT with no progress, escalating tail lines
    const PeacePathDef barky = barkyDef();
    assert(peaceGoal(barky, false) == 5);
    assert(peaceGoal(barky, true) == 7);
    {
        PeacePathState s;
        PeaceStepResult r = peaceApplyAct(barky, s, "Dance", false);
        assert(!r.known && r.progressAdded == 0 && r.line.empty() && r.furGained == 0 && !r.goalReached);
        assert(s.progress == 0 && s.hugs == 0);
        r = peaceApplyAct(barky, s, "comment on tail", false);  // matching is exact
        assert(!r.known && s.progress == 0);
        r = peaceApplyAct(barky, s, "", false);
        assert(!r.known && s.progress == 0);
        assert(!peaceMercyAllowed(barky, s, false));

        r = peaceApplyAct(barky, s, "Debate Lore", false);
        assert(r.known && r.progressAdded == 0 && r.skipsAttack && s.progress == 0);
        assert(r.line == "Season 4 episode 12 was CANON?!");

        r = peaceApplyAct(barky, s, "Comment on Tail", false);
        assert(r.progressAdded == 1 && r.line == "It's... a design choice." && !r.skipsAttack);
        r = peaceApplyAct(barky, s, "Comment on Tail", false);
        assert(r.line == "Stop LOOKING at it." && s.progress == 2);
        r = peaceApplyAct(barky, s, "Comment on Tail", false);
        assert(r.line == "It's load-bearing, okay?!" && s.progress == 3);
        r = peaceApplyAct(barky, s, "Comment on Tail", false);
        assert(r.line == "...fine. It falls off sometimes. Happy?" && s.progress == 4);
        assert(!r.goalReached && !peaceMercyAllowed(barky, s, false));
        // the 5th step reuses the last escalation line instead of running off the end
        r = peaceApplyAct(barky, s, "Comment on Tail", false);
        assert(r.line == "...fine. It falls off sometimes. Happy?");
        assert(s.progress == 5 && r.goalReached);
        assert(peaceMercyAllowed(barky, s, false));
    }

    // MERCY is blocked until the goal, and the toxic track moves the goal
    {
        PeacePathState s;
        for (int i = 0; i < 4; ++i) {
            assert(!peaceMercyAllowed(barky, s, false));
            peaceApplyAct(barky, s, "Cringe Check", false);
        }
        assert(s.progress == 4 && !peaceMercyAllowed(barky, s, false));
        peaceApplyAct(barky, s, "Offer Dew", false);
        assert(peaceMercyAllowed(barky, s, false));
        // same progress, but the track switched on: 5 of 7 is not enough
        assert(!peaceMercyAllowed(barky, s, true));
        PeaceStepResult r = peaceApplyAct(barky, s, "Offer Dew", true);
        assert(r.progressAdded == 1 && !r.goalReached && s.progress == 6);
        r = peaceApplyAct(barky, s, "Offer Dew", true);
        assert(r.goalReached && s.progress == 7 && peaceMercyAllowed(barky, s, true));
    }

    // progress caps at the goal and goal-reached fires exactly once
    {
        PeacePathState s;
        int reached = 0;
        for (int i = 0; i < 12; ++i) {
            const PeaceStepResult r = peaceApplyAct(barky, s, "Cringe Check", false);
            assert(r.known);
            if (r.goalReached) ++reached;
            assert(s.progress <= 5);
            if (i >= 5) assert(r.progressAdded == 0);
        }
        assert(reached == 1 && s.progress == 5);
        // a known ACT past the goal still shows its line
        assert(peaceApplyAct(barky, s, "Cringe Check", false).line == "H-hey! It's an artisanal design feature, b-baka!");
        // the goal growing later (track started afterwards) lets the step finish again, once
        const PeaceStepResult more = peaceApplyAct(barky, s, "Cringe Check", true);
        assert(more.progressAdded == 1 && !more.goalReached && s.progress == 6);
        assert(peaceApplyAct(barky, s, "Cringe Check", true).goalReached);
        assert(!peaceApplyAct(barky, s, "Cringe Check", true).goalReached);
    }

    // Accept Hug: FUR on every hug (furrier path), none without the path
    {
        PeacePathState s;
        PeaceStepResult r = peaceApplyAct(barky, s, "Accept Hug", false);
        assert(r.furGained == 15 && r.progressAdded == 1 && s.hugs == 1);
        r = peaceApplyAct(barky, s, "Accept Hug", false);
        assert(r.furGained == 15 && s.hugs == 2 && s.progress == 2);
        peaceApplyAct(barky, s, "Accept Hug", false);
        peaceApplyAct(barky, s, "Accept Hug", false);
        r = peaceApplyAct(barky, s, "Accept Hug", false);
        assert(r.goalReached && r.furGained == 15 && s.hugs == 5 && s.progress == 5);
        r = peaceApplyAct(barky, s, "Accept Hug", false);  // past the goal: still a hug, still FUR
        assert(r.furGained == 15 && r.progressAdded == 0 && !r.goalReached && s.hugs == 6);
        assert(r.line == "...this is the best day of my life. Don't tell anyone.");

        PeacePathDef plain = barky;
        plain.furrierPath = false;
        PeacePathState p;
        r = peaceApplyAct(plain, p, "Accept Hug", false);
        assert(r.progressAdded == 1 && r.furGained == 0 && p.hugs == 0);
        PeacePathDef free = barky;
        free.furPerHug = -4;
        PeacePathState f;
        assert(peaceApplyAct(free, f, "Accept Hug", false).furGained == 0);
    }

    // Furiell: Promise to Visit needs 2 steps, Insult gives no progress but skips her attack
    {
        const PeacePathDef furiell = furiellDef();
        assert(peaceGoal(furiell, false) == 5 && peaceGoal(furiell, true) == 8);
        PeacePathState s;
        PeaceStepResult r = peaceApplyAct(furiell, s, "Promise to Visit", false);
        assert(!r.known && s.progress == 0 && r.line.empty());
        r = peaceApplyAct(furiell, s, "Insult", false);
        assert(r.known && r.progressAdded == 0 && r.skipsAttack && r.line == "...say that again, pup. Slower.");
        r = peaceApplyAct(furiell, s, "Let Go of Her Paw", false);
        assert(r.progressAdded == 1 && r.line == "*grabs your hand again* nuh-uh~");
        r = peaceApplyAct(furiell, s, "Promise to Visit", false);
        assert(!r.known && s.progress == 1);
        r = peaceApplyAct(furiell, s, "Talk About Games", false);
        assert(r.progressAdded == 1 && s.progress == 2);
        r = peaceApplyAct(furiell, s, "Promise to Visit", false);
        assert(r.known && r.progressAdded == 1 && s.progress == 3 && r.line == "...you pwomise? Pinky pwomise?");
        assert(!peaceMercyAllowed(furiell, s, false));
        r = peaceApplyAct(furiell, s, "Accept Hug", false);
        assert(r.furGained == 15 && r.line.empty() == true && s.progress == 4 && !r.goalReached);
        r = peaceApplyAct(furiell, s, "Talk About Games", false);
        assert(r.goalReached && peaceMercyAllowed(furiell, s, false));
        assert(!peaceMercyAllowed(furiell, s, true));
        assert(s.hugs == 1);
    }

    std::cout << "[PASS] ACT-008: peace path goal 4-8 (+toxic bonus), unknown ACTs, cap, MERCY gate, "
                 "Accept Hug FUR, goal-reached once, Barky/Furiell examples." << std::endl;
}
