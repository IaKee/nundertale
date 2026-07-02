#pragma once

#include <cassert>
#include <iostream>
#include <string>
#include <string_view>

#include "core/GameContext.hpp"
#include "core/RandomEncounters.hpp"

inline void suite_enc004()
{
    using namespace nundertale;

    // Caps and boundaries for the four capped areas (D-11): 6 / 6 / 5 / 3.
    struct Case { std::string_view area; int cap; };
    const Case cases[] = { { "ruins", 6 }, { "snowdinha", 6 }, { "wetfall", 5 }, { "hotland", 3 } };
    for (const Case& c : cases) {
        assert(toxicEncounterCap(c.area) == c.cap);
        assert(!areaExhausted(c.area, 0));
        assert(!areaExhausted(c.area, c.cap - 1));
        assert(areaExhausted(c.area, c.cap));
        assert(areaExhausted(c.area, c.cap + 1));

        // Exhausted: no encounter on any route, even with budget left.
        for (int track = 0; track <= 2; ++track) {
            assert(!encounterAllowed(0, 5, track, c.cap, c.area));
            assert(!encounterAllowed(0, 5, track, c.cap + 1, c.area));
        }
        // Not exhausted: unchanged semantics.
        assert(encounterAllowed(5, 1, 1, c.cap - 1, c.area));
        assert(encounterAllowed(0, 0, 1, c.cap - 1, c.area));
        assert(encounterAllowed(0, 1, 0, c.cap - 1, c.area));
        assert(!encounterAllowed(1, 1, 0, c.cap - 1, c.area));
        assert(!encounterAllowed(1, 1, 2, c.cap - 1, c.area));
    }

    // Unknown / uncapped areas are never exhausted and keep the regular budget.
    assert(!areaExhausted("", 99));
    assert(!areaExhausted("core", 99));
    assert(!areaExhausted("RUINS", 99));
    assert(encounterAllowed(0, 1, 0, 99, ""));
    assert(!encounterAllowed(1, 1, 0, 99, ""));
    assert(encounterAllowed(0, 1, 1, 99, "core"));

    // Room ids map to the area that carries the cap.
    assert(encounterAreaForRoom("ruins_r8") == "ruins");
    assert(areaExhausted(encounterAreaForRoom("hotland_h1"), 3));
    assert(!areaExhausted(encounterAreaForRoom("unknown_room"), 99));

    // Notice text is exact.
    assert(AREA_EXHAUSTED_NOTICE == "* But nobody logged in.");

    // The notice is due only once the cap is reached, and only until announced.
    assert(areaExhaustedAnnouncedFlag("wetfall") == "kills.wetfall.announced");
    assert(!areaExhaustedNoticeDue("wetfall", 0, 0));
    assert(!areaExhaustedNoticeDue("wetfall", 4, 0));
    assert(areaExhaustedNoticeDue("wetfall", 5, 0));
    assert(areaExhaustedNoticeDue("wetfall", 6, 0));
    assert(!areaExhaustedNoticeDue("wetfall", 5, 1));
    assert(!areaExhaustedNoticeDue("core", 99, 0));

    // Flow with real game flags: fires once per area, independent across areas.
    GameState state;
    const std::string_view area = "hotland";
    auto noticeDue = [&]() {
        return areaExhaustedNoticeDue(area, state.getFlag("kills.hotland", 0),
                                      state.getFlag(areaExhaustedAnnouncedFlag(area), 0));
    };
    int shown = 0;
    for (int kill = 0; kill < 6; ++kill) {
        if (noticeDue()) {
            state.setFlag(areaExhaustedAnnouncedFlag(area), 1);
            ++shown;
        }
        if (kill < 3) {
            assert(shown == 0);
            assert(encounterAllowed(0, 3, 1, state.getFlag("kills.hotland", 0), area));
        } else {
            assert(shown == 1);
            assert(!encounterAllowed(0, 3, 1, state.getFlag("kills.hotland", 0), area));
        }
        state.incFlag("kills.hotland");
        if (kill == 2) {
            // Cap reached: the next overworld check announces it.
            assert(noticeDue());
        }
    }
    assert(shown == 1);
    assert(!noticeDue());
    // Another area is unaffected by hotland's announcement.
    assert(!areaExhaustedNoticeDue("ruins", 5, state.getFlag(areaExhaustedAnnouncedFlag("ruins"), 0)));
    assert(areaExhaustedNoticeDue("ruins", 6, state.getFlag(areaExhaustedAnnouncedFlag("ruins"), 0)));

    std::cout << "[PASS] ENC-004: caps 6/6/5/3, exhaustion boundaries, one-time 'nobody logged in' notice, encounters stop."
              << std::endl;
}
