#pragma once

#include "IAudioService.hpp"
#include <raylib.h>
#include <unordered_map>
#include <string>

namespace nundertale {

class AudioService : public IAudioService {
public:
    AudioService();
    ~AudioService() override;

    void playMusic(const std::string& path, bool loop = true) override;
    void stopMusic() override;
    void pauseMusic() override;
    void resumeMusic() override;
    void updateMusic() override;
    void setMusicVolume(float volume) override;

    void playSound(const std::string& path, float volume = 1.0f) override;
    void stopAllSounds() override;

    void setMasterVolume(float volume) override;

private:
    std::unordered_map<std::string, Sound> m_soundCache;
    Music m_currentMusic{};
    std::string m_currentMusicPath;
    bool m_hasActiveMusic = false;
    float m_musicVolume = 0.7f;
    float m_masterVolume = 1.0f;
};

} // namespace nundertale
