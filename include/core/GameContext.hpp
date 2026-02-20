#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <raylib.h>
#include "services/IAudioService.hpp"
#include "services/IResourceService.hpp"
#include "services/IInputService.hpp"
#include "services/ISaveService.hpp"

namespace nundertale {

class SceneManager;

struct PlayerData {
    std::string name = "PEPE";
    int hp = 20;
    int maxHp = 20;
    int level = 1;
    int exp = 0;
    int gold = 10;
    int at = 0;
    int df = 0;
    int weaponAt = 2;
    int armorDf = 1;
    std::string weapon = "Frayed USB Mouse";
    std::string armor = "Broken Headset";
    float furryMeter = 0.0f; // 0.0 to 100.0%
    bool isChad = false;
    bool isSkyrim = false;
    bool isWaifuFan = false;
    std::vector<std::string> inventory = { "Mt. Dew", "Doritos" };
};

struct FurryState {
    float meter = 0.0f; // 0.0 to 100.0%
    int transformations = 0;
    int stage = 0; // 0: human-frog, 1: ears, 2: paws, 3: tail, 4: snout, 5: full
};

struct GameState {
    PlayerData player;
    FurryState furry;
    std::unordered_map<std::string, int> flags;
    std::string currentRoom = "ruins_entrance";
    Vector2 playerPos = { 320.0f, 240.0f };
    int playerFacing = 0; // 0: Down, 1: Up, 2: Left, 3: Right
    float playTime = 0.0f; // in seconds
    int killsTotal = 0;
    std::unordered_map<std::string, int> killsByArea;

    // Flag system helpers (CORE-010)
    int getFlag(const std::string& key, int defaultVal = 0) const {
        auto it = flags.find(key);
        return (it != flags.end()) ? it->second : defaultVal;
    }

    void setFlag(const std::string& key, int value) {
        flags[key] = value;
    }

    int incFlag(const std::string& key, int delta = 1) {
        return flags[key] += delta;
    }

    bool hasFlag(const std::string& key) const {
        auto it = flags.find(key);
        return it != flags.end() && it->second != 0;
    }
};

struct GameContext {
    IAudioService& audio;
    IResourceService& resources;
    IInputService& input;
    SceneManager& scenes;
    ISaveService& save;
    GameState state;
    PlayerData& player;
    bool isRunning = true;

    GameContext(IAudioService& a, IResourceService& r, IInputService& i, SceneManager& s, ISaveService& sv)
        : audio(a), resources(r), input(i), scenes(s), save(sv), player(state.player) {}
};

} // namespace nundertale
