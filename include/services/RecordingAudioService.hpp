#pragma once

#include "IAudioService.hpp"
#include "MusicFade.hpp"

#include <cstdint>
#include <fstream>
#include <string>

namespace nundertale {

class RecordingAudioService : public IAudioService {
public:
    explicit RecordingAudioService(const std::string& outputDirectory);

    void playMusic(const std::string& path, bool loop = true) override;
    void playMusic(const std::string& path, bool loop, float loopStart, float loopEnd) override;
    void setMusicLoopPoints(float loopStart, float loopEnd) override;
    void stopMusic() override;
    void pauseMusic() override;
    void resumeMusic() override;
    void updateMusic() override;
    float musicTime() const override;
    void seekMusic(float seconds) override;
    void setMusicVolume(float volume) override;
    void fadeOutMusic(float seconds) override;
    void fadeInMusic(const std::string& path, float seconds, bool loop = true) override;
    void setMusicPitch(float pitch) override;

    void playSound(const std::string& path, float volume = 1.0f) override;
    void stopAllSounds() override;
    void setMasterVolume(float volume) override;

private:
    static double readTrackLength(const std::string& path);
    static std::string jsonString(const std::string& value);
    float effectiveMusicVolume() const;
    void logMusicTick();

    std::ofstream m_log;
    std::string m_musicPath;
    double m_position = 0.0;
    double m_trackLength = 0.0;
    float m_musicVolume = 0.7f;
    float m_masterVolume = 1.0f;
    float m_fadeFactor = 1.0f;
    float m_pitch = 1.0f;
    float m_loopStart = 0.0f;
    float m_loopEnd = 0.0f;
    MusicFade m_fade;
    std::int64_t m_tick = 0;
    bool m_active = false;
    bool m_paused = false;
    bool m_loop = true;
    bool m_hasUpdated = false;
};

} // namespace nundertale
