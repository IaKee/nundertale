#pragma once

#include <algorithm>
#include <string>
#include <utility>

namespace nundertale {

enum class RoomTransitionEvent {
    None,
    SwapNow,
    Done
};

class RoomTransition {
public:
    static constexpr int FADE_FRAMES = 8;
    static constexpr float FRAME_SECONDS = 1.0f / 30.0f;

    void start(std::string targetRoom, std::string targetSpawn, std::string facing = {}) {
        m_targetRoom = std::move(targetRoom);
        m_targetSpawn = targetSpawn.empty() ? "default" : std::move(targetSpawn);
        m_facing = std::move(facing);
        m_alpha = 0.0f;
        m_elapsed = 0.0f;
        m_phase = Phase::FadingOut;
    }

    RoomTransitionEvent update(float dt = FRAME_SECONDS) {
        if (m_phase == Phase::Idle) return RoomTransitionEvent::None;

        const float duration = static_cast<float>(FADE_FRAMES) * FRAME_SECONDS;
        m_elapsed += std::max(0.0f, dt);
        if (m_phase == Phase::FadingOut) {
            m_alpha = std::clamp(m_elapsed / duration, 0.0f, 1.0f);
            if (m_elapsed + 0.000001f >= duration) {
                m_alpha = 1.0f;
                m_elapsed = 0.0f;
                m_phase = Phase::FadingIn;
                return RoomTransitionEvent::SwapNow;
            }
            return RoomTransitionEvent::None;
        }

        m_alpha = std::clamp(1.0f - m_elapsed / duration, 0.0f, 1.0f);
        if (m_elapsed + 0.000001f >= duration) {
            m_alpha = 0.0f;
            m_elapsed = 0.0f;
            m_phase = Phase::Idle;
            return RoomTransitionEvent::Done;
        }
        return RoomTransitionEvent::None;
    }

    bool active() const { return m_phase != Phase::Idle; }
    float alpha() const { return m_alpha; }
    const std::string& targetRoom() const { return m_targetRoom; }
    const std::string& targetSpawn() const { return m_targetSpawn; }
    const std::string& facing() const { return m_facing; }

private:
    enum class Phase {
        Idle,
        FadingOut,
        FadingIn
    };

    Phase m_phase = Phase::Idle;
    std::string m_targetRoom;
    std::string m_targetSpawn = "default";
    std::string m_facing;
    float m_alpha = 0.0f;
    float m_elapsed = 0.0f;
};

} // namespace nundertale
