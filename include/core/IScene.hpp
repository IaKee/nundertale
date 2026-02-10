#pragma once

namespace nundertale {

struct GameContext;

class IScene {
public:
    virtual ~IScene() = default;

    virtual void onEnter(GameContext& context) = 0;
    virtual void update(float dt, GameContext& context) = 0;
    virtual void render(GameContext& context) = 0;
    virtual void onExit(GameContext& context) = 0;
};

} // namespace nundertale
