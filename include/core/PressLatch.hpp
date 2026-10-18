#pragma once

namespace nundertale {

// BUG-33: one press per key-down. Holding a key never presses it again (the player has to let
// go first), and a press that nobody reads during the logic tick it happened in is dropped, so it
// can't fire later in another menu or state.
class PressLatch {
public:
    // Called once per rendered frame. `down` = the key (or any key bound to the action) is held;
    // `pressedThisFrame` catches a press and release that both happened between two frames.
    void feed(bool down, bool pressedThisFrame = false) {
        if (pressedThisFrame || (down && !m_wasDown)) {
            m_pending = true;
        }
        m_wasDown = down;
    }

    // True once per press; reading it uses it up.
    bool consume() {
        const bool pressed = m_pending;
        m_pending = false;
        return pressed;
    }

    // Called after every logic tick: presses no scene asked for are forgotten.
    void endTick() { m_pending = false; }

private:
    bool m_wasDown = false;
    bool m_pending = false;
};

} // namespace nundertale
