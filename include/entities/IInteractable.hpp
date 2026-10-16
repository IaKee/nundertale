#pragma once

#include <raylib.h>
#include <string>

namespace nundertale {

struct GameContext;

class IInteractable {
public:
    virtual ~IInteractable() = default;

    virtual Rectangle getBounds() const = 0;
    virtual void interact(GameContext& context) = 0;
    virtual void render(GameContext& context) = 0;
};

} // namespace nundertale
