#pragma once

#include <string>

namespace nundertale {

struct GameState;

struct UserConfig {
    float musicVolume = 0.8f;
    float sfxVolume = 1.0f;
    float masterVolume = 1.0f;
    bool fullscreen = false;
    bool integerScale = true;
    std::string lang = "en";
};

class ISaveService {
public:
    virtual ~ISaveService() = default;

    virtual bool saveGame(const GameState& state) = 0;
    virtual bool loadGame(GameState& state) = 0;
    virtual bool hasSaveGame() const = 0;
    virtual bool deleteSaveGame() = 0;

    virtual bool saveConfig(const UserConfig& config) = 0;
    virtual bool loadConfig(UserConfig& config) = 0;
    virtual UserConfig getConfig() const = 0;

    virtual std::string getSaveFilePath() const = 0;
    virtual std::string getRoomDisplayName(const std::string& roomId) const = 0;
};

} // namespace nundertale
