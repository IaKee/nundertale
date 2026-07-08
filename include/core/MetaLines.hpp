#pragma once

#include "core/SessionMeta.hpp"

#include <string>
#include <vector>

namespace nundertale {

enum class Speaker {
    SansS1,
    FloofyR2,
    FuriellR3,
    SansGrillbys
};

using MetaSpeaker = Speaker;

inline std::vector<std::string> reloadLines(const SessionMeta& session, bool floofyPresent) {
    if (!floofyPresent) {
        return {};
    }
    // LORE §24 / MET-002: WELCOME HOME reload line takes precedence over kill-reload
    if (session.reloadedAfterFurryOver) {
        return {"Did you just reload? I saw that. No shame, though. I'd do it too."};
    }
    if (session.reloadedAfterKill) {
        return {"You killed them and loaded your save. Deleting the evidence? Gamer behavior."};
    }
    return {};
}

inline std::vector<std::string> resetLines(Speaker speaker, int lastEnding, int resets) {
    switch (speaker) {
    case Speaker::SansS1:
        // Any ending: 1 (Gamer), 2 (Clean Run), 3 (Toxic), 4 (Furry)
        if (lastEnding >= 1 && lastEnding <= 4) {
            return {"heh. you look like you've seen this episode before."};
        }
        break;

    case Speaker::FloofyR2:
        // After Toxic ending (3)
        if (lastEnding == 3) {
            return {"You again? You know what you did. I have screenshots. I have NO shame about showing them."};
        }
        break;

    case Speaker::FuriellR3:
        // After Furry ending (4)
        if (lastEnding == 4) {
            return {"...Have we met? You smell familiar."};
        }
        break;

    case Speaker::SansGrillbys:
        // resets >= 3
        if (resets >= 3) {
            return {"you ever feel stuck in a loop? like a gacha banner that never ends?"};
        }
        break;
    }
    return {};
}

} // namespace nundertale
