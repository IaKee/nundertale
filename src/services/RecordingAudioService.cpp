#include "services/RecordingAudioService.hpp"

#include "core/Constants.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <vector>

namespace nundertale {

namespace {

std::uint32_t readLe32(const unsigned char* bytes) {
    return static_cast<std::uint32_t>(bytes[0]) |
           (static_cast<std::uint32_t>(bytes[1]) << 8U) |
           (static_cast<std::uint32_t>(bytes[2]) << 16U) |
           (static_cast<std::uint32_t>(bytes[3]) << 24U);
}

std::uint64_t readLe64(const unsigned char* bytes) {
    std::uint64_t value = 0;
    for (unsigned int index = 0; index < 8U; ++index) {
        value |= static_cast<std::uint64_t>(bytes[index]) << (index * 8U);
    }
    return value;
}

double oggLength(const std::vector<unsigned char>& bytes) {
    const std::array<unsigned char, 7> vorbis = { 1U, 'v', 'o', 'r', 'b', 'i', 's' };
    std::uint32_t sampleRate = 0;
    for (std::size_t index = 0; index + 16U <= bytes.size(); ++index) {
        if (std::equal(vorbis.begin(), vorbis.end(), bytes.begin() + static_cast<std::ptrdiff_t>(index))) {
            sampleRate = readLe32(bytes.data() + index + 12U);
            break;
        }
    }
    if (sampleRate == 0U) return 0.0;

    std::uint64_t maximumGranule = 0;
    for (std::size_t index = 0; index + 27U <= bytes.size();) {
        if (std::memcmp(bytes.data() + index, "OggS", 4U) != 0) {
            ++index;
            continue;
        }
        const std::size_t segmentCount = bytes[index + 26U];
        if (index + 27U + segmentCount > bytes.size()) break;
        std::size_t payloadSize = 0;
        for (std::size_t segment = 0; segment < segmentCount; ++segment) {
            payloadSize += bytes[index + 27U + segment];
        }
        const std::uint64_t granule = readLe64(bytes.data() + index + 6U);
        if (granule != std::numeric_limits<std::uint64_t>::max()) {
            maximumGranule = std::max(maximumGranule, granule);
        }
        const std::size_t next = index + 27U + segmentCount + payloadSize;
        if (next <= index || next > bytes.size()) break;
        index = next;
    }
    return static_cast<double>(maximumGranule) / static_cast<double>(sampleRate);
}

double wavLength(const std::vector<unsigned char>& bytes) {
    if (bytes.size() < 12U || std::memcmp(bytes.data(), "RIFF", 4U) != 0 ||
        std::memcmp(bytes.data() + 8U, "WAVE", 4U) != 0) return 0.0;
    std::uint32_t byteRate = 0;
    std::uint32_t dataSize = 0;
    for (std::size_t index = 12U; index + 8U <= bytes.size();) {
        const std::uint32_t chunkSize = readLe32(bytes.data() + index + 4U);
        const std::size_t content = index + 8U;
        if (content + chunkSize > bytes.size()) break;
        if (std::memcmp(bytes.data() + index, "fmt ", 4U) == 0 && chunkSize >= 12U) {
            byteRate = readLe32(bytes.data() + content + 8U);
        } else if (std::memcmp(bytes.data() + index, "data", 4U) == 0) {
            dataSize = chunkSize;
        }
        index = content + chunkSize + (chunkSize & 1U);
    }
    return byteRate == 0U ? 0.0 : static_cast<double>(dataSize) / static_cast<double>(byteRate);
}

} // namespace

RecordingAudioService::RecordingAudioService(const std::string& outputDirectory) {
    const std::filesystem::path logPath = std::filesystem::path(outputDirectory) / "audio_log.jsonl";
    m_log.open(logPath, std::ios::out | std::ios::trunc);
    if (!m_log) {
        std::cerr << "[RecordingAudioService] Cannot create audio log: " << logPath.string() << '\n';
    }
}

void RecordingAudioService::playMusic(const std::string& path, bool loop) {
    playMusic(path, loop, 0.0f, 0.0f);
}

void RecordingAudioService::playMusic(const std::string& path, bool loop,
                                      float loopStart, float loopEnd) {
    if (m_active && m_musicPath == path) {
        m_loop = loop;
        m_loopStart = loopStart;
        m_loopEnd = loopEnd;
        m_paused = false;
        return;
    }

    stopMusic();
    if (!std::filesystem::exists(path)) {
        std::cerr << "[RecordingAudioService] Music file not found: " << path << '\n';
        return;
    }

    m_musicPath = path;
    m_trackLength = readTrackLength(path);
    m_position = 0.0;
    m_loop = loop;
    m_loopStart = loopStart;
    m_loopEnd = loopEnd;
    m_pitch = 1.0f;
    m_fade = MusicFade{};
    m_fadeFactor = 1.0f;
    m_paused = false;
    m_active = true;
}

void RecordingAudioService::setMusicLoopPoints(float loopStart, float loopEnd) {
    m_loopStart = loopStart;
    m_loopEnd = loopEnd;
}

void RecordingAudioService::stopMusic() {
    m_musicPath.clear();
    m_position = 0.0;
    m_trackLength = 0.0;
    m_loopStart = 0.0f;
    m_loopEnd = 0.0f;
    m_fade = MusicFade{};
    m_fadeFactor = 1.0f;
    m_paused = false;
    m_active = false;
}

void RecordingAudioService::pauseMusic() {
    if (m_active) m_paused = true;
}

void RecordingAudioService::resumeMusic() {
    if (m_active) m_paused = false;
}

void RecordingAudioService::updateMusic() {
    if (m_hasUpdated) {
        ++m_tick;
    } else {
        m_hasUpdated = true;
    }
    logMusicTick();

    if (!m_active) return;
    if (!m_paused) {
        m_position += static_cast<double>(LOGIC_TIMESTEP) * static_cast<double>(m_pitch);
        if (m_loopEnd > 0.0f && m_position >= static_cast<double>(m_loopEnd)) {
            m_position = static_cast<double>(m_loopStart);
        } else if (m_loop && m_trackLength > 0.0 && m_position >= m_trackLength) {
            m_position = std::fmod(m_position, m_trackLength);
        } else if (!m_loop && m_loopEnd <= 0.0f && m_trackLength > 0.0 &&
                   m_position >= m_trackLength) {
            stopMusic();
            return;
        }
    }

    if (m_fade.active) {
        m_fadeFactor = fadeStep(m_fade, LOGIC_TIMESTEP);
        if (!m_fade.active && m_fade.stopAtEnd) stopMusic();
    }
}

float RecordingAudioService::musicTime() const {
    return m_active ? static_cast<float>(m_position) : 0.0f;
}

void RecordingAudioService::seekMusic(float seconds) {
    if (m_active) m_position = std::max(0.0, static_cast<double>(seconds));
}

void RecordingAudioService::setMusicVolume(float volume) {
    m_musicVolume = volume;
}

void RecordingAudioService::fadeOutMusic(float seconds) {
    if (!m_active) return;
    if (seconds <= 0.0f) {
        stopMusic();
        return;
    }
    fadeStart(m_fade, m_fadeFactor, 0.0f, seconds, true);
}

void RecordingAudioService::fadeInMusic(const std::string& path, float seconds, bool loop) {
    playMusic(path, loop);
    if (!m_active || seconds <= 0.0f) return;
    m_fadeFactor = 0.0f;
    fadeStart(m_fade, 0.0f, 1.0f, seconds, false);
}

void RecordingAudioService::setMusicPitch(float pitch) {
    if (m_active) m_pitch = pitch > 0.0f ? pitch : 1.0f;
}

void RecordingAudioService::playSound(const std::string& path, float volume) {
    if (!m_log) return;
    const float effective = std::clamp(volume * m_masterVolume, 0.0f, 1.0f);
    m_log << "{\"t\":" << m_tick << ",\"sound\":" << jsonString(path)
          << ",\"vol\":" << std::setprecision(9) << effective << "}\n";
    m_log.flush();
}

void RecordingAudioService::stopAllSounds() {
}

void RecordingAudioService::setMasterVolume(float volume) {
    m_masterVolume = volume;
}

double RecordingAudioService::readTrackLength(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) return 0.0;
    const std::vector<unsigned char> bytes(
        (std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>{});
    if (bytes.size() >= 4U && std::memcmp(bytes.data(), "OggS", 4U) == 0) {
        return oggLength(bytes);
    }
    return wavLength(bytes);
}

std::string RecordingAudioService::jsonString(const std::string& value) {
    std::string escaped = "\"";
    for (const unsigned char ch : value) {
        switch (ch) {
            case '\"': escaped += "\\\""; break;
            case '\\': escaped += "\\\\"; break;
            case '\b': escaped += "\\b"; break;
            case '\f': escaped += "\\f"; break;
            case '\n': escaped += "\\n"; break;
            case '\r': escaped += "\\r"; break;
            case '\t': escaped += "\\t"; break;
            default:
                if (ch < 0x20U) {
                    const char* digits = "0123456789abcdef";
                    escaped += "\\u00";
                    escaped.push_back(digits[(ch >> 4U) & 0x0FU]);
                    escaped.push_back(digits[ch & 0x0FU]);
                } else {
                    escaped.push_back(static_cast<char>(ch));
                }
                break;
        }
    }
    escaped.push_back('\"');
    return escaped;
}

float RecordingAudioService::effectiveMusicVolume() const {
    if (!m_active || m_paused) return 0.0f;
    return std::clamp(m_musicVolume * m_fadeFactor * m_masterVolume, 0.0f, 1.0f);
}

void RecordingAudioService::logMusicTick() {
    if (!m_log) return;
    m_log << "{\"t\":" << m_tick << ",\"music\":" << jsonString(m_musicPath)
          << ",\"pos\":" << std::setprecision(9) << (m_active ? m_position : 0.0)
          << ",\"vol\":" << effectiveMusicVolume()
          << ",\"pitch\":" << (m_active ? m_pitch : 1.0f) << "}\n";
    m_log.flush();
}

} // namespace nundertale
