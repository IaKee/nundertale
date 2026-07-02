#pragma once

#include <cassert>
#include <iostream>
#include <map>
#include <string>
#include "core/GameContext.hpp"
#include "core/RaidClock.hpp"

struct MockFlagState {
    std::map<std::string, int> flags;

    void setFlag(const std::string& key, int val) {
        flags[key] = val;
    }

    int getFlag(const std::string& key, int defVal = 0) const {
        auto it = flags.find(key);
        return (it != flags.end()) ? it->second : defVal;
    }
};

inline void suite_rad004() {
    using namespace nundertale;

    // 1. Not started clock
    {
        RaidClock clock;
        assert(!clock.started);
        assert(!raidExpired(clock));
        assert(raidTimeLeftSeconds(clock) == 0);

        auto query = raidEndingQuery(clock);
        assert(!query.expired);
        assert(query.timeLeftSeconds == 0);
        assert(!query.speedrun);

        GameState state;
        raidSyncFlags(clock, state);
        assert(state.getFlag("raid.expired", -1) == 0);
        assert(state.getFlag("raid.time_left", -1) == 0);
    }

    // 2. Running clock & rounding up behavior
    {
        RaidClock clock;
        raidStart(clock, 1320.0f); // 22 minutes
        assert(clock.started);
        assert(!raidExpired(clock));
        assert(raidTimeLeftSeconds(clock) == 1320);

        // Tick fractional amount: 1320.0 - 0.2 = 1319.8 -> ceil is 1320
        raidTick(clock, 0.2f, false);
        assert(raidTimeLeftSeconds(clock) == 1320);

        // Tick 0.9s more: 1318.9 -> ceil is 1319
        raidTick(clock, 0.9f, false);
        assert(raidTimeLeftSeconds(clock) == 1319);

        // Low fractional time: 0.2s left -> ceil is 1
        clock.timeLeft = 0.2f;
        assert(!raidExpired(clock));
        assert(raidTimeLeftSeconds(clock) == 1);

        // 0.001s left -> ceil is 1
        clock.timeLeft = 0.001f;
        assert(!raidExpired(clock));
        assert(raidTimeLeftSeconds(clock) == 1);

        // Paused state preserves timeLeft
        raidTick(clock, 10.0f, true);
        assert(clock.timeLeft == 0.001f);
        assert(raidTimeLeftSeconds(clock) == 1);
    }

    // 3. Exactly at 0 and past 0 (expired)
    {
        RaidClock clock;
        raidStart(clock, 100.0f);
        raidTick(clock, 100.0f, false);
        assert(clock.timeLeft == 0.0f);
        assert(raidExpired(clock));
        assert(raidTimeLeftSeconds(clock) == 0);

        auto query = raidEndingQuery(clock);
        assert(query.expired);
        assert(query.timeLeftSeconds == 0);
        assert(!query.speedrun);

        // Past 0 tick (clamped to 0.0f in raidTick)
        raidTick(clock, 50.0f, false);
        assert(clock.timeLeft == 0.0f);
        assert(raidExpired(clock));
        assert(raidTimeLeftSeconds(clock) == 0);

        // Even if timeLeft is negative by direct assignment, raidExpired and raidTimeLeftSeconds handle it
        clock.timeLeft = -5.0f;
        assert(raidExpired(clock));
        assert(raidTimeLeftSeconds(clock) == 0);
    }

    // 4. Flags sync values with GameState and custom StateLike
    {
        RaidClock clock;
        raidStart(clock, 650.0f);

        GameState state;
        raidSyncFlags(clock, state);
        assert(state.getFlag("raid.expired") == 0);
        assert(state.getFlag("raid.time_left") == 650);

        // Expire clock and sync
        clock.timeLeft = 0.0f;
        raidSyncFlags(clock, state);
        assert(state.getFlag("raid.expired") == 1);
        assert(state.getFlag("raid.time_left") == 0);

        // Test with mock interface / map-backed state
        MockFlagState mockState;
        clock.timeLeft = 125.4f;
        raidSyncFlags(clock, mockState);
        assert(mockState.getFlag("raid.expired") == 0);
        assert(mockState.getFlag("raid.time_left") == 126);
    }

    // 5. Speedrun threshold boundaries (LORE 22.4: > 10:00 / 600s left)
    {
        // 601 seconds left -> speedrun true
        auto q601 = raidEndingQuery(false, 601, 1320);
        assert(!q601.expired);
        assert(q601.speedrun);

        // Exactly 600 seconds left -> speedrun false (> 10:00 required)
        auto q600 = raidEndingQuery(false, 600, 1320);
        assert(!q600.expired);
        assert(!q600.speedrun);

        // 599 seconds left -> speedrun false
        auto q599 = raidEndingQuery(false, 599, 1320);
        assert(!q599.expired);
        assert(!q599.speedrun);

        // Clock overload: 600.2s left -> ceil is 601 -> speedrun true
        RaidClock clock;
        raidStart(clock, 600.2f);
        auto qClock = raidEndingQuery(clock);
        assert(qClock.timeLeftSeconds == 601);
        assert(qClock.speedrun);

        // Clock at exactly 600.0f -> ceil is 600 -> speedrun false
        clock.timeLeft = 600.0f;
        auto qClockExact = raidEndingQuery(clock);
        assert(qClockExact.timeLeftSeconds == 600);
        assert(!qClockExact.speedrun);

        // Expired cannot be speedrun even if invalid positive value was somehow passed
        auto qExpired = raidEndingQuery(true, 700, 1320);
        assert(qExpired.expired);
        assert(!qExpired.speedrun);
    }

    std::cout << "[PASS] RAD-004: raid clock ending queries, flag sync, and speedrun threshold verified." << std::endl;
}
