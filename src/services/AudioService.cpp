#include "services/AudioService.hpp"
#include "services/MusicTracks.hpp"
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
    playMusic(path, loop, 0.0f, 0.0f);
}

void AudioService::playMusic(const std::string& path, bool loop, float loopStart, float loopEnd) {
    if (m_hasActiveMusic && m_currentMusicPath == path) {
        m_loopStart = loopStart;
        m_loopEnd = loopEnd;
        m_currentMusic.looping = (loopEnd > 0.0f) ? false : loop;
        if (!IsMusicStreamPlaying(m_currentMusic)) {
            ResumeMusicStream(m_currentMusic);
        }
        return;
    }

    stopMusic();

    if (FileExists(path.c_str())) {
        m_fade = MusicFade{};
        m_fadeFactor = 1.0f;
        m_loopStart = loopStart;
        m_loopEnd = loopEnd;
        m_currentMusic = LoadMusicStream(path.c_str());
        // If custom loopEnd is defined, raylib's internal loop is disabled so we manage seeking manually
        m_currentMusic.looping = (loopEnd > 0.0f) ? false : loop;
        SetMusicVolume(m_currentMusic, m_musicVolume * m_fadeFactor);
        PlayMusicStream(m_currentMusic);
        m_currentMusicPath = path;
        m_hasActiveMusic = true;
    } else {
        std::cerr << "[AudioService] Music file not found: " << path << std::endl;
    }
}

void AudioService::setMusicLoopPoints(float loopStart, float loopEnd) {
    m_loopStart = loopStart;
    m_loopEnd = loopEnd;
    if (m_hasActiveMusic && loopEnd > 0.0f) {
        m_currentMusic.looping = false;
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
    m_loopStart = 0.0f;
    m_loopEnd = 0.0f;
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

    if (m_loopEnd > 0.0f) {
        const float played = GetMusicTimePlayed(m_currentMusic);
        if (shouldLoopSeek(played, m_loopEnd)) {
            SeekMusicStream(m_currentMusic, m_loopStart);
        }
    }

    if (m_fade.active) {
        m_fadeFactor = fadeStep(m_fade, GetFrameTime());
        SetMusicVolume(m_currentMusic, m_musicVolume * m_fadeFactor);
        if (!m_fade.active && m_fade.stopAtEnd) {
            stopMusic();
        }
    }
}

float AudioService::musicTime() const {
    return m_hasActiveMusic ? GetMusicTimePlayed(m_currentMusic) : 0.0f;
}

void AudioService::seekMusic(float seconds) {
    if (m_hasActiveMusic) {
        SeekMusicStream(m_currentMusic, seconds);
    }
}

void AudioService::setMusicPitch(float pitch) {
    if (m_hasActiveMusic) {
        SetMusicPitch(m_currentMusic, pitch > 0.0f ? pitch : 1.0f);
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

    Sound voice = it->second;
    if (IsSoundPlaying(voice)) {
        // AUD-007: overlap instead of restarting — use a free alias, add one, or reuse the oldest
        std::vector<Sound>& aliases = m_soundAliases[path];
        bool found = false;
        for (const Sound& alias : aliases) {
            if (!IsSoundPlaying(alias)) {
                voice = alias;
                found = true;
                break;
            }
        }
        if (!found) {
            if (aliases.size() < MAX_SOUND_ALIASES) {
                aliases.push_back(LoadSoundAlias(it->second));
                voice = aliases.back();
            } else {
                std::size_t& next = m_nextAlias[path];
                voice = aliases[next % aliases.size()];
                next = (next + 1) % aliases.size();
            }
        }
    }
    SetSoundVolume(voice, volume);
    PlaySound(voice);
}

void AudioService::stopAllSounds() {
    for (auto& pair : m_soundAliases) {
        for (Sound& alias : pair.second) {
            StopSound(alias);
            UnloadSoundAlias(alias);
        }
    }
    m_soundAliases.clear();
    m_nextAlias.clear();
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
