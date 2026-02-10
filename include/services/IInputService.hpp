#pragma once

#include <raylib.h>

namespace nundertale {

class IInputService {
public:
    virtual ~IInputService() = default;

    virtual void update() = 0;

    // Movement
    virtual bool isUp() const = 0;
    virtual bool isDown() const = 0;
    virtual bool isLeft() const = 0;
    virtual bool isRight() const = 0;

    virtual bool isUpPressed() const = 0;
    virtual bool isDownPressed() const = 0;
    virtual bool isLeftPressed() const = 0;
    virtual bool isRightPressed() const = 0;

    // Actions
    virtual bool isConfirmPressed() const = 0; // Z / Enter / Space
    virtual bool isCancelPressed() const = 0;  // X / Shift
    virtual bool isMenuPressed() const = 0;    // C / Ctrl
    virtual bool isEscapePressed() const = 0;  // Esc
    virtual bool isBackspacePressed() const = 0; // Backspace / X
    virtual bool isQuitRequested() const = 0;  // Window close or explicit quit

    virtual int getCharPressed() const = 0;    // Any alphanumeric character pressed
};

} // namespace nundertale
