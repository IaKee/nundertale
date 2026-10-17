#include "entities/Interactable.hpp"
#include "core/GameContext.hpp"
#include <raylib.h>

namespace nundertale {

SavePoint::SavePoint(Vector2 position)
    : m_position(position) {}

Rectangle SavePoint::getBounds() const {
    return { m_position.x - 10, m_position.y - 10, 20, 20 };
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
    // Undertale save star: 4-pointed star
    Color starColor = m_frame ? Color{ 255, 255, 0, 255 } : Color{ 255, 255, 180, 255 };
    float s = m_frame ? 9.0f : 7.0f;

    DrawRectangle((int)(m_position.x - s / 2), (int)(m_position.y - s / 2), (int)s, (int)s, starColor);
    DrawRectangle((int)(m_position.x - 2), (int)(m_position.y - s - 2), 4, (int)(s * 2 + 4), starColor);
    DrawRectangle((int)(m_position.x - s - 2), (int)(m_position.y - 2), (int)(s * 2 + 4), 4, starColor);
}

} // namespace nundertale
