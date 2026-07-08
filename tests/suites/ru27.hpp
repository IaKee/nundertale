#pragma once

#include "combat/Killfeed.hpp"
#include "core/GameContext.hpp"

#include <cassert>
#include <cmath>
#include <iostream>

inline void suite_ru27() {
    using namespace nundertale;

    // 1. Ex-gamer gamertag table lookup
    {
        // Barky -> xX_BarkSc0p3_Xx
        assert(lookupExGamerGamertag("barky") == "xX_BarkSc0p3_Xx");
        assert(lookupExGamerGamertag("Barky") == "xX_BarkSc0p3_Xx");
        assert(lookupExGamerGamertag("BARKY") == "xX_BarkSc0p3_Xx");

        // Napstablurk -> xX_AFK_Xx
        assert(lookupExGamerGamertag("napstablurk") == "xX_AFK_Xx");
        assert(lookupExGamerGamertag("Napstablurk") == "xX_AFK_Xx");

        // Jerry -> BrbJerry
        assert(lookupExGamerGamertag("jerry") == "BrbJerry");
        assert(lookupExGamerGamertag("Jerry") == "BrbJerry");

        // Musk-ron -> GymRat_Gaming
        assert(lookupExGamerGamertag("musk_ron") == "GymRat_Gaming");
        assert(lookupExGamerGamertag("Musk-ron") == "GymRat_Gaming");

        // Vtuber-en -> quiet_healer
        assert(lookupExGamerGamertag("vtuber_en") == "quiet_healer");
        assert(lookupExGamerGamertag("Vtuber-en") == "quiet_healer");

        // Mettapaw -> ClipFarmer_99
        assert(lookupExGamerGamertag("mettapaw") == "ClipFarmer_99");
        assert(lookupExGamerGamertag("Mettapaw") == "ClipFarmer_99");

        // Non-ex-gamers should return empty
        assert(lookupExGamerGamertag("froggsona").empty());
        assert(lookupExGamerGamertag("furiell").empty());
        assert(lookupExGamerGamertag("sans").empty());

        // resolveVictimName helper
        assert(resolveVictimName("barky") == "xX_BarkSc0p3_Xx");
        assert(resolveVictimName("froggsona") == "froggsona");
        assert(resolveVictimName("Jerry") == "BrbJerry");
    }

    // 2. Killfeed basic push, fields, newest at bottom, ordering
    {
        Killfeed feed(5.0f);
        assert(feed.empty());
        assert(feed.size() == 0);

        feed.push("GRUB", "crowbar", "froggsona", false);
        assert(!feed.empty());
        assert(feed.size() == 1);
        assert(feed.newest().attacker == "GRUB");
        assert(feed.newest().weapon == "crowbar");
        assert(feed.newest().victim == "froggsona");
        assert(!feed.newest().headshot);
        assert(std::fabs(feed.newest().remainingTime - 5.0f) < 0.001f);
        assert(std::fabs(feed.newest().alphaRatio() - 1.0f) < 0.001f);
        assert(feed.newest().formatText() == "GRUB [crowbar] froggsona");

        // Second push: newest at bottom
        feed.push("GRUB", "crowbar", "barky", true);
        assert(feed.size() == 2);
        // Index 0 is oldest (froggsona)
        assert(feed[0].victim == "froggsona");
        assert(!feed[0].headshot);
        // Index 1 is newest (Barky converted to gamertag xX_BarkSc0p3_Xx with headshot)
        assert(feed[1].victim == "xX_BarkSc0p3_Xx");
        assert(feed[1].headshot);
        assert(feed.newest().victim == "xX_BarkSc0p3_Xx");
        assert(feed.oldest().victim == "froggsona");
        assert(feed.newest().formatText() == "GRUB [crowbar] (HS) xX_BarkSc0p3_Xx");
    }

    // 3. Max 4 lines cap (dropping oldest when 5th pushed)
    {
        Killfeed feed(5.0f);
        feed.push("GRUB", "crowbar", "victim1");
        feed.push("GRUB", "crowbar", "victim2");
        feed.push("GRUB", "crowbar", "victim3");
        feed.push("GRUB", "crowbar", "victim4");
        assert(feed.size() == 4);
        assert(feed[0].victim == "victim1");
        assert(feed[1].victim == "victim2");
        assert(feed[2].victim == "victim3");
        assert(feed[3].victim == "victim4");
        assert(feed.oldest().victim == "victim1");
        assert(feed.newest().victim == "victim4");

        // Pushing a 5th entry drops victim1 (oldest)
        feed.push("GRUB", "dorans_blade", "victim5", true);
        assert(feed.size() == 4);
        assert(feed[0].victim == "victim2");
        assert(feed[1].victim == "victim3");
        assert(feed[2].victim == "victim4");
        assert(feed[3].victim == "victim5");
        assert(feed.oldest().victim == "victim2");
        assert(feed.newest().victim == "victim5");
        assert(feed.newest().weapon == "dorans_blade");
        assert(feed.newest().headshot);

        // Pushing a 6th drops victim2
        feed.push("GRUB", "dragon_scimitar", "barky");
        assert(feed.size() == 4);
        assert(feed[0].victim == "victim3");
        assert(feed[1].victim == "victim4");
        assert(feed[2].victim == "victim5");
        assert(feed[3].victim == "xX_BarkSc0p3_Xx");
    }

    // 4. Update and 5s expiration
    {
        Killfeed feed(5.0f);
        feed.push("GRUB", "crowbar", "v1", false, 5.0f);
        feed.update(2.0f);

        // Add v2 after 2 seconds
        feed.push("GRUB", "crowbar", "v2", true, 5.0f);
        assert(feed.size() == 2);
        assert(std::fabs(feed[0].remainingTime - 3.0f) < 0.001f);
        assert(std::fabs(feed[1].remainingTime - 5.0f) < 0.001f);
        assert(std::fabs(feed[0].alphaRatio() - 0.6f) < 0.01f);
        assert(std::fabs(feed[1].alphaRatio() - 1.0f) < 0.01f);

        // Advance 3 more seconds: v1 expires (total 5s), v2 has 2s left
        feed.update(3.0f);
        assert(feed.size() == 1);
        assert(feed[0].victim == "v2");
        assert(std::fabs(feed[0].remainingTime - 2.0f) < 0.001f);
        assert(!feed.empty());

        // Advance 2 more seconds: v2 expires
        feed.update(2.0f);
        assert(feed.empty());
        assert(feed.size() == 0);
    }

    // 5. GameState push integration
    {
        GameState state;
        state.player.name = "GRUB";
        state.player.weapon = "dragon_scimitar";

        Killfeed feed;
        feed.pushKill(state, "jerry", true);
        assert(feed.size() == 1);
        assert(feed.newest().attacker == "GRUB");
        assert(feed.newest().weapon == "dragon_scimitar");
        assert(feed.newest().victim == "BrbJerry");
        assert(feed.newest().headshot);

        // Without weapon, defaults to crowbar
        GameState state2;
        state2.player.weapon = "";
        feed.pushKill(state2, "napstablurk", false);
        assert(feed.size() == 2);
        assert(feed.newest().weapon == "crowbar");
        assert(feed.newest().victim == "xX_AFK_Xx");
        assert(!feed.newest().headshot);
    }

    // 6. Colors and constants
    {
        assert(KILLFEED_MAX_LINES == 4);
        assert(std::fabs(KILLFEED_DEFAULT_LIFETIME - 5.0f) < 0.001f);
        assert(KILLFEED_COLOR_ATTACKER_RED.r == 255);
        assert(KILLFEED_COLOR_VICTIM_BLUE.b == 255);
        assert(KILLFEED_COLOR_WEAPON_ORANGE.r == 255);
        assert(KILLFEED_COLOR_HEADSHOT_YELLOW.r == 255);
    }

    std::cout << "[PASS] TOX-006: Counter-Strike 1.6 killfeed logic verified." << std::endl;
}
