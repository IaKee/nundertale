#pragma once

#include "combat/Enemy.hpp"
#include "combat/GenericPatterns.hpp"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cctype>
#include <algorithm>

namespace nundertale {

struct EnemyData {
    std::string id = "barky";
    std::string name = "Barky";
    int hp = 30;
    int maxHp = 30;
    int atk = 4;
    int def = 2;
    int exp = 20;
    int gold = 10;
    int mercyLockedAbovePct = 0;
    std::string sprite = "barky";
    std::string check = "Alpha Wolf of the Ruins. The zipper says otherwise.";
    std::vector<ActDef> acts;
    std::vector<std::string> bubbles;
    std::vector<GenericPatternDef> patterns;
    std::vector<std::string> defaultFlavor;
    std::vector<std::string> lowHpFlavor;
    std::vector<std::string> spareableFlavor;
    std::vector<std::string> defaultDialogue;
    std::vector<std::string> afterHitDialogue;
};

struct EncounterData {
    std::string id;
    std::string enemyId;
    std::string background = "grid_tube";
    std::string music = "default";
    std::string intro;
    bool canFlee = true;
};

inline std::string unescapeJsonString(const std::string& raw) {
    std::string out;
    out.reserve(raw.size());
    for (size_t i = 0; i < raw.size(); ++i) {
        if (raw[i] == '\\' && i + 1 < raw.size()) {
            char n = raw[++i];
            if (n == 'n') out += '\n';
            else if (n == 't') out += '\t';
            else if (n == 'r') out += '\r';
            else out += n;
        } else {
            out += raw[i];
        }
    }
    return out;
}

inline std::string extractJsonStringField(const std::string& json, const std::string& key) {
    std::string needle = "\"" + key + "\":";
    size_t pos = json.find(needle);
    if (pos == std::string::npos) return "";
    pos += needle.size();
    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t' || json[pos] == '\n' || json[pos] == '\r')) pos++;
    if (pos >= json.size() || json[pos] != '\"') return "";
    pos++;
    size_t end = pos;
    while (end < json.size()) {
        if (json[end] == '\"' && json[end - 1] != '\\') break;
        end++;
    }
    if (end >= json.size()) return "";
    return unescapeJsonString(json.substr(pos, end - pos));
}

inline int extractJsonIntField(const std::string& json, const std::string& key, int defaultVal = 0) {
    std::string needle = "\"" + key + "\":";
    size_t pos = json.find(needle);
    if (pos == std::string::npos) return defaultVal;
    pos += needle.size();
    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t' || json[pos] == '\n' || json[pos] == '\r')) pos++;
    size_t end = pos;
    if (end < json.size() && json[end] == '-') end++;
    while (end < json.size() && std::isdigit(static_cast<unsigned char>(json[end]))) end++;
    if (end > pos) {
        try {
            return std::stoi(json.substr(pos, end - pos));
        } catch (...) {}
    }
    return defaultVal;
}

inline float extractJsonFloatField(const std::string& json, const std::string& key, float defaultVal = 0.0f) {
    std::string needle = "\"" + key + "\":";
    size_t pos = json.find(needle);
    if (pos == std::string::npos) return defaultVal;
    pos += needle.size();
    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t' || json[pos] == '\n' || json[pos] == '\r')) pos++;
    size_t end = pos;
    if (end < json.size() && (json[end] == '-' || json[end] == '+')) end++;
    while (end < json.size() && (std::isdigit(static_cast<unsigned char>(json[end])) || json[end] == '.')) end++;
    if (end > pos) {
        try {
            return std::stof(json.substr(pos, end - pos));
        } catch (...) {}
    }
    return defaultVal;
}

inline bool extractJsonBoolField(const std::string& json, const std::string& key, bool defaultVal = false) {
    std::string needle = "\"" + key + "\":";
    size_t pos = json.find(needle);
    if (pos == std::string::npos) return defaultVal;
    pos += needle.size();
    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t' || json[pos] == '\n' || json[pos] == '\r')) pos++;
    if (json.compare(pos, 4, "true") == 0) return true;
    if (json.compare(pos, 5, "false") == 0) return false;
    return defaultVal;
}

inline std::vector<std::string> extractJsonStringArrayField(const std::string& json, const std::string& key) {
    std::vector<std::string> result;
    std::string needle = "\"" + key + "\":";
    size_t pos = json.find(needle);
    if (pos == std::string::npos) return result;
    size_t openBracket = json.find('[', pos);
    if (openBracket == std::string::npos) return result;

    int depth = 0;
    bool inString = false;
    bool escaped = false;
    size_t closeBracket = std::string::npos;
    for (size_t p = openBracket; p < json.size(); ++p) {
        char ch = json[p];
        if (inString) {
            if (escaped) escaped = false;
            else if (ch == '\\') escaped = true;
            else if (ch == '\"') inString = false;
            continue;
        }
        if (ch == '\"') inString = true;
        else if (ch == '[') ++depth;
        else if (ch == ']') {
            if (--depth == 0) {
                closeBracket = p;
                break;
            }
        }
    }
    if (closeBracket == std::string::npos) return result;

    std::string slice = json.substr(openBracket + 1, closeBracket - openBracket - 1);
    size_t cur = 0;
    while (cur < slice.size()) {
        size_t q1 = slice.find('\"', cur);
        if (q1 == std::string::npos) break;
        size_t q2 = q1 + 1;
        while (q2 < slice.size()) {
            if (slice[q2] == '\"' && slice[q2 - 1] != '\\') break;
            q2++;
        }
        if (q2 >= slice.size()) break;
        result.push_back(unescapeJsonString(slice.substr(q1 + 1, q2 - q1 - 1)));
        cur = q2 + 1;
    }
    return result;
}

inline std::vector<std::string> extractJsonObjectArrayField(const std::string& json, const std::string& key) {
    std::vector<std::string> result;
    std::string needle = "\"" + key + "\":";
    size_t pos = json.find(needle);
    if (pos == std::string::npos) return result;
    size_t openBracket = json.find('[', pos);
    if (openBracket == std::string::npos) return result;

    int depth = 0;
    bool inString = false;
    bool escaped = false;
    size_t closeBracket = std::string::npos;
    for (size_t p = openBracket; p < json.size(); ++p) {
        char ch = json[p];
        if (inString) {
            if (escaped) escaped = false;
            else if (ch == '\\') escaped = true;
            else if (ch == '\"') inString = false;
            continue;
        }
        if (ch == '\"') inString = true;
        else if (ch == '[') ++depth;
        else if (ch == ']') {
            if (--depth == 0) {
                closeBracket = p;
                break;
            }
        }
    }
    if (closeBracket == std::string::npos) return result;

    size_t cur = openBracket + 1;
    while (cur < closeBracket) {
        size_t objStart = json.find('{', cur);
        if (objStart == std::string::npos || objStart >= closeBracket) break;
        std::string obj = jsonObjectAt(json, objStart);
        if (obj.empty()) break;
        result.push_back(obj);
        cur = objStart + obj.size();
    }
    return result;
}

inline EnemyData parseEnemyJson(const std::string& json) {
    EnemyData data;
    data.id = extractJsonStringField(json, "id");
    data.name = extractJsonStringField(json, "name");
    if (data.name.empty()) data.name = "Barky";
    if (data.id.empty()) data.id = data.name;

    data.hp = extractJsonIntField(json, "hp", 30);
    data.maxHp = extractJsonIntField(json, "maxHp", data.hp);
    data.atk = extractJsonIntField(json, "atk", 4);
    data.def = extractJsonIntField(json, "def", 2);
    data.exp = extractJsonIntField(json, "exp", 20);
    data.gold = extractJsonIntField(json, "gold", 10);
    data.mercyLockedAbovePct = extractJsonIntField(json, "mercyLockedAbovePct", 0);

    data.sprite = extractJsonStringField(json, "sprite");
    if (data.sprite.empty()) data.sprite = data.id;

    data.check = extractJsonStringField(json, "check");

    // Parse ACTs
    std::vector<std::string> actObjs = extractJsonObjectArrayField(json, "acts");
    for (const auto& actStr : actObjs) {
        ActDef act;
        act.id = extractJsonStringField(actStr, "id");
        act.label = extractJsonStringField(actStr, "label");
        act.text = extractJsonStringField(actStr, "text");
        act.repeatText = extractJsonStringField(actStr, "repeatText");
        act.response = extractJsonStringField(actStr, "response");
        act.repeatResponse = extractJsonStringField(actStr, "repeatResponse");
        act.requiresItem = extractJsonStringField(actStr, "requiresItem");

        size_t effPos = actStr.find("\"effects\":");
        if (effPos != std::string::npos) {
            size_t effOpen = actStr.find('{', effPos);
            if (effOpen != std::string::npos) {
                std::string effObj = jsonObjectAt(actStr, effOpen);
                act.spareable = extractJsonBoolField(effObj, "spare", false) ||
                                extractJsonBoolField(effObj, "spareable", false);
                act.flee = extractJsonBoolField(effObj, "flee", false);
                act.spareNow = extractJsonBoolField(effObj, "spareNow", false);
                act.skipAttack = extractJsonBoolField(effObj, "skipAttack", false);
                act.fur = extractJsonIntField(effObj, "fur", 0);
                act.mercy = extractJsonIntField(effObj, "mercy", 0);
                act.atkDelta = extractJsonIntField(effObj, "atkDelta", 0);
                act.defDelta = extractJsonIntField(effObj, "defDelta", 0);
                act.freezeTurns = extractJsonIntField(effObj, "freezeTurns", act.skipAttack ? 1 : 0);
            }
        }
        data.acts.push_back(act);
    }

    // Parse bubbles
    data.bubbles = extractJsonStringArrayField(json, "bubbles");

    // Parse patterns
    std::vector<std::string> patternObjs = extractJsonObjectArrayField(json, "patterns");
    for (const auto& patStr : patternObjs) {
        GenericPatternDef pat;
        pat.id = extractJsonStringField(patStr, "id");
        pat.kind = extractJsonStringField(patStr, "kind");
        if (pat.kind.empty()) pat.kind = "rain";
        pat.count = extractJsonIntField(patStr, "count", 4);
        pat.speed = extractJsonFloatField(patStr, "speed", 80.0f);
        pat.duration = extractJsonFloatField(patStr, "duration", 5.0f);
        pat.damage = extractJsonIntField(patStr, "damage", 3);
        pat.fur = extractJsonIntField(patStr, "fur", 3);
        pat.bullet = extractJsonStringField(patStr, "bullet");
        if (pat.bullet.empty()) pat.bullet = ":3";
        data.patterns.push_back(pat);
    }

    // Parse flavor pools
    size_t flavorPos = json.find("\"flavor\":");
    if (flavorPos != std::string::npos) {
        size_t flavorOpen = json.find('{', flavorPos);
        if (flavorOpen != std::string::npos) {
            std::string flavorSlice = jsonObjectAt(json, flavorOpen);
            data.defaultFlavor = extractJsonStringArrayField(flavorSlice, "default");
            data.lowHpFlavor = extractJsonStringArrayField(flavorSlice, "lowHp");
            data.spareableFlavor = extractJsonStringArrayField(flavorSlice, "spareable");
        }
    }

    // Parse dialogue pools
    size_t diagPos = json.find("\"dialogue\":");
    if (diagPos != std::string::npos) {
        size_t diagOpen = json.find('{', diagPos);
        if (diagOpen != std::string::npos) {
            std::string diagSlice = jsonObjectAt(json, diagOpen);
            data.defaultDialogue = extractJsonStringArrayField(diagSlice, "default");
            data.afterHitDialogue = extractJsonStringArrayField(diagSlice, "afterHit");
        }
    }

    return data;
}

inline EnemyData loadEnemyDataFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        EnemyData def;
        return def;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return parseEnemyJson(buffer.str());
}

