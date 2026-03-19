#pragma once

#include <string>

namespace nundertale {

enum class BattleBackground {
    Grid,
    GridTube,
    RedGrid,
    Black,
    SpeedLines,
    Trophies,
    LobbyGold
};

inline BattleBackground backgroundFromId(const std::string& id) {
    if (id == "grid") return BattleBackground::Grid;
    if (id == "grid_tube") return BattleBackground::GridTube;
    if (id == "red_grid") return BattleBackground::RedGrid;
    if (id == "black") return BattleBackground::Black;
    if (id == "speed_lines") return BattleBackground::SpeedLines;
    if (id == "trophies") return BattleBackground::Trophies;
    if (id == "lobby_gold") return BattleBackground::LobbyGold;
    return BattleBackground::Grid;
}

void drawBattleBackground(BattleBackground bg, float animTime);

} // namespace nundertale
