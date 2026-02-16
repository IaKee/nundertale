#include "services/AudioService.hpp"
#include <iostream>

namespace nundertale {

AudioService::AudioService() {
    InitAudioDevice();
    SetMasterVolume(m_masterVolume);
}

AudioService::~AudioService() {
    stopMusic();
    stopAllSounds();
    CloseAudioDevice();
}

void AudioService::playMusic(const std::string& path, bool loop) {
    if (m_hasActiveMusic && m_currentMusicPath == path) {
        if (!IsMusicStreamPlaying(m_currentMusic)) {
            ResumeMusicStream(m_currentMusic);
        }
        return;
    }

    stopMusic();

    if (FileExists(path.c_str())) {
        m_currentMusic = LoadMusicStream(path.c_str());
        m_currentMusic.looping = loop;
        SetMusicVolume(m_currentMusic, m_musicVolume);
        PlayMusicStream(m_currentMusic);
        m_currentMusicPath = path;
        m_hasActiveMusic = true;
    } else {
        std::cerr << "[AudioService] Music file not found: " << path << std::endl;
    }
}

void AudioService::stopMusic() {
    if (m_hasActiveMusic) {
        StopMusicStream(m_currentMusic);
        UnloadMusicStream(m_currentMusic);
        m_hasActiveMusic = false;
        m_currentMusicPath.clear();
    }
}

void AudioService::pauseMusic() {
    if (m_hasActiveMusic) {
        PauseMusicStream(m_currentMusic);
    }
}

void AudioService::resumeMusic() {
    if (m_hasActiveMusic) {
        ResumeMusicStream(m_currentMusic);
    }
}

void AudioService::updateMusic() {
    if (m_hasActiveMusic) {
        UpdateMusicStream(m_currentMusic);
    }
}

void AudioService::setMusicVolume(float volume) {
    m_musicVolume = volume;
    if (m_hasActiveMusic) {
        SetMusicVolume(m_currentMusic, m_musicVolume);
    }
}

void AudioService::playSound(const std::string& path, float volume) {
    auto it = m_soundCache.find(path);
    if (it == m_soundCache.end()) {
        if (!FileExists(path.c_str())) {
            std::cerr << "[AudioService] Sound file not found: " << path << std::endl;
            return;
        }
        Sound snd = LoadSound(path.c_str());
        it = m_soundCache.emplace(path, snd).first;
    }
    SetSoundVolume(it->second, volume);
    PlaySound(it->second);
}

void AudioService::stopAllSounds() {
    for (auto& pair : m_soundCache) {
        StopSound(pair.second);
        UnloadSound(pair.second);
    }
    m_soundCache.clear();
}

void AudioService::setMasterVolume(float volume) {
    m_masterVolume = volume;
    SetMasterVolume(m_masterVolume);
}

} // namespace nundertale
