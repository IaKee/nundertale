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
        m_fade = MusicFade{};
        m_fadeFactor = 1.0f;
        m_currentMusic = LoadMusicStream(path.c_str());
        m_currentMusic.looping = loop;
        SetMusicVolume(m_currentMusic, m_musicVolume * m_fadeFactor);
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
    m_fade = MusicFade{};
    m_fadeFactor = 1.0f;
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
    if (!m_hasActiveMusic) {
        return;
    }
    UpdateMusicStream(m_currentMusic);
    if (m_fade.active) {
        m_fadeFactor = fadeStep(m_fade, GetFrameTime());
        SetMusicVolume(m_currentMusic, m_musicVolume * m_fadeFactor);
        if (!m_fade.active && m_fade.stopAtEnd) {
            stopMusic();
        }
    }
}

void AudioService::fadeOutMusic(float seconds) {
    if (!m_hasActiveMusic) {
        return;
    }
    if (seconds <= 0.0f) {
        stopMusic();
        return;
    }
    fadeStart(m_fade, m_fadeFactor, 0.0f, seconds, true);
}

void AudioService::fadeInMusic(const std::string& path, float seconds, bool loop) {
    playMusic(path, loop);
    if (!m_hasActiveMusic || seconds <= 0.0f) {
        return;
    }
    m_fadeFactor = 0.0f;
    fadeStart(m_fade, 0.0f, 1.0f, seconds, false);
    SetMusicVolume(m_currentMusic, 0.0f);
}

void AudioService::setMusicVolume(float volume) {
    m_musicVolume = volume;
    if (m_hasActiveMusic) {
        SetMusicVolume(m_currentMusic, m_musicVolume * m_fadeFactor);
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
