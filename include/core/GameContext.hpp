#pragma once

#include <string>
#include <vector>
#include "services/IAudioService.hpp"
#include "services/IResourceService.hpp"
#include "services/IInputService.hpp"

namespace nundertale {

class SceneManager;

struct PlayerData {
    std::string name = "PEPE";
    int hp = 20;
    int maxHp = 20;
    int level = 1;
    int exp = 0;
    int gold = 10;
    float furryMeter = 0.0f; // 0.0 to 100.0%
    bool isChad = false;
    bool isSkyrim = false;
    bool isWaifuFan = false;
    std::vector<std::string> inventory = { "Mt. Dew", "Doritos" };
};

struct GameContext {
    IAudioService& audio;
    IResourceService& resources;
    IInputService& input;
    SceneManager& scenes;
    PlayerData player;
    bool isRunning = true;

    GameContext(IAudioService& a, IResourceService& r, IInputService& i, SceneManager& s)
        : audio(a), resources(r), input(i), scenes(s) {}
};

} // namespace nundertale
