#pragma once

#include <raylib.h>
#include <string>

namespace nundertale {

// Virtual game resolution (Undertale standard 4:3)
constexpr int VIRTUAL_WIDTH = 640;
constexpr int VIRTUAL_HEIGHT = 480;

// Target Frame Rate (Undertale runs at 30 fps for authentic timings and pixel movement)
constexpr int TARGET_FPS = 30;
constexpr float LOGIC_TIMESTEP = 1.0f / 30.0f;

// Window initial size
constexpr int WINDOW_WIDTH = 960;
constexpr int WINDOW_HEIGHT = 720;
constexpr const char* WINDOW_TITLE = "nUndertale";

// Official Undertale Colors (GFX-001)
const Color UI_ORANGE          = { 255, 127,  39, 255 }; // #FF7F27 (Button normal)
const Color UI_YELLOW          = { 255, 255,   0, 255 }; // #FFFF00 (Button selected / spareable)
const Color SOUL_RED           = { 255,   0,   0, 255 }; // #FF0000 (Pepe / standard soul)
const Color HP_RED             = { 255,   0,   0, 255 }; // #FF0000 (HP bar background)
const Color HP_YELLOW          = { 255, 255,   0, 255 }; // #FFFF00 (HP bar fill)
const Color ENEMY_HP_GREEN     = {   0, 255,   0, 255 }; // #00FF00 (Enemy damage HP bar)
const Color GRID_GREEN         = {  39, 175,  75, 255 }; // #27AF4B (Battle background grid lines)
const Color MISS_GRAY          = { 192, 192, 192, 255 }; // #C0C0C0 ("MISS" damage text)
const Color SOUL_BLUE          = {   0,  38, 255, 255 }; // #0026FF (Gravity mode)
const Color SOUL_GREEN         = {   0, 192,   0, 255 }; // #00C000 (Shield mode)
const Color SOUL_YELLOW        = { 255, 255,   0, 255 }; // #FFFF00 (Shooter mode)
const Color SOUL_PURPLE        = { 213,  53, 217, 255 }; // #D535D9 (Muffet web strings mode)
const Color KR_PURPLE          = { 255,   0, 255, 255 }; // #FF00FF (Karma / Furry overlay on HP)

// Legacy alias colors for backward compatibility
const Color COLOR_UNDERTALE_RED    = SOUL_RED;
const Color COLOR_UNDERTALE_YELLOW = UI_YELLOW;
const Color COLOR_UNDERTALE_ORANGE = UI_ORANGE;
const Color COLOR_UNDERTALE_GREEN  = { 0, 255, 0, 255 };
const Color COLOR_UNDERTALE_CYAN   = { 0, 255, 255, 255 };
const Color COLOR_UNDERTALE_WHITE  = { 255, 255, 255, 255 };
const Color COLOR_UNDERTALE_GRAY   = { 128, 128, 128, 255 };
const Color COLOR_UNDERTALE_DARK   = { 20, 20, 20, 255 };
const Color COLOR_UNDERTALE_BLACK  = { 0, 0, 0, 255 };

// Player default configuration
constexpr float PLAYER_SPEED = 180.0f; // 6 px/frame at 30fps in 2x canvas (OVW-006)
constexpr int PLAYER_DEFAULT_MAX_HP = 20;
constexpr int SOUL_SPEED = 4; // 4 px/frame at 30fps (BHL-006)

// Battle geometry (Section 2.1 & BUI-001)
// Battle Box default (Text / Menu state)
constexpr int BATTLE_BOX_X = 32;
constexpr int BATTLE_BOX_Y = 250;
constexpr int BATTLE_BOX_W = 575; // outer right: 606 (32 + 575 - 1 = 606)
constexpr int BATTLE_BOX_H = 140; // outer bottom: 389 (250 + 140 - 1 = 389)
constexpr int BATTLE_BOX_BORDER = 5;

// Battle Buttons (BUI-006)
constexpr int BATTLE_BUTTON_Y = 432;
constexpr int BATTLE_BUTTON_W = 110;
constexpr int BATTLE_BUTTON_H = 42;
constexpr int BATTLE_BUTTON_BORDER = 2;
constexpr int BATTLE_BUTTON_X_FIGHT = 32;
constexpr int BATTLE_BUTTON_X_ACT   = 185;
constexpr int BATTLE_BUTTON_X_ITEM  = 345;
constexpr int BATTLE_BUTTON_X_MERCY = 500;

// Enemy baseline y (feet at y ≈ 241, BUI-003)
constexpr int ENEMY_BASELINE_Y = 241;

// HUD geometry (BUI-005)
constexpr int HUD_Y = 400;
constexpr int HUD_NAME_X = 30;
constexpr int HUD_LV_X = 132;
constexpr int HUD_HP_LABEL_X = 244;
constexpr int HUD_HP_BAR_X = 275;
constexpr int HUD_HP_BAR_H = 20;

} // namespace nundertale
