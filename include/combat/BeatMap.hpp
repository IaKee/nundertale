#pragma once

#include "core/TiledMap.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <limits>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace nundertale {

constexpr double BEAT_TIMING_TOLERANCE = 0.0005;
constexpr double BEAT_CLOCK_JITTER_TOLERANCE = 0.002;

struct BeatShot {
    double from = 0.0;
    double to = 0.0;
    std::string shot;
};

struct BeatMap {
    std::string track;
    double bpm = 130.0;
    double offset = 0.0;
    double loopLength = 32.0 * (60.0 / 130.0 / 4.0);
    double menuLoopStart = 16.0 * (60.0 / 130.0 / 4.0);
    double menuLoopEnd = 32.0 * (60.0 / 130.0 / 4.0);
    int passesPerTurn = 3;
    double turnEndsAt = 12.0 * (60.0 / 130.0 / 4.0);
    double latencyMs = 0.0;
    std::vector<BeatShot> shots;
    std::vector<double> notes;
    std::vector<int> bigNotes;
    std::vector<double> kicks;
    std::vector<std::string> expressions;
};

struct BeatEvent {
    double turnTime = 0.0;
    int pass = 0;
    int noteIndex = 0;
    bool big = false;
    bool kick = false;
};

inline double beatQuarter(const BeatMap& map) {
    return 60.0 / map.bpm;
}

inline double beatSixteenth(const BeatMap& map) {
    return beatQuarter(map) / 4.0;
}

inline double positiveBeatModulo(double value, double modulus) {
    if (!(modulus > 0.0) || !std::isfinite(value)) return 0.0;
    double result = std::fmod(value, modulus);
    if (result < 0.0) result += modulus;
    return result;
}

namespace beat_detail {

using JsonValue = tiled_detail::JsonValue;

inline bool readString(const JsonValue& object, const char* key, std::string& out) {
    const JsonValue* value = object.field(key);
    if (value == nullptr) return true;
    if (value->kind != JsonValue::Kind::String) return false;
    out = value->string;
    return true;
}

inline bool readDouble(const JsonValue& object, const char* key, double& out) {
    const JsonValue* value = object.field(key);
    if (value == nullptr) return true;
    if (value->kind != JsonValue::Kind::Number || !std::isfinite(value->number)) return false;
    out = value->number;
    return true;
}

inline bool readInt(const JsonValue& object, const char* key, int& out) {
    const JsonValue* value = object.field(key);
    if (value == nullptr) return true;
    if (value->kind != JsonValue::Kind::Number ||
        value->number < static_cast<double>(std::numeric_limits<int>::min()) ||
        value->number > static_cast<double>(std::numeric_limits<int>::max()) ||
        std::floor(value->number) != value->number) {
        return false;
    }
    out = static_cast<int>(value->number);
    return true;
}

inline bool readDoubleArray(const JsonValue& object,
                            const char* key,
                            std::vector<double>& out) {
    const JsonValue* value = object.field(key);
    if (value == nullptr) return true;
    if (value->kind != JsonValue::Kind::Array) return false;
    std::vector<double> parsed;
    parsed.reserve(value->array.size());
    for (const JsonValue& element : value->array) {
        if (element.kind != JsonValue::Kind::Number || !std::isfinite(element.number)) return false;
        parsed.push_back(element.number);
    }
    out = std::move(parsed);
    return true;
}

inline bool readIntArray(const JsonValue& object, const char* key, std::vector<int>& out) {
    const JsonValue* value = object.field(key);
    if (value == nullptr) return true;
    if (value->kind != JsonValue::Kind::Array) return false;
    std::vector<int> parsed;
    parsed.reserve(value->array.size());
    for (const JsonValue& element : value->array) {
        if (element.kind != JsonValue::Kind::Number ||
            element.number < static_cast<double>(std::numeric_limits<int>::min()) ||
            element.number > static_cast<double>(std::numeric_limits<int>::max()) ||
            std::floor(element.number) != element.number) {
            return false;
        }
        parsed.push_back(static_cast<int>(element.number));
    }
    out = std::move(parsed);
    return true;
}

inline bool readStringArray(const JsonValue& object,
                            const char* key,
                            std::vector<std::string>& out) {
    const JsonValue* value = object.field(key);
    if (value == nullptr) return true;
    if (value->kind != JsonValue::Kind::Array) return false;
    std::vector<std::string> parsed;
    parsed.reserve(value->array.size());
    for (const JsonValue& element : value->array) {
        if (element.kind != JsonValue::Kind::String) return false;
        parsed.push_back(element.string);
    }
    out = std::move(parsed);
    return true;
}

inline bool containsIndex(const std::vector<int>& values, int target) {
    return std::find(values.begin(), values.end(), target) != values.end();
}

inline bool containsTime(const std::vector<double>& values, double target) {
    return std::any_of(values.begin(), values.end(), [target](double value) {
        return std::fabs(value - target) <= BEAT_TIMING_TOLERANCE;
    });
}

inline bool snapToSixteenth(double& value, double sixteenth) {
    if (!std::isfinite(value) || !(sixteenth > 0.0)) return false;
    const double snapped = std::round(value / sixteenth) * sixteenth;
    if (std::fabs(value - snapped) > BEAT_TIMING_TOLERANCE) return false;
    value = snapped;
    return true;
}

// Authored decimals describe integer musical steps. Validate them, then replace them with
// values derived from BPM so rounded JSON never becomes the transport clock.
inline bool canonicalizeTiming(BeatMap& map) {
    if (!(map.bpm > 0.0) || !std::isfinite(map.bpm) || !std::isfinite(map.offset) ||
        !std::isfinite(map.latencyMs)) {
        return false;
    }
    const double sixteenth = beatSixteenth(map);
    if (!snapToSixteenth(map.loopLength, sixteenth) || map.loopLength <= 0.0 ||
        !snapToSixteenth(map.menuLoopStart, sixteenth) ||
        !snapToSixteenth(map.menuLoopEnd, sixteenth) ||
        !snapToSixteenth(map.turnEndsAt, sixteenth)) {
        return false;
    }
    for (BeatShot& shot : map.shots) {
        if (!snapToSixteenth(shot.from, sixteenth) ||
            !snapToSixteenth(shot.to, sixteenth)) return false;
    }
    for (double& note : map.notes) {
        if (!snapToSixteenth(note, sixteenth)) return false;
    }
    for (double& kick : map.kicks) {
        if (!snapToSixteenth(kick, sixteenth)) return false;
    }
    return map.passesPerTurn > 0 && map.menuLoopStart >= 0.0 &&
           map.menuLoopEnd > map.menuLoopStart && map.menuLoopEnd <= map.loopLength &&
           map.turnEndsAt >= 0.0 && map.turnEndsAt <= map.loopLength;
}

} // namespace beat_detail

