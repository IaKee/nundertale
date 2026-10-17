#include "entities/Interactable.hpp"
#include "core/GameContext.hpp"
#include <raylib.h>

namespace nundertale {

SavePoint::SavePoint(Vector2 position)
    : m_position(position) {}

Rectangle SavePoint::getBounds() const {
    return { m_position.x - 10.0f, m_position.y - 18.0f, 20.0f, 33.0f };
}

void SavePoint::update(float dt) {
    m_animTimer += dt;
    if (m_animTimer >= 0.25f) {
        m_animTimer = 0.0f;
        m_frame = !m_frame;
    }
}

void SavePoint::interact(GameContext& context) {
    context.player.hp = context.player.maxHp;
    context.audio.playSound("assets/sfx/u_activate.wav", 0.8f);
}

void SavePoint::render(GameContext& /*context*/) {
    int cx = static_cast<int>(m_position.x);
    int cy = static_cast<int>(m_position.y);

    // SAV-001: Vintage soda bottle standing on the floor at the save position
    // Glass green body (~10x16 px)
    DrawRectangle(cx - 5, cy - 1, 10, 16, Color{ 60, 140, 70, 255 });
    // Darker neck (~4x6 px)
    DrawRectangle(cx - 2, cy - 7, 4, 6, Color{ 40, 100, 50, 255 });
    // Bottle cap at top of neck
    DrawRectangle(cx - 2, cy - 8, 4, 1, Color{ 35, 85, 40, 255 });
    // Cream label band
    DrawRectangle(cx - 5, cy + 5, 10, 5, Color{ 240, 235, 200, 255 });
    // 1 px highlight line (specular reflection on glass and label)
    DrawRectangle(cx - 4, cy, 1, 14, Color{ 120, 200, 130, 255 });
    DrawRectangle(cx - 4, cy + 5, 1, 5, Color{ 255, 255, 240, 255 });

    // Undertale save star: yellow 4-point star hovering ~4 px above the bottle cap
    // 2-frame animation: alternate every ~0.25 s, Frame B rotated 45° ("x"-ish diagonal variant, 2 px smaller)
    // Gentle 1 px bob: starY bobs between cy - 18 and cy - 17
    int starY = cy - 18 + (m_frame ? 1 : 0);
    Color starYellow = Color{ 255, 255, 0, 255 };
    Color starOrange = Color{ 255, 160, 0, 255 };

    if (!m_frame) {
        // Frame A: Plus-shaped star with pointed tips (~14x14 px, orange outline/core, yellow fill)
        // Orange outline base
        DrawRectangle(cx, starY - 6, 1, 13, starOrange);
        DrawRectangle(cx - 1, starY - 5, 3, 11, starOrange);
        DrawRectangle(cx - 2, starY - 3, 5, 7, starOrange);
        DrawRectangle(cx - 3, starY - 2, 7, 5, starOrange);
        DrawRectangle(cx - 5, starY - 1, 11, 3, starOrange);
        DrawRectangle(cx - 6, starY, 13, 1, starOrange);

        // Yellow fill (1 px inside)
        DrawRectangle(cx, starY - 5, 1, 11, starYellow);
        DrawRectangle(cx - 1, starY - 3, 3, 7, starYellow);
        DrawRectangle(cx - 2, starY - 2, 5, 5, starYellow);
        DrawRectangle(cx - 4, starY - 1, 9, 3, starYellow);
        DrawRectangle(cx - 5, starY, 11, 1, starYellow);

        // Orange core
        DrawRectangle(cx - 1, starY - 1, 3, 3, starOrange);
    } else {
        // Frame B: Rotated 45° "x"-ish diagonal variant (~2 px smaller, spinning look)
        // Orange outline base
        DrawRectangle(cx - 3, starY - 3, 7, 7, starOrange);
        DrawRectangle(cx - 4, starY - 4, 2, 2, starOrange);
        DrawRectangle(cx - 5, starY - 5, 1, 1, starOrange);
        DrawRectangle(cx + 3, starY - 4, 2, 2, starOrange);
        DrawRectangle(cx + 4, starY - 5, 1, 1, starOrange);
        DrawRectangle(cx - 4, starY + 3, 2, 2, starOrange);
        DrawRectangle(cx - 5, starY + 4, 1, 1, starOrange);
        DrawRectangle(cx + 3, starY + 3, 2, 2, starOrange);
        DrawRectangle(cx + 4, starY + 4, 1, 1, starOrange);

        // Yellow fill
        DrawRectangle(cx - 2, starY - 2, 5, 5, starYellow);
        DrawRectangle(cx - 3, starY - 3, 2, 2, starYellow);
        DrawRectangle(cx + 2, starY - 3, 2, 2, starYellow);
        DrawRectangle(cx - 3, starY + 2, 2, 2, starYellow);
        DrawRectangle(cx + 2, starY + 2, 2, 2, starYellow);
        DrawRectangle(cx - 4, starY - 4, 1, 1, starYellow);
        DrawRectangle(cx + 3, starY - 4, 1, 1, starYellow);
        DrawRectangle(cx - 4, starY + 3, 1, 1, starYellow);
        DrawRectangle(cx + 3, starY + 3, 1, 1, starYellow);

        // Orange core
        DrawRectangle(cx - 1, starY - 1, 3, 3, starOrange);
    }
}

} // namespace nundertale
