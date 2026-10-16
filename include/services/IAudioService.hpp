#pragma once

#include <string>

namespace nundertale {

class IAudioService {
public:
    virtual ~IAudioService() = default;

    virtual void playMusic(const std::string& path, bool loop = true) = 0;
    virtual void stopMusic() = 0;
    virtual void pauseMusic() = 0;
    virtual void resumeMusic() = 0;
    virtual void updateMusic() = 0;
    virtual void setMusicVolume(float volume) = 0;

    virtual void playSound(const std::string& path, float volume = 1.0f) = 0;
    virtual void stopAllSounds() = 0;

    virtual void setMasterVolume(float volume) = 0;
};

} // namespace nundertale