inline bool parseBeatMap(const std::string& jsonText, BeatMap& out) {
    using beat_detail::JsonValue;

    JsonValue root;
    if (!tiled_detail::JsonParser(jsonText).parse(root) || root.kind != JsonValue::Kind::Object) {
        return false;
    }

    BeatMap parsed;
    if (!beat_detail::readString(root, "track", parsed.track) ||
        !beat_detail::readDouble(root, "bpm", parsed.bpm) ||
        !beat_detail::readDouble(root, "offset", parsed.offset) ||
        !beat_detail::readDouble(root, "loopLength", parsed.loopLength) ||
        !beat_detail::readInt(root, "passesPerTurn", parsed.passesPerTurn) ||
        !beat_detail::readDouble(root, "turnEndsAt", parsed.turnEndsAt) ||
        !beat_detail::readDouble(root, "latencyMs", parsed.latencyMs) ||
        !beat_detail::readStringArray(root, "expressions", parsed.expressions)) {
        return false;
    }

    if (const JsonValue* menuLoop = root.field("menuLoop")) {
        if (menuLoop->kind != JsonValue::Kind::Array || menuLoop->array.size() != 2U ||
            menuLoop->array[0].kind != JsonValue::Kind::Number ||
            menuLoop->array[1].kind != JsonValue::Kind::Number) {
            return false;
        }
        parsed.menuLoopStart = menuLoop->array[0].number;
        parsed.menuLoopEnd = menuLoop->array[1].number;
        if (!std::isfinite(parsed.menuLoopStart) || !std::isfinite(parsed.menuLoopEnd)) return false;
    }

    if (const JsonValue* shots = root.field("shots")) {
        if (shots->kind != JsonValue::Kind::Array) return false;
        parsed.shots.clear();
        parsed.shots.reserve(shots->array.size());
        for (const JsonValue& value : shots->array) {
            if (value.kind != JsonValue::Kind::Object) return false;
            BeatShot shot;
            if (!beat_detail::readDouble(value, "from", shot.from) ||
                !beat_detail::readDouble(value, "to", shot.to) ||
                !beat_detail::readString(value, "shot", shot.shot) || shot.shot.empty()) {
                return false;
            }
            parsed.shots.push_back(std::move(shot));
        }
    }

    if (const JsonValue* burst = root.field("burst")) {
        if (burst->kind != JsonValue::Kind::Object ||
            !beat_detail::readDoubleArray(*burst, "notes", parsed.notes) ||
            !beat_detail::readIntArray(*burst, "bigNotes", parsed.bigNotes) ||
            !beat_detail::readDoubleArray(*burst, "kicks", parsed.kicks)) {
            return false;
        }
    }

    if (!beat_detail::canonicalizeTiming(parsed)) return false;
    out = std::move(parsed);
    return true;
}

