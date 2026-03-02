#include "services/InputService.hpp"

namespace nundertale {

void InputService::update() {
    m_lastCharPressed = GetCharPressed();

    bool gpUpPressed = false, gpDownPressed = false, gpLeftPressed = false, gpRightPressed = false;
    bool gpConfirmPressed = false, gpCancelPressed = false, gpMenuPressed = false, gpEscapePressed = false;

    if (IsGamepadAvailable(0)) {
        float stickX = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
        float stickY = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y);

        gpUpPressed = IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_UP) || (stickY < -0.6f && !IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_UP));
        gpDownPressed = IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN) || (stickY > 0.6f && !IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN));
        gpLeftPressed = IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT) || (stickX < -0.6f && !IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT));
        gpRightPressed = IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT) || (stickX > 0.6f && !IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT));

        gpConfirmPressed = IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN); // A
        gpCancelPressed = IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_LEFT); // B or X
        gpMenuPressed = IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_UP) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_LEFT); // Y or Select
        gpEscapePressed = IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT); // Start
    }

    // Accumulate presses between logic ticks (CORE-009)
    m_upPressed |= IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W) || gpUpPressed;
    m_downPressed |= IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S) || gpDownPressed;
    m_leftPressed |= IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A) || gpLeftPressed;
    m_rightPressed |= IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D) || gpRightPressed;

    m_confirmPressed |= IsKeyPressed(KEY_Z) || IsKeyPressed(KEY_ENTER) || gpConfirmPressed;
    m_cancelPressed |= IsKeyPressed(KEY_X) || IsKeyPressed(KEY_LEFT_SHIFT) || IsKeyPressed(KEY_RIGHT_SHIFT) || gpCancelPressed;
    m_menuPressed |= IsKeyPressed(KEY_C) || IsKeyPressed(KEY_LEFT_CONTROL) || IsKeyPressed(KEY_RIGHT_CONTROL) || gpMenuPressed;
    m_escapePressed |= IsKeyPressed(KEY_ESCAPE) || gpEscapePressed;
    m_backspacePressed |= IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_X) || gpCancelPressed;
}

bool InputService::isUp() const {
    bool gp = IsGamepadAvailable(0) && (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_UP) || GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) < -0.4f);
    return IsKeyDown(KEY_UP) || IsKeyDown(KEY_W) || gp;
}

bool InputService::isDown() const {
    bool gp = IsGamepadAvailable(0) && (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN) || GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) > 0.4f);
    return IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S) || gp;
}

bool InputService::isLeft() const {
    bool gp = IsGamepadAvailable(0) && (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT) || GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) < -0.4f);
    return IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A) || gp;
}

bool InputService::isRight() const {
    bool gp = IsGamepadAvailable(0) && (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT) || GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) > 0.4f);
    return IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D) || gp;
}

bool InputService::isUpPressed() const {
    bool r = m_upPressed;
    m_upPressed = false;
    return r;
}

bool InputService::isDownPressed() const {
    bool r = m_downPressed;
    m_downPressed = false;
    return r;
}

bool InputService::isLeftPressed() const {
    bool r = m_leftPressed;
    m_leftPressed = false;
    return r;
}

bool InputService::isRightPressed() const {
    bool r = m_rightPressed;
    m_rightPressed = false;
    return r;
}

bool InputService::isConfirmPressed() const {
    bool r = m_confirmPressed;
    m_confirmPressed = false;
    return r;
}

bool InputService::isCancelPressed() const {
    bool r = m_cancelPressed;
    m_cancelPressed = false;
    return r;
}

bool InputService::isMenuPressed() const {
    bool r = m_menuPressed;
    m_menuPressed = false;
    return r;
}

bool InputService::isEscapePressed() const {
    bool r = m_escapePressed;
    m_escapePressed = false;
    return r;
}

bool InputService::isBackspacePressed() const {
    bool r = m_backspacePressed;
    m_backspacePressed = false;
    return r;
}

bool InputService::isQuitRequested() const {
    return WindowShouldClose();
}

int InputService::getCharPressed() const {
    return m_lastCharPressed;
}

} // namespace nundertale
