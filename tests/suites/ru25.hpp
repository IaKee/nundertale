#pragma once

#include "combat/Hitsplat.hpp"
#include "core/GameContext.hpp"
#include "core/ToxicTrack.hpp"

#include <cassert>
#include <cmath>
#include <iostream>
#include <string>

inline void suite_ru25() {
    using namespace nundertale;

    // 1. HitsplatKind mapping from HitResultKind
    assert(hitsplatKindFor(HitResultKind::Miss) == HitsplatKind::Miss);
    assert(hitsplatKindFor(HitResultKind::Hit) == HitsplatKind::Damage);
    assert(hitsplatKindFor(HitResultKind::Crit) == HitsplatKind::MaxHit);

    // HitsplatKind mapping from damage + flags
    assert(hitsplatKindFor(0, false, true) == HitsplatKind::Miss);
    assert(hitsplatKindFor(0, false, false) == HitsplatKind::Miss);
    assert(hitsplatKindFor(12, false, false) == HitsplatKind::Damage);
    assert(hitsplatKindFor(45, true, false) == HitsplatKind::MaxHit);
    assert(hitsplatKindFor(0, true, true) == HitsplatKind::Miss);

    // 2. Hitsplat activation condition (toxic track == 1 and kills >= 1)
    {
        GameState state;
        assert(!hitsplatsActive(state));
        assert(!xpDropsActive(state));

        // Track 0, kills 0
        assert(!hitsplatsActive(0, 0));
        // Track 1, kills 0
        state.setFlag("toxic.track", 1);
        assert(!hitsplatsActive(state));
        assert(!hitsplatsActive(0, 1));

        // Track 1, kills 1 (activated on 1st kill!)
        state.setFlag("toxic.kills", 1);
        assert(hitsplatsActive(state));
        assert(hitsplatsActive(1, 1));
        assert(xpDropsActive(state));
        assert(xpDropsActive(1, 1));

        // Track 2 (ended / spared) -> inactive
        state.setFlag("toxic.track", 2);
        assert(!hitsplatsActive(state));
        assert(!hitsplatsActive(1, 2));
        assert(!xpDropsActive(state));
        assert(!xpDropsActive(1, 2));
    }

    // 3. Hitsplat struct: progress, alpha, timer update, expiry
    {
        Hitsplat miss = makeHitsplat(HitsplatKind::Miss, 99, 100.0f, 200.0f, 1.0f);
        assert(miss.kind == HitsplatKind::Miss);
        assert(miss.amount == 0); // Miss splat is always 0
        assert(miss.active());
        assert(miss.timer == 1.0f);
        assert(miss.lifetime == 1.0f);
        assert(miss.progress() == 0.0f);
        assert(miss.alpha() == 1.0f);

        // Update 0.5s (50% elapsed)
        miss.update(0.5f);
        assert(miss.active());
        assert(std::fabs(miss.progress() - 0.5f) < 1e-4f);
        assert(miss.alpha() == 1.0f); // fade starts at 75%

        // Update 0.35s (85% elapsed -> 15% remaining -> in fade zone)
        miss.update(0.35f);
        assert(miss.active());
        assert(std::fabs(miss.progress() - 0.85f) < 1e-4f);
        // Alpha at 85% progress: (1 - 0.85) / 0.25 = 0.15 / 0.25 = 0.60
        assert(std::fabs(miss.alpha() - 0.6f) < 1e-3f);

        // Expire
        miss.update(0.2f);
        assert(!miss.active());
        assert(miss.timer == 0.0f);
        assert(miss.alpha() == 0.0f);
    }

    // Damage and MaxHit hitsplats
    {
        Hitsplat dmg = makeHitsplat(HitsplatKind::Damage, 15, 320.0f, 160.0f);
        assert(dmg.kind == HitsplatKind::Damage);
        assert(dmg.amount == 15);

        Hitsplat crit = makeHitsplat(HitsplatKind::MaxHit, 52, 320.0f, 160.0f);
        assert(crit.kind == HitsplatKind::MaxHit);
        assert(crit.amount == 52);
    }

    // 4. XP Drop: motion, text, alpha, expiry
    {
        XpDrop drop = makeXpDrop(12, 600.0f, 300.0f, 50.0f, 2.0f);
        assert(drop.amount == 12);
        assert(drop.text() == "+12");
        assert(drop.active());
        assert(drop.currentY == 300.0f);
        assert(drop.timer == 2.0f);
        assert(drop.progress() == 0.0f);
        assert(drop.alpha() == 1.0f);

        // 1 second passes -> moves upward by floatSpeed * 1.0 = 50px -> currentY becomes 250.0f
        drop.update(1.0f);
        assert(drop.active());
        assert(std::fabs(drop.currentY - 250.0f) < 1e-4f);
        assert(std::fabs(drop.progress() - 0.5f) < 1e-4f);
        assert(drop.alpha() == 1.0f); // 50% <= 60%, still full opacity

        // 0.4 seconds passes -> progress 1.4 / 2.0 = 0.7 (in fade zone: (1 - 0.7) / 0.4 = 0.75 alpha)
        drop.update(0.4f);
        assert(drop.active());
        assert(std::fabs(drop.currentY - 230.0f) < 1e-4f);
        assert(std::fabs(drop.progress() - 0.7f) < 1e-4f);
        assert(std::fabs(drop.alpha() - 0.75f) < 1e-3f);

        // Expire
        drop.update(0.7f);
        assert(!drop.active());
        assert(drop.timer == 0.0f);
        assert(drop.alpha() == 0.0f);
    }

    // 5. ToxicCombatFxManager container
    {
        ToxicCombatFxManager mgr;
        assert(mgr.hitsplatCount() == 0);
        assert(mgr.xpDropCount() == 0);

        mgr.spawnHit(HitResultKind::Miss, 0, 100.0f, 100.0f, 1.0f);
        mgr.spawnHit(HitResultKind::Hit, 25, 200.0f, 100.0f, 1.0f);
        mgr.spawnHit(HitResultKind::Crit, 70, 300.0f, 100.0f, 1.0f);
        mgr.spawnXpDrop(15, 600.0f, 320.0f, 40.0f, 1.0f);

        assert(mgr.hitsplatCount() == 3);
        assert(mgr.xpDropCount() == 1);
        assert(mgr.hitsplats()[0].kind == HitsplatKind::Miss);
        assert(mgr.hitsplats()[1].kind == HitsplatKind::Damage);
        assert(mgr.hitsplats()[2].kind == HitsplatKind::MaxHit);
        assert(mgr.xpDrops()[0].text() == "+15");

        // Partial update
        mgr.update(0.5f);
        assert(mgr.hitsplatCount() == 3);
        assert(mgr.xpDropCount() == 1);

        // Expire all
        mgr.update(0.6f);
        assert(mgr.hitsplatCount() == 0);
        assert(mgr.xpDropCount() == 0);

        // Clear verification
        mgr.spawnHitsplat(HitsplatKind::Damage, 10, 100.0f, 100.0f);
        mgr.spawnXpDrop(5);
        assert(mgr.hitsplatCount() == 1);
        assert(mgr.xpDropCount() == 1);
        mgr.clear();
        assert(mgr.hitsplatCount() == 0);
        assert(mgr.xpDropCount() == 0);
    }

    // 6. Level-up line threshold & OSRS jingle verification
    {
        GameState state;
        state.setFlag("toxic.track", 1);

        // Kills 0..8: default line "* Your LEVEL OF VITRIOL increased." and default SFX
        for (int k = 0; k < 9; ++k) {
            state.setFlag("toxic.kills", k);
            assert(!shouldUseOsrsLevelUp(state));
            assert(!shouldUseOsrsLevelUp(k, 1));
            assert(levelUpNarratorLine(state) == DEFAULT_LEVEL_UP_LINE);
            assert(levelUpNarratorLine(k, 1) == "* Your LEVEL OF VITRIOL increased.");
            assert(levelUpSoundSfx(state) == DEFAULT_LEVEL_UP_SFX);
            assert(levelUpSoundSfx(k, 1) == "assets/sfx/level_up.ogg");
        }

        // Kill 9 and above: OSRS line "* Congratulations, you just advanced a Vitriol level." and OSRS jingle SFX
        for (int k = 9; k <= 25; ++k) {
            state.setFlag("toxic.kills", k);
            assert(shouldUseOsrsLevelUp(state));
            assert(shouldUseOsrsLevelUp(k, 1));
            assert(levelUpNarratorLine(state) == OSRS_LEVEL_UP_LINE);
            assert(levelUpNarratorLine(k, 1) == "* Congratulations, you just advanced a Vitriol level.");
            assert(levelUpSoundSfx(state) == OSRS_LEVEL_UP_JINGLE_SFX);
            assert(levelUpSoundSfx(k, 1) == "assets/sfx/osrs_level_up.ogg");
        }

        // Spared / ended track (toxic.track != 1) -> does not use OSRS level up even if kills >= 9
        state.setFlag("toxic.track", 2);
        assert(!shouldUseOsrsLevelUp(state));
        assert(!shouldUseOsrsLevelUp(9, 2));
        assert(!shouldUseOsrsLevelUp(15, 0));
        assert(levelUpNarratorLine(state) == DEFAULT_LEVEL_UP_LINE);
        assert(levelUpSoundSfx(state) == DEFAULT_LEVEL_UP_SFX);
    }

    std::cout << "[PASS] TOX-003: OSRS hitsplats (damage/miss/crit), XP drop motion/expiry, and 9th kill level-up line verified." << std::endl;
}