inline bool loadBeatMap(const std::string& path, BeatMap& out) {
    std::ifstream file(path);
    if (!file.is_open()) return false;
    std::ostringstream buffer;
    buffer << file.rdbuf();
    return parseBeatMap(buffer.str(), out);
}

struct BeatClockSample {
    double turnTime = 0.0;
    std::int64_t pass = 0;
    double phase = 0.0;
    bool accepted = false;
    bool advanced = false;
    bool wrapped = false;
};

struct BeatTurnClock {
    double lastRaw = 0.0;
    double lastPhase = 0.0;
    double turnTime = 0.0;
    std::int64_t pass = 0;
    std::uint64_t generation = 1;
    std::uint64_t acceptedGeneration = 0;
    bool awaitingSeek = true;

    void beginSeek() {
        ++generation;
        acceptedGeneration = 0;
        awaitingSeek = true;
        lastRaw = 0.0;
        lastPhase = 0.0;
        turnTime = 0.0;
        pass = 0;
    }

    bool hasAcceptedSeek() const {
        return !awaitingSeek && acceptedGeneration == generation;
    }

    BeatClockSample sample(const BeatMap& map, double rawMusicTime) const {
        BeatClockSample result{turnTime, pass, lastPhase, false, false, false};
        const double length = map.loopLength;
        if (awaitingSeek || !(length > 0.0) || !std::isfinite(rawMusicTime)) return result;

        const double phase = positiveBeatModulo(rawMusicTime - map.offset, length);
        std::int64_t sampledPass = pass;
        bool wrapped = false;
        if (phase + BEAT_CLOCK_JITTER_TOLERANCE < lastPhase) {
            const bool validatedWrap = lastPhase >= length * 0.75 && phase <= length * 0.25;
            if (!validatedWrap) return result;
            ++sampledPass;
            wrapped = true;
        } else if (phase < lastPhase) {
            return result;
        }

        const double sampledTurnTime = static_cast<double>(sampledPass) * length + phase;
        if (sampledTurnTime + BEAT_CLOCK_JITTER_TOLERANCE < turnTime) return result;
        result.turnTime = std::max(turnTime, sampledTurnTime);
        result.pass = sampledPass;
        result.phase = phase;
        result.accepted = true;
        result.advanced = result.turnTime > turnTime;
        result.wrapped = wrapped;
        return result;
    }

    BeatClockSample advance(const BeatMap& map, double rawMusicTime) {
        const double length = map.loopLength;
        if (!(length > 0.0) || !std::isfinite(rawMusicTime)) {
            return {turnTime, pass, lastPhase, false, false, false};
        }

        if (awaitingSeek) {
            const double relative = rawMusicTime - map.offset;
            // The old menu generation lives in [L/2, L). Exact L/2 is stale too.
            const bool inNewStartWindow =
                relative >= -BEAT_TIMING_TOLERANCE &&
                relative + BEAT_TIMING_TOLERANCE < length * 0.5;
            if (!inNewStartWindow) {
                return {turnTime, pass, lastPhase, false, false, false};
            }
            awaitingSeek = false;
            acceptedGeneration = generation;
            pass = 0;
            lastRaw = rawMusicTime;
            lastPhase = relative < 0.0 ? 0.0 : positiveBeatModulo(relative, length);
            turnTime = lastPhase;
            return {turnTime, pass, lastPhase, true, true, false};
        }

        const BeatClockSample next = sample(map, rawMusicTime);
        if (!next.accepted) return next;
        lastRaw = rawMusicTime;
        lastPhase = next.phase;
        turnTime = next.turnTime;
        pass = next.pass;
        return next;
    }
};

inline double beatTurnDuration(const BeatMap& map) {
    return static_cast<double>(map.passesPerTurn - 1) * map.loopLength + map.turnEndsAt;
}

inline bool beatTurnFinished(const BeatMap& map, double turnTime) {
    return turnTime >= beatTurnDuration(map);
}

inline double firstCompensatedBeatEvent(const BeatMap& map) {
    if (map.notes.empty()) return 0.0;
    return map.notes.front() - map.latencyMs / 1000.0;
}

inline double initialBeatEventCursor(const BeatMap& map) {
    return std::nextafter(firstCompensatedBeatEvent(map),
                          -std::numeric_limits<double>::infinity());
}

