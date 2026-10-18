#pragma once

#include "IInteractable.hpp"
#include <functional>

namespace nundertale {

class GenericInteractable : public IInteractable {
public:
    GenericInteractable(Rectangle bounds, std::function<void(GameContext&)> onInteract, std::function<void(GameContext&)> onRender = nullptr)
        : m_bounds(bounds), m_onInteract(onInteract), m_onRender(onRender) {}

    Rectangle getBounds() const override { return m_bounds; }
    void interact(GameContext& context) override {
        if (m_onInteract) m_onInteract(context);
    }
    void render(GameContext& context) override {
        if (m_onRender) m_onRender(context);
    }

private:
    Rectangle m_bounds;
    std::function<void(GameContext&)> m_onInteract;
    std::function<void(GameContext&)> m_onRender;
};

class SavePoint : public IInteractable {
public:
    SavePoint(Vector2 position);

    Rectangle getBounds() const override;
    void interact(GameContext& context) override;
    void render(GameContext& context) override;

    void update(float dt);

private:
    Vector2 m_position;
    float m_animTimer = 0.0f;
    float m_spriteAnimTimer = 0.0f;
    bool m_frame = false;
};

} // namespace nundertale
