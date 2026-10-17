#include "core/SceneManager.hpp"
#include "core/GameContext.hpp"
#include "core/Constants.hpp"
#include <raylib.h>

namespace nundertale {

SceneManager::SceneManager() = default;

void SceneManager::changeScene(std::unique_ptr<IScene> newScene, float fadeDuration) {
    if (fadeDuration <= 0.0f) {
        changeScene(std::move(newScene), cut());
    } else {
        changeScene(std::move(newScene), fadeBlack(fadeDuration));
    }
}

void SceneManager::changeScene(std::unique_ptr<IScene> newScene, std::unique_ptr<ITransition> transition) {
    m_pendingScene = std::move(newScene);
    m_transition = transition ? std::move(transition) : cut();
    m_transitionTimer = 0.0f;
    m_phase = TransitionPhase::CoveringOut;
}

void SceneManager::pushScene(std::unique_ptr<IScene> scene, GameContext& context) {
    if (!m_scenes.empty()) {
        m_scenes.back()->onPause(context);
    }
    m_scenes.push_back(std::move(scene));
    m_scenes.back()->onEnter(context);
}

void SceneManager::popScene(GameContext& context) {
    if (!m_scenes.empty()) {
        m_scenes.back()->onExit(context);
        m_scenes.pop_back();
    }
    if (!m_scenes.empty()) {
        m_scenes.back()->onResume(context);
    }
}

void SceneManager::update(float dt, GameContext& context) {
    if (m_phase == TransitionPhase::CoveringOut) {
        if (!m_transition || m_transition->duration() <= 0.0f) {
            if (!m_scenes.empty()) {
                m_scenes.back()->onExit(context);
                m_scenes.pop_back();
            }
            if (m_pendingScene) {
                m_scenes.push_back(std::move(m_pendingScene));
                m_scenes.back()->onEnter(context);
            }
            if (!m_transition || m_transition->durationIn() <= 0.0f) {
                m_phase = TransitionPhase::None;
                m_transition.reset();
            } else {
                m_phase = TransitionPhase::RevealingIn;
                m_transitionTimer = 0.0f;
            }
            return;
        }

        m_transitionTimer += dt;
        float dur = m_transition->duration();
        if (m_transitionTimer >= dur) {
            m_transitionTimer = 0.0f;
            if (!m_scenes.empty()) {
                m_scenes.back()->onExit(context);
                m_scenes.pop_back();
            }
            if (m_pendingScene) {
                m_scenes.push_back(std::move(m_pendingScene));
                m_scenes.back()->onEnter(context);
            }
            if (!m_transition || m_transition->durationIn() <= 0.0f) {
                m_phase = TransitionPhase::None;
                m_transition.reset();
            } else {
                m_phase = TransitionPhase::RevealingIn;
            }
        }
        return;
    } else if (m_phase == TransitionPhase::RevealingIn) {
        if (!m_transition || m_transition->durationIn() <= 0.0f) {
            m_phase = TransitionPhase::None;
            m_transition.reset();
        } else {
            m_transitionTimer += dt;
            float dur = m_transition->durationIn();
            if (m_transitionTimer >= dur) {
                m_phase = TransitionPhase::None;
                m_transition.reset();
                m_transitionTimer = 0.0f;
            }
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

    if (m_phase != TransitionPhase::None && m_transition) {
        bool coveringOut = (m_phase == TransitionPhase::CoveringOut);
        float dur = coveringOut ? m_transition->duration() : m_transition->durationIn();
        float progress = (dur > 0.0f) ? std::clamp(m_transitionTimer / dur, 0.0f, 1.0f) : 1.0f;
        m_transition->render(progress, coveringOut);
    }
}

bool SceneManager::hasScenes() const {
    return !m_scenes.empty();
}

} // namespace nundertale