inline EncounterData parseEncounterJson(const std::string& json, const std::string& fallbackId = "barky") {
    EncounterData data;
    data.id = extractJsonStringField(json, "id");
    if (data.id.empty()) data.id = fallbackId;

    data.enemyId = extractJsonStringField(json, "enemy");
    if (data.enemyId.empty()) {
        std::vector<std::string> enemies = extractJsonStringArrayField(json, "enemies");
        if (!enemies.empty()) {
            data.enemyId = enemies[0];
        } else {
            data.enemyId = data.id;
        }
    }

    data.background = extractJsonStringField(json, "background");
    if (data.background.empty()) data.background = "grid_tube";

    data.music = extractJsonStringField(json, "music");
    if (data.music.empty()) data.music = "default";

    data.intro = extractJsonStringField(json, "intro");
    if (data.intro.empty()) {
        data.intro = extractJsonStringField(json, "introText");
    }

    data.canFlee = extractJsonBoolField(json, "canFlee", true);
    return data;
}

inline EncounterData loadEncounterDataFromFile(const std::string& filepath, const std::string& fallbackId = "barky") {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        EncounterData def;
        def.id = fallbackId;
        def.enemyId = fallbackId;
        return def;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return parseEncounterJson(buffer.str(), fallbackId);
}

} // namespace nundertale