inline std::vector<BeatEvent> beatEventsBetween(const BeatMap& map,
                                                double prevTurnTime,
                                                double nowTurnTime) {
    std::vector<BeatEvent> events;
    if (nowTurnTime <= prevTurnTime) return events;

    const double latencySeconds = map.latencyMs / 1000.0;
    for (int pass = 0; pass < map.passesPerTurn; ++pass) {
        for (std::size_t noteIndex = 0; noteIndex < map.notes.size(); ++noteIndex) {
            const double note = map.notes[noteIndex];
            const double eventTime = static_cast<double>(pass) * map.loopLength + note -
                                     latencySeconds;
            if (eventTime <= prevTurnTime || eventTime > nowTurnTime) continue;

            BeatEvent event;
            event.turnTime = eventTime;
            event.pass = pass;
            event.noteIndex = static_cast<int>(noteIndex);
            event.big = beat_detail::containsIndex(map.bigNotes, event.noteIndex);
            event.kick = beat_detail::containsTime(map.kicks, note);
            events.push_back(event);
        }
    }
    return events;
}

inline std::string beatShotAt(const BeatMap& map, double loopTime) {
    if (map.loopLength <= 0.0) return {};
    const double wrapped = positiveBeatModulo(loopTime, map.loopLength);
    for (const BeatShot& shot : map.shots) {
        if (wrapped >= shot.from && wrapped < shot.to) return shot.shot;
    }
    return {};
}

struct BeatShakePose {
    double sway = 0.0;
    double squash = 1.0;
};

inline BeatShakePose beatShakePose(const BeatMap& map, double loopTime) {
    BeatShakePose pose;
    if (map.notes.empty()) return pose;
    const double phaseTime = positiveBeatModulo(loopTime, map.loopLength);
    int last = -1;
    for (std::size_t i = 0; i < map.notes.size(); ++i) {
        if (map.notes[i] <= phaseTime) last = static_cast<int>(i);
    }
    if (last < 0) return pose;
    const double start = map.notes[static_cast<std::size_t>(last)];
    const bool isLast = last + 1 >= static_cast<int>(map.notes.size());
    const double side = (last % 2 == 0) ? -1.0 : 1.0;
    if (isLast) {
        const double settle = std::clamp((phaseTime - start) / beatSixteenth(map), 0.0, 1.0);
        pose.sway = side * (1.0 - settle);
    } else {
        const double next = map.notes[static_cast<std::size_t>(last) + 1];
        const double notePhase = std::clamp((phaseTime - start) / (next - start), 0.0, 1.0);
        pose.sway = side * std::cos(notePhase * 3.14159265358979323846);
    }
    for (double kick : map.kicks) {
        const double since = phaseTime - kick;
        if (since >= 0.0 && since < beatSixteenth(map)) {
            pose.squash = std::min(pose.squash,
                                   0.94 + 0.06 * (since / beatSixteenth(map)));
        }
    }
    return pose;
}

inline std::size_t beatExpressionIndex(const BeatMap& map, int turnIndex, int pass) {
    if (map.expressions.empty()) return 0U;
    const int count = static_cast<int>(map.expressions.size());
    int index = (turnIndex + pass) % count;
    if (index < 0) index += count;
    return static_cast<std::size_t>(index);
}

inline int beatCloseColumn(double sway, int frameCount = 3) {
    if (frameCount <= 1) return 0;
    if (frameCount == 2) return sway > 0.0 ? 1 : 0;
    if (sway <= -0.5) return 0;
    if (sway >= 0.5) return 2;
    return 1;
}

struct BeatVisualState {
    double turnTime = 0.0;
    double phase = 0.0;
    int pass = 0;
    bool closeup = false;
    std::size_t expressionIndex = 0U;
    int wideFrame = 0;
    int closeColumn = 1;
    BeatShakePose pose;
};

inline BeatVisualState beatVisualStateAt(const BeatMap& map,
                                         int turnIndex,
                                         double turnTime,
                                         int wideFrameCount = 8,
                                         int closeFrameCount = 3) {
    BeatVisualState state;
    state.turnTime = std::max(0.0, turnTime);
    state.phase = positiveBeatModulo(state.turnTime, map.loopLength);
    state.pass = static_cast<int>(std::floor(state.turnTime / map.loopLength + 1e-12));
    state.closeup = beatShotAt(map, state.phase) == "closeup" &&
                    !beatTurnFinished(map, state.turnTime);
    state.expressionIndex = beatExpressionIndex(map, turnIndex, state.pass);
    const int frameCount = std::max(1, wideFrameCount);
    state.wideFrame = static_cast<int>(std::floor(state.phase / beatSixteenth(map) + 1e-12)) %
                      frameCount;
    state.pose = beatShakePose(map, state.phase);
    state.closeColumn = beatCloseColumn(state.pose.sway, closeFrameCount);
    return state;
}

} // namespace nundertale
