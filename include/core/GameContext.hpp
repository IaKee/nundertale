#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <raylib.h>
#include "services/IAudioService.hpp"
#include "services/IResourceService.hpp"
#include "services/IInputService.hpp"
#include "services/ISaveService.hpp"
#include "core/Fur.hpp"
#include "core/ItemDatabase.hpp"

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
    std::string armor = "One-Sided Broken Headset";
    bool isChad = false;
    bool isSkyrim = false;
    bool isWaifuFan = false;
    static constexpr size_t MAX_INVENTORY_SIZE = 8; // ITM-004: Undertale 8-slot limit
    std::vector<std::string> inventory = { "Bottle", "Mt. Dew", "Doritos" };

    bool canAddItem() const { return inventory.size() < MAX_INVENTORY_SIZE; }
    bool addItem(const std::string& item) {
        if (!canAddItem()) return false;
        inventory.push_back(item);
        return true;
    }
};

struct FurryState {
    int value = 0;
    int stageMax = 0;
    int transformations = 0;
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
    int sparesTotal = 0;
    int fleesTotal = 0;
    std::unordered_map<std::string, int> killsByArea;

    enum class RouteType {
        Wholesome,   // 0 kills
        Normie,      // some kills
        ToxicGamer   // exhausted ruins kill quota (>= 20)
    };

    RouteType getRoute() const {
        if (killsTotal == 0) return RouteType::Wholesome;
        auto it = killsByArea.find("ruins");
        if (it != killsByArea.end() && it->second >= 20) {
            return RouteType::ToxicGamer;
        }
        return RouteType::Normie;
    }

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
    ItemDatabase itemDatabase;
    GameState state;
    PlayerData& player;
    bool isRunning = true;
    bool debugHitboxes = false; // QA-002: F2 hitbox overlay toggle

    // GFX-006: Screen Shake
    float shakeIntensity = 0.0f;
    float shakeTimer = 0.0f;
    Vector2 shakeOffset{ 0.0f, 0.0f };

    void shake(float intensity, float duration) {
        shakeIntensity = intensity;
        shakeTimer = duration;
    }

    // GFX-007: Flash & Tint overlay
    Color flashColor = WHITE;
    float flashTimer = 0.0f;
    float flashDuration = 0.0f;

    void flash(Color color, float duration) {
        flashColor = color;
        flashDuration = duration;
        flashTimer = duration;
    }

    GameContext(IAudioService& a, IResourceService& r, IInputService& i, SceneManager& s, ISaveService& sv)
        : audio(a), resources(r), input(i), scenes(s), save(sv), player(state.player) {}
};

} // namespace nundertale
