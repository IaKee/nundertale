#pragma once

#include "services/ISaveService.hpp"
#include <string>
#include <filesystem>

namespace nundertale {

class SaveService : public ISaveService {
public:
    SaveService();
    ~SaveService() override = default;

    bool saveGame(const GameState& state) override;
    bool loadGame(GameState& state) override;
    bool hasSaveGame() const override;
    bool deleteSaveGame() override;

    bool saveConfig(const UserConfig& config) override;
    bool loadConfig(UserConfig& config) override;
    UserConfig getConfig() const override { return m_userConfig; }

    bool saveMeta(const MetaData& meta) override;
    MetaData getMeta() const override;

    std::string getSaveFilePath() const override;
    std::string getRoomDisplayName(const std::string& roomId) const override;

private:
    std::filesystem::path m_saveDir;
    std::filesystem::path m_savePath;
    std::filesystem::path m_backupPath;
    std::filesystem::path m_configPath;
    std::filesystem::path m_metaPath;
    UserConfig m_userConfig;

    void ensureDirectoryExists();
    bool parseStateFromJson(const std::string& jsonContent, GameState& state);
    std::string serializeStateToJson(const GameState& state) const;
};

} // namespace nundertale
