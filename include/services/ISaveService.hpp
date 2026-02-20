#pragma once

#include <string>

namespace nundertale {

struct GameState;

class ISaveService {
public:
    virtual ~ISaveService() = default;

    virtual bool saveGame(const GameState& state) = 0;
    virtual bool loadGame(GameState& state) = 0;
    virtual bool hasSaveGame() const = 0;
    virtual bool deleteSaveGame() = 0;

    virtual std::string getSaveFilePath() const = 0;
    virtual std::string getRoomDisplayName(const std::string& roomId) const = 0;
};

} // namespace nundertale
