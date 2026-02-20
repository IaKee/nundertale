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

    std::string getSaveFilePath() const override;
    std::string getRoomDisplayName(const std::string& roomId) const override;

private:
    std::filesystem::path m_saveDir;
    std::filesystem::path m_savePath;
    std::filesystem::path m_backupPath;

    void ensureDirectoryExists();
    bool parseStateFromJson(const std::string& jsonContent, GameState& state);
    std::string serializeStateToJson(const GameState& state) const;
};

} // namespace nundertale
