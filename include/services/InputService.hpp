#pragma once

#include "IInputService.hpp"

namespace nundertale {

class InputService : public IInputService {
public:
    InputService() = default;
    ~InputService() override = default;

    void update() override;

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
};

} // namespace nundertale
