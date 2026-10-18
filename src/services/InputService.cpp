#include "services/InputService.hpp"
#include <initializer_list>

namespace nundertale {

namespace {

bool anyKeyDown(std::initializer_list<int> keys) {
    for (int key : keys) {
        if (IsKeyDown(key)) return true;
    }
    return false;
}

bool anyKeyPressed(std::initializer_list<int> keys) {
    for (int key : keys) {
        if (IsKeyPressed(key)) return true;
    }
    return false;
}

bool padDown(std::initializer_list<int> buttons) {
    if (!IsGamepadAvailable(0)) return false;
    for (int button : buttons) {
        if (IsGamepadButtonDown(0, button)) return true;
    }
    return false;
}

bool padPressed(std::initializer_list<int> buttons) {
    if (!IsGamepadAvailable(0)) return false;
    for (int button : buttons) {
        if (IsGamepadButtonPressed(0, button)) return true;
    }
    return false;
}

float padAxis(int axis) {
    return IsGamepadAvailable(0) ? GetGamepadAxisMovement(0, axis) : 0.0f;
}

} // namespace

void InputService::update() {
    // Typed letters (name screen): ignore the OS key repeat, so holding a letter types it once
    const bool newKeyThisFrame = GetKeyPressed() != 0;
    while (GetKeyPressed() != 0) {}
    m_lastCharPressed = 0;
    for (int c = GetCharPressed(); c != 0; c = GetCharPressed()) {
        if (newKeyThisFrame && m_lastCharPressed == 0) {
            m_lastCharPressed = c;
        }
    }

    // Every action is edge-triggered through a PressLatch (BUG-33); the stick counts as a held
    // d-pad direction, so pushing and holding it moves the cursor once.
    const float stickX = padAxis(GAMEPAD_AXIS_LEFT_X);
    const float stickY = padAxis(GAMEPAD_AXIS_LEFT_Y);

    m_up.feed(anyKeyDown({ KEY_UP, KEY_W }) || padDown({ GAMEPAD_BUTTON_LEFT_FACE_UP }) || stickY < -0.6f,
              anyKeyPressed({ KEY_UP, KEY_W }) || padPressed({ GAMEPAD_BUTTON_LEFT_FACE_UP }));
    m_down.feed(anyKeyDown({ KEY_DOWN, KEY_S }) || padDown({ GAMEPAD_BUTTON_LEFT_FACE_DOWN }) || stickY > 0.6f,
                anyKeyPressed({ KEY_DOWN, KEY_S }) || padPressed({ GAMEPAD_BUTTON_LEFT_FACE_DOWN }));
    m_left.feed(anyKeyDown({ KEY_LEFT, KEY_A }) || padDown({ GAMEPAD_BUTTON_LEFT_FACE_LEFT }) || stickX < -0.6f,
                anyKeyPressed({ KEY_LEFT, KEY_A }) || padPressed({ GAMEPAD_BUTTON_LEFT_FACE_LEFT }));
    m_right.feed(anyKeyDown({ KEY_RIGHT, KEY_D }) || padDown({ GAMEPAD_BUTTON_LEFT_FACE_RIGHT }) || stickX > 0.6f,
                 anyKeyPressed({ KEY_RIGHT, KEY_D }) || padPressed({ GAMEPAD_BUTTON_LEFT_FACE_RIGHT }));

    const std::initializer_list<int> cancelPad = { GAMEPAD_BUTTON_RIGHT_FACE_RIGHT, GAMEPAD_BUTTON_RIGHT_FACE_LEFT }; // B or X
    m_confirm.feed(anyKeyDown({ KEY_Z, KEY_ENTER }) || padDown({ GAMEPAD_BUTTON_RIGHT_FACE_DOWN }),
                   anyKeyPressed({ KEY_Z, KEY_ENTER }) || padPressed({ GAMEPAD_BUTTON_RIGHT_FACE_DOWN }));
    m_cancel.feed(anyKeyDown({ KEY_X, KEY_LEFT_SHIFT, KEY_RIGHT_SHIFT }) || padDown(cancelPad),
                  anyKeyPressed({ KEY_X, KEY_LEFT_SHIFT, KEY_RIGHT_SHIFT }) || padPressed(cancelPad));
    m_menu.feed(anyKeyDown({ KEY_C, KEY_LEFT_CONTROL, KEY_RIGHT_CONTROL }) || padDown({ GAMEPAD_BUTTON_RIGHT_FACE_UP, GAMEPAD_BUTTON_MIDDLE_LEFT }),
                anyKeyPressed({ KEY_C, KEY_LEFT_CONTROL, KEY_RIGHT_CONTROL }) || padPressed({ GAMEPAD_BUTTON_RIGHT_FACE_UP, GAMEPAD_BUTTON_MIDDLE_LEFT }));
    m_escape.feed(anyKeyDown({ KEY_ESCAPE }) || padDown({ GAMEPAD_BUTTON_MIDDLE_RIGHT }),
                  anyKeyPressed({ KEY_ESCAPE }) || padPressed({ GAMEPAD_BUTTON_MIDDLE_RIGHT }));
    m_backspace.feed(anyKeyDown({ KEY_BACKSPACE, KEY_X }) || padDown(cancelPad),
                     anyKeyPressed({ KEY_BACKSPACE, KEY_X }) || padPressed(cancelPad));
}

void InputService::endTick() {
    m_up.endTick();
    m_down.endTick();
    m_left.endTick();
    m_right.endTick();
    m_confirm.endTick();
    m_cancel.endTick();
    m_menu.endTick();
    m_escape.endTick();
    m_backspace.endTick();
}

bool InputService::isUp() const {
    return anyKeyDown({ KEY_UP, KEY_W }) || padDown({ GAMEPAD_BUTTON_LEFT_FACE_UP }) || padAxis(GAMEPAD_AXIS_LEFT_Y) < -0.4f;
}

bool InputService::isDown() const {
    return anyKeyDown({ KEY_DOWN, KEY_S }) || padDown({ GAMEPAD_BUTTON_LEFT_FACE_DOWN }) || padAxis(GAMEPAD_AXIS_LEFT_Y) > 0.4f;
}

bool InputService::isLeft() const {
    return anyKeyDown({ KEY_LEFT, KEY_A }) || padDown({ GAMEPAD_BUTTON_LEFT_FACE_LEFT }) || padAxis(GAMEPAD_AXIS_LEFT_X) < -0.4f;
}

bool InputService::isRight() const {
    return anyKeyDown({ KEY_RIGHT, KEY_D }) || padDown({ GAMEPAD_BUTTON_LEFT_FACE_RIGHT }) || padAxis(GAMEPAD_AXIS_LEFT_X) > 0.4f;
}

bool InputService::isUpPressed() const { return m_up.consume(); }
bool InputService::isDownPressed() const { return m_down.consume(); }
bool InputService::isLeftPressed() const { return m_left.consume(); }
bool InputService::isRightPressed() const { return m_right.consume(); }
bool InputService::isConfirmPressed() const { return m_confirm.consume(); }
bool InputService::isCancelPressed() const { return m_cancel.consume(); }
bool InputService::isMenuPressed() const { return m_menu.consume(); }
bool InputService::isEscapePressed() const { return m_escape.consume(); }
bool InputService::isBackspacePressed() const { return m_backspace.consume(); }

bool InputService::isQuitRequested() const {
    return WindowShouldClose();
}

int InputService::getCharPressed() const {
    return m_lastCharPressed;
}

} // namespace nundertale
