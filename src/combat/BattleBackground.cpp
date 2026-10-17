#include "combat/BattleBackground.hpp"
#include "core/Constants.hpp"
#include <raylib.h>
#include <cmath>

namespace nundertale {

namespace {

void drawGridLines(Color color) {
    // Horizontal 1px lines (BUI-002 area y: 9..244)
    DrawLine(15, 9, 623, 9, color);
    DrawLine(15, 126, 623, 126, color);
    DrawLine(15, 244, 623, 244, color);

    // Vertical 1px lines
    const int vertX[7] = { 15, 115, 218, 317, 420, 523, 623 };
    for (int vx : vertX) {
        DrawLine(vx, 9, vx, 244, color);
    }
}

void drawTrophyShape(int centerX, int topY, Color color) {
    // Cup: 8x6 rect
    DrawRectangle(centerX - 4, topY, 8, 6, color);
    // Stem: 2x2 rect
    DrawRectangle(centerX - 1, topY + 6, 2, 2, color);
    // Small base: 6x2 rect
    DrawRectangle(centerX - 3, topY + 8, 6, 2, color);
}

} // namespace

void drawBattleBackground(BattleBackground bg, float animTime) {
    switch (bg) {
        case BattleBackground::Grid: {
            drawGridLines(GRID_GREEN);
            break;
        }

        case BattleBackground::GridTube: {
            drawGridLines(GRID_GREEN);

            // Fluorescent tube: ~200x6 centered near y=14 (canvas center X=320 -> X=220, Y=11)
            // Subtle flicker: alpha 200-255 changing every few frames, deterministic, no RNG
            int frame = static_cast<int>(animTime * 30.0f);
            int cycle = (frame / 3) % 8;
            static constexpr unsigned char kTubeAlphas[8] = {
                250, 228, 255, 212, 246, 255, 220, 238
            };
            unsigned char alpha = kTubeAlphas[cycle];
            Color tubeColor = { 245, 248, 242, alpha };
            DrawRectangle(220, 11, 200, 6, tubeColor);
            break;
        }

        case BattleBackground::RedGrid: {
            // Inverse of the green grid: red { 180, 0, 0, 255 }
            Color redGridColor = { 180, 0, 0, 255 };
            drawGridLines(redGridColor);
            break;
        }

        case BattleBackground::Black: {
            // Nothing rendered behind enemy
            break;
        }

        case BattleBackground::SpeedLines: {
            // ~12 short horizontal white lines near left/right edges scrolling horizontally
            // 6 lines on the left edge (x 20..190), 6 lines on the right edge (x 450..620)
            const float span = 170.0f;
            for (int i = 0; i < 6; ++i) {
                // Left side lines
                int leftY = 25 + i * 36;
                float leftSpeed = 140.0f + static_cast<float>(i * 35);
                float leftLen = 35.0f + static_cast<float>((i % 3) * 15);
                float leftX = 20.0f + std::fmod(static_cast<float>(i * 47) + animTime * leftSpeed, span);
                DrawRectangle(static_cast<int>(leftX), leftY, static_cast<int>(leftLen), 2, WHITE);

                // Right side lines
                int rightY = 35 + i * 36;
                float rightSpeed = 160.0f + static_cast<float>(i * 30);
                float rightLen = 30.0f + static_cast<float>((i % 4) * 12);
                float rightX = 450.0f + std::fmod(static_cast<float>(i * 53) + animTime * rightSpeed, span);
                DrawRectangle(static_cast<int>(rightX), rightY, static_cast<int>(rightLen), 2, WHITE);
            }
            break;
        }

        case BattleBackground::Trophies: {
            // Black + row of small gold trophy shapes along top and side borders
            Color gold = { 230, 180, 34, 255 };

            // Top border row
            for (int x = 35; x <= 605; x += 38) {
                drawTrophyShape(x, 14, gold);
            }

            // Left and right side borders
            for (int y = 45; y <= 230; y += 32) {
                drawTrophyShape(25, y, gold);
                drawTrophyShape(615, y, gold);
            }
            break;
        }

        case BattleBackground::LobbyGold: {
            // Black + soft gold glow band (a few stacked translucent rects) behind enemy area
            DrawRectangle(110, 85, 420, 120, Color{ 230, 180, 34, 18 });
            DrawRectangle(160, 100, 320, 90, Color{ 230, 180, 34, 28 });
            DrawRectangle(210, 115, 220, 60, Color{ 230, 180, 34, 38 });
            DrawRectangle(250, 128, 140, 34, Color{ 230, 180, 34, 48 });
            break;
        }
    }
}

} // namespace nundertale
