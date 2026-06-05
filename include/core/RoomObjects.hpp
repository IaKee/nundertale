#pragma once

#include "core/TiledMap.hpp"
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <functional>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

namespace nundertale {

struct RoomConditionTerm {
    std::string flag;
    std::string op;
    int value = 0;
};

inline std::string trimRoomText(const std::string& text) {
    std::size_t first = 0;
    while (first < text.size() && std::isspace(static_cast<unsigned char>(text[first])) != 0) ++first;
    std::size_t last = text.size();
    while (last > first && std::isspace(static_cast<unsigned char>(text[last - 1])) != 0) --last;
    return text.substr(first, last - first);
}

inline bool parseRoomCondition(const std::string& text, std::vector<RoomConditionTerm>& terms) {
    terms.clear();
    const std::string trimmed = trimRoomText(text);
    if (trimmed.empty()) return true;

    std::size_t start = 0;
    while (start < trimmed.size()) {
        const std::size_t separator = trimmed.find("&&", start);
        const std::string part = trimRoomText(trimmed.substr(
            start, separator == std::string::npos ? std::string::npos : separator - start));
        std::istringstream stream(part);
        RoomConditionTerm term;
        std::string extra;
        long long value = 0;
        if (!(stream >> term.flag >> term.op >> value) || (stream >> extra) ||
            term.flag.empty() ||
            (term.op != "==" && term.op != "!=" && term.op != ">=" &&
             term.op != "<=" && term.op != ">" && term.op != "<") ||
            value < static_cast<long long>(std::numeric_limits<int>::min()) ||
            value > static_cast<long long>(std::numeric_limits<int>::max())) {
            terms.clear();
            return false;
        }
        term.value = static_cast<int>(value);
        terms.push_back(std::move(term));
        if (separator == std::string::npos) break;
        start = separator + 2;
        if (start >= trimmed.size()) {
            terms.clear();
            return false;
        }
    }
    return true;
}

inline bool evaluateRoomCondition(
    const std::string& text,
    const std::function<int(const std::string&)>& flagLookup,
    bool* parsed = nullptr) {
    std::vector<RoomConditionTerm> terms;
    const bool valid = parseRoomCondition(text, terms);
    if (parsed != nullptr) *parsed = valid;
    if (!valid) return true;

    for (const RoomConditionTerm& term : terms) {
        const int actual = flagLookup(term.flag);
        bool matches = false;
        if (term.op == "==") matches = actual == term.value;
        else if (term.op == "!=") matches = actual != term.value;
        else if (term.op == ">=") matches = actual >= term.value;
        else if (term.op == "<=") matches = actual <= term.value;
        else if (term.op == ">") matches = actual > term.value;
        else if (term.op == "<") matches = actual < term.value;
        if (!matches) return false;
    }
    return true;
}

inline std::vector<std::string> splitRoomPages(const std::string& text) {
    std::vector<std::string> pages;
    std::size_t start = 0;
    while (true) {
        const std::size_t separator = text.find("||", start);
        pages.push_back(text.substr(start,
            separator == std::string::npos ? std::string::npos : separator - start));
        if (separator == std::string::npos) break;
        start = separator + 2;
    }
    return pages;
}

inline std::string selectRoomVariant(const std::vector<std::string>& variants,
                                     int counter,
                                     int fur,
                                     int furMin,
                                     const std::string& furLines) {
    if (!furLines.empty() && fur >= furMin) return furLines;
    if (variants.empty()) return {};
    const int index = std::clamp(counter, 0, static_cast<int>(variants.size()) - 1);
    return variants[static_cast<std::size_t>(index)];
}

constexpr std::uint32_t TILED_GID_MASK = 0x1FFFFFFFu;

inline std::uint32_t maskTiledGid(std::uint32_t gid) {
    return gid & TILED_GID_MASK;
}

struct TiledSourceRect {
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
    bool valid = false;
};

inline TiledSourceRect gidToSourceRect(std::uint32_t gid,
                                       std::uint32_t firstGid,
                                       int tileWidth,
                                       int tileHeight,
                                       int columns) {
    const std::uint32_t masked = maskTiledGid(gid);
    if (masked < firstGid || firstGid == 0 || tileWidth <= 0 || tileHeight <= 0 || columns <= 0) {
        return {};
    }
    const std::uint32_t index = masked - firstGid;
    return {
        static_cast<int>(index % static_cast<std::uint32_t>(columns)) * tileWidth,
        static_cast<int>(index / static_cast<std::uint32_t>(columns)) * tileHeight,
        tileWidth,
        tileHeight,
        true
    };
}

inline std::string tiledObjectType(const TiledObject& object) {
    return object.type.empty() ? object.className : object.type;
}

inline const TiledObject* findRoomObject(const TiledMap& map,
                                         const std::string& type,
                                         const std::string& name = {}) {
    for (const TiledLayer& layer : map.layers) {
        if (layer.type != TiledLayerType::Objects) continue;
        for (const TiledObject& object : layer.objects) {
            if (tiledObjectType(object) == type && (name.empty() || object.name == name)) {
                return &object;
            }
        }
    }
    return nullptr;
}

inline const TiledObject* findRoomSpawn(const TiledMap& map, const std::string& name = "spawn") {
    return findRoomObject(map, "spawn", name.empty() ? "spawn" : name);
}

inline const TiledObject* findRoomDoor(const TiledMap& map, const std::string& name) {
    return findRoomObject(map, "door", name);
}

inline const TiledProperty* findTiledProperty(const std::vector<TiledProperty>& properties,
                                              const std::string& name) {
    for (const TiledProperty& property : properties) {
        if (property.name == name) return &property;
    }
    return nullptr;
}

inline std::string tiledStringProperty(const std::vector<TiledProperty>& properties,
                                       const std::string& name,
                                       const std::string& fallback = {}) {
    const TiledProperty* property = findTiledProperty(properties, name);
    if (property == nullptr) return fallback;
    const std::string* value = std::get_if<std::string>(&property->value);
    return value == nullptr ? fallback : *value;
}

inline bool tiledBoolProperty(const std::vector<TiledProperty>& properties,
                              const std::string& name,
                              bool fallback = false) {
    const TiledProperty* property = findTiledProperty(properties, name);
    if (property == nullptr) return fallback;
    if (const bool* value = std::get_if<bool>(&property->value)) return *value;
    return fallback;
}

inline int tiledIntProperty(const std::vector<TiledProperty>& properties,
                            const std::string& name,
                            int fallback = 0) {
    const TiledProperty* property = findTiledProperty(properties, name);
    if (property == nullptr) return fallback;
    if (const double* value = std::get_if<double>(&property->value)) {
        if (*value >= static_cast<double>(std::numeric_limits<int>::min()) &&
            *value <= static_cast<double>(std::numeric_limits<int>::max())) {
            return static_cast<int>(*value);
        }
    }
    return fallback;
}

} // namespace nundertale
