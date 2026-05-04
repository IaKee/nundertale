#pragma once

#include <charconv>
#include <cmath>
#include <cctype>
#include <iomanip>
#include <sstream>
#include <string>
#include <system_error>
#include <vector>

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

inline std::string raidFormatMinutesSeconds(float secondsVal) {
    const float safeVal = (secondsVal > 0.0f) ? secondsVal : 0.0f;
    const long long totalSeconds = static_cast<long long>(std::ceil(safeVal));
    const long long minutes = totalSeconds / 60;
    const long long seconds = totalSeconds % 60;

    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << minutes
        << ':' << std::setw(2) << seconds;
    return oss.str();
}

inline std::string raidLabel(const RaidClock& clock) {
    if (!clock.started) return "";
    if (raidKicked(clock)) return "RAID: KICKED";
    return "RAID " + raidFormatMinutesSeconds(clock.timeLeft);
}

inline int raidPendingMessage(float prevLeft, float nowLeft, int alreadyShownMask) {
    if ((alreadyShownMask & (1 << 0)) == 0) {
        return 0;
    }
    if ((alreadyShownMask & (1 << 1)) == 0 && prevLeft >= 900.0f && nowLeft <= 900.0f) {
        return 1;
    }
    if ((alreadyShownMask & (1 << 2)) == 0 && prevLeft >= 300.0f && nowLeft <= 300.0f) {
        return 2;
    }
    if ((alreadyShownMask & (1 << 3)) == 0 && prevLeft >= 0.0f && nowLeft <= 0.0f) {
        return 3;
    }
    return -1;
}

inline std::vector<std::string> raidMessageLines(int index, const std::string& startMmSs = "22:00") {
    switch (index) {
        case 0:
            return {
                "* Your phone buzzes.",
                "* \"RAID STARTS IN " + startMmSs + ". BE THERE OR GET KICKED. bring dew -xX_Moderator_Xx\""
            };
        case 1:
            return {
                "* Your phone buzzes.",
                "* \"15 MIN WHERE ARE U\""
            };
        case 2:
            return {
                "* Your phone buzzes.",
                "* \"5 min. ur kicked if ur late. no cap.\""
            };
        case 3:
            return {
                "* Your phone buzzes.",
                "* \"Raid started without you. Kicked.\"",
                "* ...You feel strangely okay with that."
            };
        default:
            return {};
    }
}

inline std::vector<std::string> raidMessageLines(int index, float startSeconds) {
    return raidMessageLines(index, raidFormatMinutesSeconds(startSeconds));
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
