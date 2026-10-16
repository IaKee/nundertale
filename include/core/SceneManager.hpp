#pragma once

#include "IScene.hpp"
#include <memory>
#include <vector>
#include <string>

namespace nundertale {

struct GameContext;

enum class FadeState {
    None,
    FadeOut,
    FadeIn
};

class SceneManager {
public:
    SceneManager();
    ~SceneManager() = default;

    // Transition with fade (replaces active scene)
    void changeScene(std::unique_ptr<IScene> newScene, float fadeDuration = 0.4f);

    // Push scene to stack with lifecycle management (BUG-01, CORE-002)
    void pushScene(std::unique_ptr<IScene> scene, GameContext& context);

    // Pop active scene and resume previous scene (CORE-002)
    void popScene(GameContext& context);

    void update(float dt, GameContext& context);
    void render(GameContext& context);

    bool hasScenes() const;

private:
    std::vector<std::unique_ptr<IScene>> m_scenes;
    std::unique_ptr<IScene> m_pendingScene;

    FadeState m_fadeState = FadeState::None;
    float m_fadeAlpha = 0.0f;
    float m_fadeDuration = 0.4f;
    float m_fadeTimer = 0.0f;
};

} // namespace nundertale
