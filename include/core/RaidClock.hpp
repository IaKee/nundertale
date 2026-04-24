#pragma once

#include <charconv>
#include <cmath>
#include <cctype>
#include <iomanip>
#include <sstream>
#include <string>
#include <system_error>

namespace nundertale {

struct RaidClock {
    bool started = false;
    float timeLeft = 0.0f;
    bool kickedAnnounced = false;
};

inline void raidStart(RaidClock& clock, float startSeconds) {
    if (clock.started) return;
    clock.started = true;
    clock.timeLeft = (startSeconds > 0.0f) ? startSeconds : 0.0f;
}

inline void raidTick(RaidClock& clock, float dt, bool paused) {
    if (!clock.started || paused || dt <= 0.0f) return;
    clock.timeLeft -= dt;
    if (clock.timeLeft < 0.0f) clock.timeLeft = 0.0f;
}

inline bool raidKicked(const RaidClock& clock) {
    return clock.started && clock.timeLeft <= 0.0f;
}

inline std::string raidLabel(const RaidClock& clock) {
    if (!clock.started) return "";
    if (raidKicked(clock)) return "RAID: KICKED";

    const long long totalSeconds = static_cast<long long>(std::ceil(clock.timeLeft));
    const long long minutes = totalSeconds / 60;
    const long long seconds = totalSeconds % 60;

    std::ostringstream label;
    label << "RAID " << std::setfill('0') << std::setw(2) << minutes
          << ':' << std::setw(2) << seconds;
    return label.str();
}

inline int raidStartSecondsFromJson(const std::string& text, int fallback) {
    const std::string key = "\"startSeconds\"";
    std::size_t pos = text.find(key);
    if (pos == std::string::npos) return fallback;

    pos = text.find(':', pos + key.size());
    if (pos == std::string::npos) return fallback;
    ++pos;

    while (pos < text.size() && std::isspace(static_cast<unsigned char>(text[pos])) != 0) {
        ++pos;
    }

    int value = 0;
    const char* begin = text.data() + pos;
    const char* end = text.data() + text.size();
    const auto result = std::from_chars(begin, end, value);
    if (result.ec != std::errc{} || result.ptr == begin || value < 0) return fallback;

    const char* cursor = result.ptr;
    while (cursor < end && std::isspace(static_cast<unsigned char>(*cursor)) != 0) {
        ++cursor;
    }
    if (cursor < end && *cursor != ',' && *cursor != '}') return fallback;

    return value;
}

} // namespace nundertale
