#pragma once

#include "combat/KillAnnouncer.hpp"
#include "core/GameContext.hpp"
#include "core/RandomEncounters.hpp"
#include "core/ToxicTrack.hpp"

#include <cassert>
#include <cmath>
#include <iostream>

inline void suite_ru24() {
    using namespace nundertale;

    // 1. Basic streak to announcement kind mapping & text
    assert(streakToAnnouncementKind(1) == KillAnnouncementKind::None);
    assert(streakToAnnouncementKind(2) == KillAnnouncementKind::DoubleKill);
    assert(streakToAnnouncementKind(3) == KillAnnouncementKind::TripleKill);
    assert(streakToAnnouncementKind(4) == KillAnnouncementKind::QuadraKill);
    assert(streakToAnnouncementKind(5) == KillAnnouncementKind::PentaKill);
    assert(streakToAnnouncementKind(6) == KillAnnouncementKind::None);

    assert(announcementKindBannerText(KillAnnouncementKind::FirstBlood) == "FIRST BLOOD");
    assert(announcementKindBannerText(KillAnnouncementKind::DoubleKill) == "DOUBLE KILL");
    assert(announcementKindBannerText(KillAnnouncementKind::TripleKill) == "TRIPLE KILL");
    assert(announcementKindBannerText(KillAnnouncementKind::QuadraKill) == "QUADRA KILL");
    assert(announcementKindBannerText(KillAnnouncementKind::PentaKill) == "PENTA KILL");
    assert(announcementKindBannerText(KillAnnouncementKind::Ace) == "ACE");

    // 2. KillAnnouncer standalone logic (without GameState)
    {
        KillAnnouncer announcer(3.0f, 4.0f);
        assert(announcer.empty());
        assert(!announcer.hasActiveBanner());
        assert(announcer.pendingCount() == 0);

        // Kill 1: First blood
        announcer.onKill(1, 1);
        assert(announcer.hasActiveBanner());
        assert(announcer.pendingCount() == 0);
        assert(announcer.currentBanner().kind == KillAnnouncementKind::FirstBlood);
        assert(announcer.currentBanner().bannerText == "FIRST BLOOD");
        assert(announcer.currentBanner().voiceLine == "FIRST BLOOD");
        assert(announcer.currentBanner().subText.empty());
        assert(std::fabs(announcer.currentRemainingTime() - 3.0f) < 0.001f);

        // Advance time partially
        announcer.update(1.5f);
        assert(announcer.hasActiveBanner());
        assert(announcer.currentBanner().kind == KillAnnouncementKind::FirstBlood);
        assert(std::fabs(announcer.currentRemainingTime() - 1.5f) < 0.001f);

        // While FIRST BLOOD is active, Kill 2 happens (DOUBLE KILL queued)
        announcer.onKill(2, 2);
        // Never two banners at once: current banner is still FIRST BLOOD, queue has 1
        assert(announcer.hasActiveBanner());
        assert(announcer.currentBanner().kind == KillAnnouncementKind::FirstBlood);
        assert(announcer.pendingCount() == 1);

        // Advance time to expire FIRST BLOOD
        announcer.update(1.5f);
        // DOUBLE KILL pops immediately
        assert(announcer.hasActiveBanner());
        assert(announcer.currentBanner().kind == KillAnnouncementKind::DoubleKill);
        assert(announcer.currentBanner().bannerText == "DOUBLE KILL");
        assert(announcer.currentBanner().voiceLine == "DOUBLE KILL");
        assert(announcer.pendingCount() == 0);
        assert(std::fabs(announcer.currentRemainingTime() - 3.0f) < 0.001f);

        // Expire DOUBLE KILL
        announcer.update(3.0f);
        assert(announcer.empty());
        assert(!announcer.hasActiveBanner());

        // Kill 3 with streak 3 -> TRIPLE KILL
        announcer.onKill(3, 3);
        assert(announcer.hasActiveBanner());
        assert(announcer.currentBanner().kind == KillAnnouncementKind::TripleKill);
        assert(announcer.currentBanner().bannerText == "TRIPLE KILL");

        // Kill 4 with streak 4 -> QUADRA KILL queued
        announcer.onKill(4, 4);
        assert(announcer.pendingCount() == 1);

        // Kill 5 with streak 5 -> PENTA KILL queued
        announcer.onKill(5, 5);
        assert(announcer.pendingCount() == 2);

        // Step time through TRIPLE KILL
        announcer.update(3.0f);
        assert(announcer.currentBanner().kind == KillAnnouncementKind::QuadraKill);
        assert(announcer.currentBanner().bannerText == "QUADRA KILL");
        assert(announcer.pendingCount() == 1);

        // Step time through QUADRA KILL
        announcer.update(3.0f);
        assert(announcer.currentBanner().kind == KillAnnouncementKind::PentaKill);
        assert(announcer.currentBanner().bannerText == "PENTA KILL");
        assert(announcer.pendingCount() == 0);

        // Step time through PENTA KILL
        announcer.update(3.0f);
        assert(announcer.empty());
    }

    // 3. Area exhaustion and ACE banner with '* But nobody logged in.'
    {
        KillAnnouncer announcer(3.0f, 4.0f);

        // Ruins cap is 6 (from TOXIC_ENCOUNTER_CAPS in RandomEncounters.hpp)
        assert(toxicEncounterCap("ruins") == 6);

        // Check area check when area reaches 6
        bool triggered = announcer.checkAreaExhausted("ruins", 6);
        assert(triggered);
        assert(announcer.hasActiveBanner());
        assert(announcer.currentBanner().kind == KillAnnouncementKind::Ace);
        assert(announcer.currentBanner().bannerText == "ACE");
        assert(announcer.currentBanner().voiceLine == "ACE");
        assert(announcer.currentBanner().subText == "* But nobody logged in.");
        assert(std::fabs(announcer.currentBanner().duration - 4.0f) < 0.001f);

        // Consecutive calls should not re-trigger ACE for the same area
        assert(!announcer.checkAreaExhausted("ruins", 6));
        assert(!announcer.checkAreaExhausted("ruins", 7));
        assert(announcer.pendingCount() == 0);

        // Update to finish ACE
        announcer.update(4.0f);
        assert(announcer.empty());

        // Unknown area never triggers ACE
        assert(!announcer.checkAreaExhausted("core", 10));
        assert(announcer.empty());
    }

    // 4. Full sequence with GameState integration, streak reset on save, and area exhaustion
    {
        GameState state;
        KillAnnouncer announcer;

        // Dummy kill excluded from toxic track
        bool dummyKill = announcer.onKill(state, "fursuit_dummy", "ruins");
        assert(!dummyKill);
        assert(announcer.empty());
        assert(toxicKills(state) == 0);
        assert(toxicStreak(state) == 0);

        // Kill 1 in ruins: FIRST BLOOD
        bool k1 = announcer.onKill(state, "froggsona", "ruins");
        assert(k1);
        assert(toxicKills(state) == 1);
        assert(toxicStreak(state) == 1);
        assert(announcer.hasActiveBanner());
        assert(announcer.currentBanner().kind == KillAnnouncementKind::FirstBlood);
        assert(announcer.pendingCount() == 0);

        // Banner finishes
        announcer.update(DEFAULT_ANNOUNCER_BANNER_DURATION);
        assert(announcer.empty());

        // Save point: resets streak
        announcer.onSave(state);
        assert(toxicStreak(state) == 0);
        assert(toxicKills(state) == 1);

        // Kill 2 after save: kills=2, streak=1 (no multikill banner generated, no first blood since kills > 1)
        bool k2 = announcer.onKill(state, "shysona", "ruins");
        assert(k2);
        assert(toxicKills(state) == 2);
        assert(toxicStreak(state) == 1);
        assert(announcer.empty());

        // Kill 3 without save: kills=3, streak=2 -> DOUBLE KILL
        bool k3 = announcer.onKill(state, "modsmal", "ruins");
        assert(k3);
        assert(toxicKills(state) == 3);
        assert(toxicStreak(state) == 2);
        assert(announcer.hasActiveBanner());
        assert(announcer.currentBanner().kind == KillAnnouncementKind::DoubleKill);
        assert(announcer.pendingCount() == 0);

        // Kill 4 without waiting: kills=4, streak=3 -> TRIPLE KILL enqueued
        bool k4 = announcer.onKill(state, "barky_fan", "ruins");
        assert(k4);
        assert(toxicKills(state) == 4);
        assert(toxicStreak(state) == 3);
        assert(announcer.currentBanner().kind == KillAnnouncementKind::DoubleKill);
        assert(announcer.pendingCount() == 1);

        // Kill 5: kills=5, streak=4 -> QUADRA KILL enqueued
        bool k5 = announcer.onKill(state, "furiell_admirer", "ruins");
        assert(k5);
        assert(toxicKills(state) == 5);
        assert(toxicStreak(state) == 4);
        assert(announcer.pendingCount() == 2);

        // Kill 6: kills=6 in ruins -> area cap reached (6)!
        // This generates PENTA KILL (streak=5) and ACE (area emptied) enqueued
        bool k6 = announcer.onKill(state, "final_ruins_mob", "ruins");
        assert(k6);
        assert(toxicKills(state) == 6);
        assert(toxicStreak(state) == 5);
        assert(areaExhausted("ruins", state.getFlag("kills.ruins", 0)));
        assert(state.getFlag(areaExhaustedAnnouncedFlag("ruins"), 0) == 1);

        // Double Kill is still currently active; queue contains Triple, Quadra, Penta, and Ace
        assert(announcer.currentBanner().kind == KillAnnouncementKind::DoubleKill);
        assert(announcer.pendingCount() == 4);

        // Pop through all in sequence
        announcer.update(DEFAULT_ANNOUNCER_BANNER_DURATION);
        assert(announcer.currentBanner().kind == KillAnnouncementKind::TripleKill);
        assert(announcer.pendingCount() == 3);

        announcer.update(DEFAULT_ANNOUNCER_BANNER_DURATION);
        assert(announcer.currentBanner().kind == KillAnnouncementKind::QuadraKill);
        assert(announcer.pendingCount() == 2);

        announcer.update(DEFAULT_ANNOUNCER_BANNER_DURATION);
        assert(announcer.currentBanner().kind == KillAnnouncementKind::PentaKill);
        assert(announcer.pendingCount() == 1);

        announcer.update(DEFAULT_ANNOUNCER_BANNER_DURATION);
        assert(announcer.currentBanner().kind == KillAnnouncementKind::Ace);
        assert(announcer.currentBanner().subText == "* But nobody logged in.");
        assert(announcer.pendingCount() == 0);

        announcer.update(ACE_ANNOUNCER_BANNER_DURATION);
        assert(announcer.empty());
    }

    std::cout << "[PASS] TOX-002: LoL kill announcer logic verified." << std::endl;
}
