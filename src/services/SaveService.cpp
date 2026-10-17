#include "services/SaveService.hpp"
#include "core/GameContext.hpp"
#include "core/Fur.hpp"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>

namespace nundertale {

static std::string extractJsonString(const std::string& json, const std::string& key) {
    std::string needle = "\"" + key + "\":";
    size_t pos = json.find(needle);
    if (pos == std::string::npos) return "";
    pos += needle.size();
    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t' || json[pos] == '\n' || json[pos] == '\r')) pos++;
    if (pos >= json.size() || json[pos] != '\"') return "";
    pos++;
    size_t end = json.find('\"', pos);
    if (end == std::string::npos) return "";
    return json.substr(pos, end - pos);
}

static double extractJsonNumber(const std::string& json, const std::string& key, double defaultVal = 0.0) {
    std::string needle = "\"" + key + "\":";
    size_t pos = json.find(needle);
    if (pos == std::string::npos) return defaultVal;
    pos += needle.size();
    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t' || json[pos] == '\n' || json[pos] == '\r')) pos++;
    size_t end = pos;
    while (end < json.size() && (std::isdigit(json[end]) || json[end] == '.' || json[end] == '-')) end++;
    if (end > pos) {
        try {
            return std::stod(json.substr(pos, end - pos));
        } catch (...) {
            return defaultVal;
        }
    }
    return defaultVal;
}

SaveService::SaveService() {
    const char* localAppData = std::getenv("LOCALAPPDATA");
    if (localAppData && localAppData[0] != '\0') {
        m_saveDir = std::filesystem::path(localAppData) / "nUndertale";
    } else {
        m_saveDir = std::filesystem::path("saves");
    }

    m_savePath = m_saveDir / "file0.json";
    m_backupPath = m_saveDir / "file0.bak";
    m_configPath = m_saveDir / "config.json";
    m_metaPath = m_saveDir / "meta.json";

    ensureDirectoryExists();
    loadConfig(m_userConfig);
}

void SaveService::ensureDirectoryExists() {
    std::error_code ec;
    if (!std::filesystem::exists(m_saveDir, ec)) {
        std::filesystem::create_directories(m_saveDir, ec);
    }
}

std::string SaveService::getSaveFilePath() const {
    return m_savePath.string();
}

bool SaveService::hasSaveGame() const {
    std::error_code ec;
    return std::filesystem::exists(m_savePath, ec) && std::filesystem::file_size(m_savePath, ec) > 0;
}

bool SaveService::deleteSaveGame() {
    std::error_code ec;
    bool res = false;
    if (std::filesystem::exists(m_savePath, ec)) {
        res = std::filesystem::remove(m_savePath, ec);
    }
    // SAV-005: Meta persistence - record reset count
    MetaData meta = getMeta();
    meta.resets++;
    saveMeta(meta);
    return res;
}

bool SaveService::saveMeta(const MetaData& meta) {
    ensureDirectoryExists();
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"resets\": " << meta.resets << ",\n";
    ss << "  \"everKilled\": " << (meta.everKilled ? "true" : "false") << ",\n";
    ss << "  \"timesTransformed\": " << meta.timesTransformed << ",\n";
    ss << "  \"lastClearedRoute\": \"" << meta.lastClearedRoute << "\"\n";
    ss << "}\n";

    std::filesystem::path tempPath = m_saveDir / "meta.tmp";
    std::ofstream out(tempPath);
    if (!out.is_open()) return false;
    out << ss.str();
    out.close();

    std::error_code ec;
    std::filesystem::rename(tempPath, m_metaPath, ec);
    return !ec;
}

MetaData SaveService::getMeta() const {
    MetaData meta;
    std::error_code ec;
    if (!std::filesystem::exists(m_metaPath, ec)) {
        return meta;
    }
    std::ifstream file(m_metaPath);
    if (!file.is_open()) return meta;
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string json = buffer.str();

    meta.resets = (int)extractJsonNumber(json, "resets", 0.0);
    meta.everKilled = (json.find("\"everKilled\": true") != std::string::npos || json.find("\"everKilled\":true") != std::string::npos);
    meta.timesTransformed = (int)extractJsonNumber(json, "timesTransformed", 0.0);
    std::string route = extractJsonString(json, "lastClearedRoute");
    if (!route.empty()) meta.lastClearedRoute = route;

    return meta;
}

std::string SaveService::getRoomDisplayName(const std::string& roomId) const {
    if (roomId == "ruins_entrance" || roomId.empty()) {
        return "Ruins - Entrance";
    } else if (roomId == "ruins_hall") {
        return "Ruins - Leaf Pile";
    } else if (roomId == "ruins_furiell") {
        return "Ruins - Home";
    } else if (roomId == "snowdin_town") {
        return "Snowdinha - Town";
    }
    return roomId;
}

std::string SaveService::serializeStateToJson(const GameState& state) const {
    std::ostringstream ss;
    ss << "{\n";
    ss << "  \"version\": 1,\n";
    ss << "  \"player\": {\n";
    ss << "    \"name\": \"" << state.player.name << "\",\n";
    ss << "    \"hp\": " << state.player.hp << ",\n";
    ss << "    \"maxHp\": " << state.player.maxHp << ",\n";
    ss << "    \"level\": " << state.player.level << ",\n";
    ss << "    \"exp\": " << state.player.exp << ",\n";
    ss << "    \"gold\": " << state.player.gold << ",\n";
    ss << "    \"at\": " << state.player.at << ",\n";
    ss << "    \"df\": " << state.player.df << ",\n";
    ss << "    \"weapon\": \"" << state.player.weapon << "\",\n";
    ss << "    \"armor\": \"" << state.player.armor << "\",\n";
    ss << "    \"weaponAt\": " << state.player.weaponAt << ",\n";
    ss << "    \"armorDf\": " << state.player.armorDf << ",\n";
    ss << "    \"inventory\": [";
    for (size_t i = 0; i < state.player.inventory.size(); ++i) {
        ss << "\"" << state.player.inventory[i] << "\"";
        if (i + 1 < state.player.inventory.size()) ss << ", ";
    }
    ss << "]\n";
    ss << "  },\n";

    ss << "  \"furry\": {\n";
    ss << "    \"value\": " << state.furry.value << ",\n";
    ss << "    \"stageMax\": " << state.furry.stageMax << ",\n";
    ss << "    \"transformations\": " << state.furry.transformations << "\n";
    ss << "  },\n";

    ss << "  \"currentRoom\": \"" << state.currentRoom << "\",\n";
    ss << "  \"playerPos\": { \"x\": " << state.playerPos.x << ", \"y\": " << state.playerPos.y << " },\n";
    ss << "  \"playerFacing\": " << state.playerFacing << ",\n";
    ss << "  \"playTime\": " << state.playTime << ",\n";
    ss << "  \"killsTotal\": " << state.killsTotal << ",\n";

    ss << "  \"flags\": {\n";
    size_t flagCount = 0;
    for (const auto& [k, v] : state.flags) {
        ss << "    \"" << k << "\": " << v;
        if (++flagCount < state.flags.size()) ss << ",";
        ss << "\n";
    }
    ss << "  }\n";
    ss << "}\n";
    return ss.str();
}

bool SaveService::parseStateFromJson(const std::string& json, GameState& state) {
    if (json.empty() || json.find('{') == std::string::npos) return false;

    std::string pName = extractJsonString(json, "name");
    if (!pName.empty()) state.player.name = pName;

    state.player.hp = (int)extractJsonNumber(json, "hp", 20);
    state.player.maxHp = (int)extractJsonNumber(json, "maxHp", 20);
    state.player.level = (int)extractJsonNumber(json, "level", 1);
    state.player.exp = (int)extractJsonNumber(json, "exp", 0);
    state.player.gold = (int)extractJsonNumber(json, "gold", 0);
    state.player.at = (int)extractJsonNumber(json, "at", 0);
    state.player.df = (int)extractJsonNumber(json, "df", 0);
    state.player.weaponAt = (int)extractJsonNumber(json, "weaponAt", 2);
    state.player.armorDf = (int)extractJsonNumber(json, "armorDf", 1);

    std::string wep = extractJsonString(json, "weapon");
    if (!wep.empty()) state.player.weapon = wep;
    std::string arm = extractJsonString(json, "armor");
    if (!arm.empty()) state.player.armor = arm;

    size_t furryPos = json.find("\"furry\":");
    std::string furryJson;
    if (furryPos != std::string::npos) {
        size_t openBrace = json.find('{', furryPos);
        size_t closeBrace = json.find('}', openBrace);
        if (openBrace != std::string::npos && closeBrace != std::string::npos) {
            furryJson = json.substr(openBrace, closeBrace - openBrace + 1);
        }
    }

    const bool hasNewFurValue = furryJson.find("\"value\":") != std::string::npos;
    if (hasNewFurValue) {
        state.furry.value = std::clamp(
            static_cast<int>(std::lround(extractJsonNumber(furryJson, "value", 0.0))),
            0,
            FUR_MAX);
    } else {
        const double playerMeter = extractJsonNumber(json, "furryMeter", 0.0);
        const double furryMeter = extractJsonNumber(furryJson, "meter", 0.0);
        state.furry.value = std::clamp(
            static_cast<int>(std::lround(std::max(playerMeter, furryMeter))),
            0,
            FUR_MAX);
    }

    const int savedStage = hasNewFurValue
        ? static_cast<int>(extractJsonNumber(furryJson, "stageMax", 0.0))
        : static_cast<int>(extractJsonNumber(furryJson, "stage", 0.0));
    const int valueStage = static_cast<int>(furStageFor(state.furry.value));
    state.furry.stageMax = std::clamp(
        std::max(savedStage, valueStage),
        0,
        static_cast<int>(FurStage::Mirror));
    state.furry.transformations = static_cast<int>(extractJsonNumber(furryJson, "transformations", 0.0));

    std::string room = extractJsonString(json, "currentRoom");
    if (!room.empty()) state.currentRoom = room;

    state.playerFacing = (int)extractJsonNumber(json, "playerFacing", 0);
    state.playTime = (float)extractJsonNumber(json, "playTime", 0.0);
    state.killsTotal = (int)extractJsonNumber(json, "killsTotal", 0);

    // Extract inventory
    size_t invPos = json.find("\"inventory\":");
    if (invPos != std::string::npos) {
        size_t openBracket = json.find('[', invPos);
        size_t closeBracket = json.find(']', openBracket);
        if (openBracket != std::string::npos && closeBracket != std::string::npos) {
            state.player.inventory.clear();
            std::string invSlice = json.substr(openBracket + 1, closeBracket - openBracket - 1);
            size_t cur = 0;
            while (cur < invSlice.size()) {
                size_t q1 = invSlice.find('\"', cur);
                if (q1 == std::string::npos) break;
                size_t q2 = invSlice.find('\"', q1 + 1);
                if (q2 == std::string::npos) break;
                state.player.inventory.push_back(invSlice.substr(q1 + 1, q2 - q1 - 1));
                cur = q2 + 1;
            }
        }
    }

    // Extract playerPos
    size_t posPos = json.find("\"playerPos\":");
    if (posPos != std::string::npos) {
        state.playerPos.x = (float)extractJsonNumber(json.substr(posPos), "x", 320.0);
        state.playerPos.y = (float)extractJsonNumber(json.substr(posPos), "y", 240.0);
    }

    // Extract flags
    size_t flagsPos = json.find("\"flags\":");
    if (flagsPos != std::string::npos) {
        size_t openBrace = json.find('{', flagsPos);
        size_t closeBrace = json.find('}', openBrace);
        if (openBrace != std::string::npos && closeBrace != std::string::npos) {
            std::string flagsSlice = json.substr(openBrace + 1, closeBrace - openBrace - 1);
            size_t cur = 0;
            while (cur < flagsSlice.size()) {
                size_t q1 = flagsSlice.find('\"', cur);
                if (q1 == std::string::npos) break;
                size_t q2 = flagsSlice.find('\"', q1 + 1);
                if (q2 == std::string::npos) break;
                std::string flagName = flagsSlice.substr(q1 + 1, q2 - q1 - 1);
                size_t colon = flagsSlice.find(':', q2 + 1);
                if (colon == std::string::npos) break;
                size_t numStart = colon + 1;
                while (numStart < flagsSlice.size() && (flagsSlice[numStart] == ' ' || flagsSlice[numStart] == '\t')) numStart++;
                size_t numEnd = numStart;
                while (numEnd < flagsSlice.size() && (std::isdigit(flagsSlice[numEnd]) || flagsSlice[numEnd] == '-')) numEnd++;
                if (numEnd > numStart) {
                    try {
                        int val = std::stoi(flagsSlice.substr(numStart, numEnd - numStart));
                        state.flags[flagName] = val;
                    } catch (...) {}
                }
                cur = numEnd + 1;
            }
        }
    }

    return true;
}

bool SaveService::saveGame(const GameState& state) {
    ensureDirectoryExists();
    std::filesystem::path tmpPath = m_saveDir / "file0.tmp";

    std::string jsonStr = serializeStateToJson(state);

    {
        std::ofstream out(tmpPath, std::ios::out | std::ios::trunc);
        if (!out.is_open()) return false;
        out << jsonStr;
        out.close();
    }

    std::error_code ec;
    // Create backup of existing save (SAV-006)
    if (std::filesystem::exists(m_savePath, ec)) {
        std::filesystem::copy_file(m_savePath, m_backupPath, std::filesystem::copy_options::overwrite_existing, ec);
    }

    // Atomic rename tmp -> savePath
    std::filesystem::rename(tmpPath, m_savePath, ec);
    return !ec;
}

bool SaveService::loadGame(GameState& state) {
    ensureDirectoryExists();
    std::error_code ec;

    auto tryLoad = [this, &state](const std::filesystem::path& p) -> bool {
        std::ifstream in(p);
        if (!in.is_open()) return false;
        std::stringstream buffer;
        buffer << in.rdbuf();
        return parseStateFromJson(buffer.str(), state);
    };

    if (std::filesystem::exists(m_savePath, ec) && tryLoad(m_savePath)) {
        return true;
    }

    // Corrupted save recovery fallback (SAV-006)
    if (std::filesystem::exists(m_backupPath, ec) && tryLoad(m_backupPath)) {
        std::cout << "[SaveService] Main save corrupted! Recovered from file0.bak" << std::endl;
        return true;
    }

    return false;
}

bool SaveService::saveConfig(const UserConfig& config) {
    ensureDirectoryExists();
    m_userConfig = config;

    std::filesystem::path tmpPath = m_saveDir / "config.tmp";

    std::ostringstream ss;
    ss << "{\n";
    ss << "  \"musicVolume\": " << config.musicVolume << ",\n";
    ss << "  \"sfxVolume\": " << config.sfxVolume << ",\n";
    ss << "  \"masterVolume\": " << config.masterVolume << ",\n";
    ss << "  \"fullscreen\": " << (config.fullscreen ? "true" : "false") << ",\n";
    ss << "  \"integerScale\": " << (config.integerScale ? "true" : "false") << ",\n";
    ss << "  \"spareDing\": " << (config.spareDing ? "true" : "false") << ",\n";
    ss << "  \"lang\": \"" << config.lang << "\"\n";
    ss << "}\n";

    {
        std::ofstream out(tmpPath, std::ios::out | std::ios::trunc);
        if (!out.is_open()) return false;
        out << ss.str();
        out.close();
    }

    std::error_code ec;
    std::filesystem::rename(tmpPath, m_configPath, ec);
    return !ec;
}

bool SaveService::loadConfig(UserConfig& config) {
    ensureDirectoryExists();
    std::error_code ec;
    if (!std::filesystem::exists(m_configPath, ec)) {
        config = m_userConfig;
        return false;
    }

    std::ifstream in(m_configPath);
    if (!in.is_open()) return false;
    std::stringstream buffer;
    buffer << in.rdbuf();
    std::string content = buffer.str();

    auto extractFloat = [&content](const std::string& key, float defaultVal) -> float {
        auto pos = content.find("\"" + key + "\"");
        if (pos == std::string::npos) return defaultVal;
        pos = content.find(':', pos);
        if (pos == std::string::npos) return defaultVal;
        try {
            return std::stof(content.substr(pos + 1));
        } catch (...) {
            return defaultVal;
        }
    };
    auto extractBool = [&content](const std::string& key, bool defaultVal) -> bool {
        auto pos = content.find("\"" + key + "\"");
        if (pos == std::string::npos) return defaultVal;
        pos = content.find(':', pos);
        if (pos == std::string::npos) return defaultVal;
        std::string sub = content.substr(pos + 1, 12);
        if (sub.find("true") != std::string::npos) return true;
        if (sub.find("false") != std::string::npos) return false;
        return defaultVal;
    };
    auto extractString = [&content](const std::string& key, const std::string& defaultVal) -> std::string {
        auto pos = content.find("\"" + key + "\"");
        if (pos == std::string::npos) return defaultVal;
        pos = content.find(':', pos);
        if (pos == std::string::npos) return defaultVal;
        auto startQuote = content.find('"', pos);
        if (startQuote == std::string::npos) return defaultVal;
        auto endQuote = content.find('"', startQuote + 1);
        if (endQuote == std::string::npos) return defaultVal;
        return content.substr(startQuote + 1, endQuote - startQuote - 1);
    };

    config.musicVolume = extractFloat("musicVolume", 0.8f);
    config.sfxVolume = extractFloat("sfxVolume", 1.0f);
    config.masterVolume = extractFloat("masterVolume", 1.0f);
    config.fullscreen = extractBool("fullscreen", false);
    config.integerScale = extractBool("integerScale", true);
    config.spareDing = extractBool("spareDing", true);
    config.lang = extractString("lang", "en");

    m_userConfig = config;
    return true;
}

} // namespace nundertale
