#pragma once

#include "IInputService.hpp"

#include <array>
#include <string>
#include <vector>

namespace nundertale {

class ScriptedInputService : public IInputService {
public:
    explicit ScriptedInputService(const std::string& path);

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
    enum class Key : std::size_t {
        Confirm,
        Cancel,
        Menu,
        Up,
        Down,
        Left,
        Right,
        Count
    };

    struct Event {
        int firstTick = 0;
        int lastTick = 0;
        Key key = Key::Confirm;
    };

    bool isHeld(Key key) const;
    bool consumePress(Key key) const;

    std::vector<Event> m_events;
    int m_tick = 0;
    mutable std::array<bool, static_cast<std::size_t>(Key::Count)> m_consumed{};
};

} // namespace nundertale
