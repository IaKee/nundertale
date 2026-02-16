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

    // Called when another scene is pushed on top of this scene (CORE-002)
    virtual void onPause(GameContext& /*context*/) {}

    // Called when the scene on top is popped and this scene becomes active again (CORE-002)
    virtual void onResume(GameContext& /*context*/) {}
};

} // namespace nundertale
