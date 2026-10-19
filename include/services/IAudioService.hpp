#pragma once

#include <string>

namespace nundertale {

class IAudioService {
public:
    virtual ~IAudioService() = default;

    virtual void playMusic(const std::string& path, bool loop = true) = 0;
    virtual void playMusic(const std::string& path, bool loop, float loopStart, float loopEnd) = 0;
    virtual void setMusicLoopPoints(float loopStart, float loopEnd) = 0;
    virtual void stopMusic() = 0;
    virtual void pauseMusic() = 0;
    virtual void resumeMusic() = 0;
    virtual void updateMusic() = 0;
    // BSK: playback position of the current music stream in seconds (0 when nothing plays)
    virtual float musicTime() const = 0;
    // BSK: jump the current music stream to `seconds` (no-op when nothing plays)
    virtual void seekMusic(float seconds) = 0;
    virtual void setMusicVolume(float volume) = 0;
    // AUD-006: fadeOutMusic stops the track when the fade ends; fadeInMusic starts a track from silence
    virtual void fadeOutMusic(float seconds) = 0;
    virtual void fadeInMusic(const std::string& path, float seconds, bool loop = true) = 0;
    // AUD-008: pitch/tempo of the current track (1.0 = normal); a new track resets it
    virtual void setMusicPitch(float pitch) = 0;

    virtual void playSound(const std::string& path, float volume = 1.0f) = 0;
    virtual void stopAllSounds() = 0;

    virtual void setMasterVolume(float volume) = 0;
};

} // namespace nundertale
