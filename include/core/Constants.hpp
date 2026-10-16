#pragma once

#include <raylib.h>
#include <string>

namespace nundertale {

// Virtual game resolution (Undertale standard 4:3)
constexpr int VIRTUAL_WIDTH = 640;
constexpr int VIRTUAL_HEIGHT = 480;

// Window initial size
constexpr int WINDOW_WIDTH = 960;
constexpr int WINDOW_HEIGHT = 720;
constexpr const char* WINDOW_TITLE = "nUndertale";

// Retro Game Colors
const Color COLOR_UNDERTALE_RED    = { 255, 0, 0, 255 };
const Color COLOR_UNDERTALE_YELLOW = { 255, 255, 0, 255 };
const Color COLOR_UNDERTALE_ORANGE = { 255, 165, 0, 255 };
const Color COLOR_UNDERTALE_GREEN  = { 0, 255, 0, 255 };
const Color COLOR_UNDERTALE_CYAN   = { 0, 255, 255, 255 };
const Color COLOR_UNDERTALE_WHITE  = { 255, 255, 255, 255 };
const Color COLOR_UNDERTALE_GRAY   = { 128, 128, 128, 255 };
const Color COLOR_UNDERTALE_DARK   = { 20, 20, 20, 255 };
const Color COLOR_UNDERTALE_BLACK  = { 0, 0, 0, 255 };

// Player default configuration
constexpr float PLAYER_SPEED = 180.0f;
constexpr int PLAYER_DEFAULT_MAX_HP = 20;

} // namespace nundertale
