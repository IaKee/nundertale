#include "core/SceneManager.hpp"
#include "core/GameContext.hpp"
#include "core/Constants.hpp"
#include <raylib.h>

namespace nundertale {

SceneManager::SceneManager() = default;

void SceneManager::changeScene(std::unique_ptr<IScene> newScene, float fadeDuration) {
    m_pendingScene = std::move(newScene);
    m_fadeDuration = (fadeDuration <= 0.0f) ? 0.001f : fadeDuration;
    m_fadeTimer = 0.0f;
    m_fadeState = FadeState::FadeOut;
}

void SceneManager::pushScene(std::unique_ptr<IScene> scene) {
    m_scenes.push_back(std::move(scene));
}

void SceneManager::popScene() {
    if (!m_scenes.empty()) {
        m_scenes.pop_back();
    }
}

void SceneManager::update(float dt, GameContext& context) {
    if (m_fadeState == FadeState::FadeOut) {
        m_fadeTimer += dt;
        m_fadeAlpha = m_fadeTimer / m_fadeDuration;
        if (m_fadeAlpha >= 1.0f) {
            m_fadeAlpha = 1.0f;
            if (!m_scenes.empty()) {
                m_scenes.back()->onExit(context);
                m_scenes.pop_back();
            }
            if (m_pendingScene) {
                m_scenes.push_back(std::move(m_pendingScene));
                m_scenes.back()->onEnter(context);
            }
            m_fadeState = FadeState::FadeIn;
            m_fadeTimer = m_fadeDuration;
        }
        return;
    } else if (m_fadeState == FadeState::FadeIn) {
        m_fadeTimer -= dt;
        m_fadeAlpha = m_fadeTimer / m_fadeDuration;
        if (m_fadeAlpha <= 0.0f) {
            m_fadeAlpha = 0.0f;
            m_fadeState = FadeState::None;
        }
    }

    if (!m_scenes.empty()) {
        m_scenes.back()->update(dt, context);
    }
}

void SceneManager::render(GameContext& context) {
    if (!m_scenes.empty()) {
        m_scenes.back()->render(context);
    }

    if (m_fadeAlpha > 0.0f) {
        unsigned char a = static_cast<unsigned char>(m_fadeAlpha * 255.0f);
        DrawRectangle(0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT, { 0, 0, 0, a });
    }
}

bool SceneManager::hasScenes() const {
    return !m_scenes.empty();
}

} // namespace nundertale
