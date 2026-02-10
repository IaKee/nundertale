#include "services/InputService.hpp"

namespace nundertale {

void InputService::update() {
    m_lastCharPressed = GetCharPressed();
}

bool InputService::isUp() const {
    return IsKeyDown(KEY_UP) || IsKeyDown(KEY_W);
}

bool InputService::isDown() const {
    return IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S);
}

bool InputService::isLeft() const {
    return IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A);
}

bool InputService::isRight() const {
    return IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D);
}

bool InputService::isUpPressed() const {
    return IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W);
}

bool InputService::isDownPressed() const {
    return IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S);
}

bool InputService::isLeftPressed() const {
    return IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A);
}

bool InputService::isRightPressed() const {
    return IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D);
}

bool InputService::isConfirmPressed() const {
    return IsKeyPressed(KEY_Z) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE);
}

bool InputService::isCancelPressed() const {
    return IsKeyPressed(KEY_X) || IsKeyPressed(KEY_LEFT_SHIFT) || IsKeyPressed(KEY_RIGHT_SHIFT);
}

bool InputService::isMenuPressed() const {
    return IsKeyPressed(KEY_C) || IsKeyPressed(KEY_LEFT_CONTROL) || IsKeyPressed(KEY_RIGHT_CONTROL);
}

bool InputService::isEscapePressed() const {
    return IsKeyPressed(KEY_ESCAPE);
}

bool InputService::isBackspacePressed() const {
    return IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_X);
}

bool InputService::isQuitRequested() const {
    return WindowShouldClose();
}

int InputService::getCharPressed() const {
    return m_lastCharPressed;
}

} // namespace nundertale
