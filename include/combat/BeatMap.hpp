#pragma once

#include "core/TiledMap.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <limits>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace nundertale {

struct BeatShot {
    float from = 0.0f;
    float to = 0.0f;
    std::string shot;
};

struct BeatMap {
    std::string track;
    float bpm = 130.0f;
    float offset = 0.0f;
    float loopLength = 3.692f;
    float menuLoopStart = 1.846f;
    float menuLoopEnd = 3.692f;
    int passesPerTurn = 3;
    float turnEndsAt = 1.385f;
    float latencyMs = 0.0f;
    std::vector<BeatShot> shots;
    std::vector<float> notes;
    std::vector<int> bigNotes;
    std::vector<float> kicks;
    std::vector<std::string> expressions;
};

struct BeatEvent {
    float turnTime = 0.0f;
    int pass = 0;
    int noteIndex = 0;
    bool big = false;
    bool kick = false;
};

namespace beat_detail {

using JsonValue = tiled_detail::JsonValue;

inline bool readString(const JsonValue& object, const char* key, std::string& out) {
    const JsonValue* value = object.field(key);
    if (value == nullptr) return true;
    if (value->kind != JsonValue::Kind::String) return false;
    out = value->string;
    return true;
}

inline bool readFloat(const JsonValue& object, const char* key, float& out) {
    const JsonValue* value = object.field(key);
    if (value == nullptr) return true;
    if (value->kind != JsonValue::Kind::Number) return false;
    out = static_cast<float>(value->number);
    return std::isfinite(out);
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

inline bool readFloatArray(const JsonValue& object, const char* key, std::vector<float>& out) {
    const JsonValue* value = object.field(key);
    if (value == nullptr) return true;
    if (value->kind != JsonValue::Kind::Array) return false;
    std::vector<float> parsed;
    parsed.reserve(value->array.size());
    for (const JsonValue& element : value->array) {
        if (element.kind != JsonValue::Kind::Number) return false;
        const float number = static_cast<float>(element.number);
        if (!std::isfinite(number)) return false;
        parsed.push_back(number);
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

inline bool containsTime(const std::vector<float>& values, float target) {
    constexpr float EPSILON = 0.0001f;
    return std::any_of(values.begin(), values.end(), [target](float value) {
        return std::fabs(value - target) <= EPSILON;
    });
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
        !beat_detail::readFloat(root, "bpm", parsed.bpm) ||
        !beat_detail::readFloat(root, "offset", parsed.offset) ||
        !beat_detail::readFloat(root, "loopLength", parsed.loopLength) ||
        !beat_detail::readInt(root, "passesPerTurn", parsed.passesPerTurn) ||
        !beat_detail::readFloat(root, "turnEndsAt", parsed.turnEndsAt) ||
        !beat_detail::readFloat(root, "latencyMs", parsed.latencyMs) ||
        !beat_detail::readStringArray(root, "expressions", parsed.expressions)) {
        return false;
    }

    if (const JsonValue* menuLoop = root.field("menuLoop")) {
        if (menuLoop->kind != JsonValue::Kind::Array || menuLoop->array.size() != 2U ||
            menuLoop->array[0].kind != JsonValue::Kind::Number ||
            menuLoop->array[1].kind != JsonValue::Kind::Number) {
            return false;
        }
        parsed.menuLoopStart = static_cast<float>(menuLoop->array[0].number);
        parsed.menuLoopEnd = static_cast<float>(menuLoop->array[1].number);
        if (!std::isfinite(parsed.menuLoopStart) || !std::isfinite(parsed.menuLoopEnd)) return false;
    }

    if (const JsonValue* shots = root.field("shots")) {
        if (shots->kind != JsonValue::Kind::Array) return false;
        parsed.shots.clear();
        parsed.shots.reserve(shots->array.size());
        for (const JsonValue& value : shots->array) {
            if (value.kind != JsonValue::Kind::Object) return false;
            BeatShot shot;
            if (!beat_detail::readFloat(value, "from", shot.from) ||
                !beat_detail::readFloat(value, "to", shot.to) ||
                !beat_detail::readString(value, "shot", shot.shot) || shot.shot.empty()) {
                return false;
            }
            parsed.shots.push_back(std::move(shot));
        }
    }

    if (const JsonValue* burst = root.field("burst")) {
        if (burst->kind != JsonValue::Kind::Object ||
            !beat_detail::readFloatArray(*burst, "notes", parsed.notes) ||
            !beat_detail::readIntArray(*burst, "bigNotes", parsed.bigNotes) ||
            !beat_detail::readFloatArray(*burst, "kicks", parsed.kicks)) {
            return false;
        }
    }

    if (parsed.loopLength <= 0.0f || parsed.passesPerTurn < 0) return false;
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

struct BeatTurnClock {
    float lastRaw = -1.0f;
    int pass = 0;

    float advance(const BeatMap& map, float rawMusicTime) {
        if (lastRaw >= 0.0f && lastRaw - rawMusicTime > map.loopLength * 0.5f) {
            ++pass;
        }
        lastRaw = rawMusicTime;
        return static_cast<float>(pass) * map.loopLength + (rawMusicTime - map.offset);
    }
};

inline float beatTurnDuration(const BeatMap& map) {
    return static_cast<float>(map.passesPerTurn - 1) * map.loopLength +
           map.turnEndsAt;
}

inline bool beatTurnFinished(const BeatMap& map, float turnTime) {
    return turnTime >= beatTurnDuration(map);
}

inline std::vector<BeatEvent> beatEventsBetween(const BeatMap& map,
                                                float prevTurnTime,
                                                float nowTurnTime) {
    std::vector<BeatEvent> events;
    if (nowTurnTime <= prevTurnTime) return events;

    const float latencySeconds = map.latencyMs / 1000.0f;
    for (int pass = 0; pass < map.passesPerTurn; ++pass) {
        for (std::size_t noteIndex = 0; noteIndex < map.notes.size(); ++noteIndex) {
            const float note = map.notes[noteIndex];
            // notes are relative to the pass downbeat; BeatTurnClock already removes `offset`
            const float eventTime = static_cast<float>(pass) * map.loopLength + note -
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

inline std::string beatShotAt(const BeatMap& map, float loopTime) {
    if (map.loopLength <= 0.0f) return {};
    float wrapped = std::fmod(loopTime, map.loopLength);
    if (wrapped < 0.0f) wrapped += map.loopLength;
    for (const BeatShot& shot : map.shots) {
        if (wrapped >= shot.from && wrapped < shot.to) return shot.shot;
    }
    return {};
}

inline std::size_t beatExpressionIndex(const BeatMap& map, int turnIndex, int pass) {
    if (map.expressions.empty()) return 0U;
    const int count = static_cast<int>(map.expressions.size());
    int index = (turnIndex + pass) % count;
    if (index < 0) index += count;
    return static_cast<std::size_t>(index);
}

} // namespace nundertale
