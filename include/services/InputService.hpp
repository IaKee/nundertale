#pragma once

#include "IInputService.hpp"
#include "core/PressLatch.hpp"

namespace nundertale {

class InputService : public IInputService {
public:
    InputService() = default;
    ~InputService() override = default;

    void update() override;
    void endTick() override;

    bool isUp() const override;
    bool isDown() const override;
    bool isLeft() const override;
    bool isRight() const override;

    bool isUpPressed() const override;
    bool isDownPressed() const override;
    bool isLeftPressed() const override;
    bool isRightPressed() const override;

    bool isConfirmPressed() const override;
    bool isCancelPressed() const override;
    bool isMenuPressed() const override;
    bool isEscapePressed() const override;
    bool isBackspacePressed() const override;
    bool isQuitRequested() const override;

    int getCharPressed() const override;

private:
    int m_lastCharPressed = 0;

    // Reading a press uses it up, so the latches change inside the const getters
    mutable PressLatch m_up;
    mutable PressLatch m_down;
    mutable PressLatch m_left;
    mutable PressLatch m_right;
    mutable PressLatch m_confirm;
    mutable PressLatch m_cancel;
    mutable PressLatch m_menu;
    mutable PressLatch m_escape;
    mutable PressLatch m_backspace;
};

} // namespace nundertale
