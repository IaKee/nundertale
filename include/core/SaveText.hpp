#pragma once

#include <algorithm>
#include <string>
#include <vector>

#include "core/GameContext.hpp"

namespace nundertale {

inline std::string savePlaceLine(const std::string& roomId) {
    if (roomId == "ruins_fall" || roomId.empty()) {
        return "* The scent of lukewarm energy drinks and stale Doritos fills you with DETERMINATION.";
    }
    if (roomId == "ruins_entrance") {
        return "* The distant hum of dial-up internet fills you with DETERMINATION.";
    }
    if (roomId == "ruins_hall") {
        return "* The distant hum of dial-up internet fills you with DETERMINATION.";
    }
    if (roomId == "ruins_exit") {
        return "* The cold air and the faint smell of ozone fill you with DETERMINATION.";
    }
    if (roomId == "snowdin_entrance") {
        return "* The crunch of styrofoam snow fills you with DETERMINATION.";
    }
    if (roomId == "snowdin_town") {
        return "* The sight of 47 people sharing one Wi-Fi password on a fridge fills you with DETERMINATION.";
    }
    if (roomId == "snowdin_fog") {
        return "* The hum of the town's single shared router fills you with DETERMINATION.";
    }
    if (roomId == "waterfall_entrance") {
        return "* The sound of dripping and a distant \"rawr\" fill you with DETERMINATION.";
    }
    if (roomId == "waterfall_arena") {
        return "* The humidity of a thousand fursuits fills you with DETERMINATION.";
    }
    if (roomId == "hotland_quiz") {
        return "* The sound of 400 server fans fills you with DETERMINATION.";
    }
    if (roomId == "newhome_throne") {
        return "* The smell of herbal tea and old carpet fills you with DETERMINATION.";
    }
    if (roomId == "con_center_hall") {
        return "* The terrified hugging of a hundred furries fills you with DETERMINATION.";
    }
    return "* The scent of lukewarm energy drinks and stale Doritos fills you with DETERMINATION.";
}

inline std::vector<std::string> saveLines(const std::string& roomId, int furValue, bool toxicRoute) {
    if (toxicRoute) {
        return { "* Determination." };
    }
    std::vector<std::string> lines;
    lines.push_back(savePlaceLine(roomId));
    if (furValue > 0) {
        lines.push_back("* DETERMINATION can't fix fluff.");
    }
    return lines;
}

// SAV-007 (LORE §8.8, §8.6): what a save point does. Pure result, nothing is applied here.
// FUR is never touched. On the Furry route (`route.furry`) the heal goes to FLUFF
// (`fluff.value`, max = max HP + 10 per §8.6) instead of HP, and FUR no longer exists there.
struct SaveHealResult {
    bool furryRoute = false;
    int hpAfter = 0;
    int fluffAfter = 0;
    int furAfter = 0;
    std::vector<std::string> lines;
};

inline int fluffMax(const GameState& state) {
    return state.player.maxHp + 10;
}

// Call site (not wired yet): OverworldScene::currentSaveLines / the SavePoint interaction, which
// should apply hpAfter (or fluffAfter via setFlag("fluff.value", ...)) and show `lines` in order.
inline SaveHealResult saveHealResult(const GameState& state, const std::string& placeLineOverride = "") {
    SaveHealResult result;
    result.furryRoute = state.getFlag("route.furry", 0) != 0;
    result.furAfter = state.furry.value;
    result.hpAfter = state.player.hp;
    result.fluffAfter = state.getFlag("fluff.value", 0);
    if (result.furryRoute) {
        result.fluffAfter = fluffMax(state);
    } else {
        result.hpAfter = state.player.maxHp;
    }
    if (state.getRoute() == GameState::RouteType::ToxicGamer) {
        result.lines = { "* Determination." };
        return result;
    }
    result.lines.push_back(placeLineOverride.empty() ? savePlaceLine(state.currentRoom) : placeLineOverride);
    if (state.furry.value > 0 && !result.furryRoute) {
        result.lines.push_back("* DETERMINATION can't fix fluff.");
    }
    return result;
}

// BUG-43 / LORE §23.1: pure WELCOME HOME retry helpers. A retry is built from a
// fresh GameState so flags acquired after the save cannot leak through SaveService's
// field-by-field loader. With no save, the selected identity survives but all story,
// combat, room, HP and FUR state returns to a clean R1 game.
inline bool isFloofyFight(const std::string& encounterOrEnemyId) {
    return encounterOrEnemyId == "floofy" || encounterOrEnemyId == "floofy_bass";
}

inline std::string floofyWelcomeHomeLine(const std::string& playerName) {
    return "Floofy? ...Oh dear. Stay determined, " + playerName +
           ". And stay away from his petals.";
}

inline GameState cleanWelcomeHomeRestart(const GameState& defeated) {
    GameState clean;
    clean.player.name = defeated.player.name;
    clean.player.isChad = defeated.player.isChad;
    clean.player.isSkyrim = defeated.player.isSkyrim;
    clean.player.isWaifuFan = defeated.player.isWaifuFan;
    return clean;
}

inline void applyWelcomeHomeRetryState(GameState& state,
                                       const std::vector<std::string>& reArmFlags,
                                       int floofyDeathsAfter = -1) {
    for (const std::string& flag : reArmFlags) {
        if (!flag.empty()) state.flags.erase(flag);
    }
    if (floofyDeathsAfter >= 0) {
        state.setFlag("ruins.floofy.deaths", floofyDeathsAfter);
    }
}

inline GameState buildWelcomeHomeRetryState(const GameState& defeated,
                                            const GameState* saved,
                                            const std::vector<std::string>& reArmFlags,
                                            bool countFloofyDeath) {
    GameState retry = saved != nullptr ? *saved : cleanWelcomeHomeRestart(defeated);
    int deathsAfter = -1;
    if (countFloofyDeath) {
        const int savedDeaths = saved != nullptr
            ? saved->getFlag("ruins.floofy.deaths", 0)
            : 0;
        deathsAfter = std::max(defeated.getFlag("ruins.floofy.deaths", 0), savedDeaths) + 1;
    }
    applyWelcomeHomeRetryState(retry, reArmFlags, deathsAfter);
    return retry;
}

} // namespace nundertale
