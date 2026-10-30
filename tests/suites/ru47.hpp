#pragma once

// C1-001 (LORE v3.5 §12 R1) / ru47:
// Wire the R1 opening landing scene (r1_landing) using round 8 assets:
// Floofy in flower bed (ow/floofy.png landing_idle), Grub drops from light beam (grub.png fall),
// impact (landing_impact, petals/shake, thud sound pomf.ogg), Floofy knocked_out, Grub stunned then
// lying on top, fade to black (~2 s black), fade in on the same picture, then chains into existing
// r1_wake (buzz while lying -> get up -> thought). No text in r1_landing.
// Key X skips it on replays (ruins.r1.landing_seen). Loading a save in R1 never replays it.
// BUG-40's rule: after fade-in, Grub is lying, never standing.

#include "core/Cutscene.hpp"
#include "core/RoomObjects.hpp"
#include "core/TiledMap.hpp"

#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace ru47_detail {

inline std::string slurp(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    assert(file && "file must exist");
    std::ostringstream text;
    text << file.rdbuf();
    return text.str();
}

class LandingTestHost final : public nundertale::CutsceneHost {
public:
    explicit LandingTestHost(nundertale::GameState& state) : m_state(state) {}

    void dialog(const std::string& speaker, const std::vector<std::string>& lines) override {
        dialogCount++;
        lastSpeaker = speaker;
        lastLines = lines;
    }
    bool dialogOpen() const override { return false; }

    void sound(const std::string& path, float volume) override {
        soundsPlayed.push_back(path);
        lastVolume = volume;
    }

    void setFlag(const std::string& key, int value) override {
        m_state.setFlag(key, value);
    }

    void shake(float intensity, float seconds) override {
        shakeCount++;
        lastShakeIntensity = intensity;
        lastShakeSeconds = seconds;
    }

    void flash(int, int, int, float) override {}
    void startBattle(const std::string&) override {}
    void setFur(int) override {}
    void healFull() override {}
    void facePlayer(const std::string&) override {}

    void playerAnim(const std::string& path, const std::string& key, float seconds) override {
        playerAnims.push_back({ path, key, seconds });
    }

    void npcAnim(const std::string& npc, const std::string& key, float seconds) override {
        npcAnims.push_back({ npc, key, seconds });
    }

    void setPlayerPos(float x, float y) {
        m_playerX = x;
        m_playerY = y;
        posHistory.push_back({ x, y });
    }

    bool movePlayer(float x, float y, float, float) override {
        setPlayerPos(x, y);
        return true;
    }

    void waterBurst(const std::string& target) override {
        petalBursts.push_back(target);
    }

    bool fadeOut(float, float) override {
        fadedOut = true;
        return true;
    }

    bool fadeIn(float, float) override {
        fadedIn = true;
        return true;
    }

    bool loadCutscene(const std::string& id, nundertale::Cutscene& out) override {
        chainedCutscenes.push_back(id);
        const std::string path = "data/cutscenes/" + id + ".json";
        const std::string text = slurp(path);
        return nundertale::parseCutsceneJson(text, out);
    }

    struct AnimEvent {
        std::string path;
        std::string key;
        float seconds;
    };

    struct NpcAnimEvent {
        std::string npc;
        std::string key;
        float seconds;
    };

    struct PosEvent {
        float x;
        float y;
    };

    int dialogCount = 0;
    std::string lastSpeaker;
    std::vector<std::string> lastLines;
    std::vector<std::string> soundsPlayed;
    float lastVolume = 0.0f;
    int shakeCount = 0;
    float lastShakeIntensity = 0.0f;
    float lastShakeSeconds = 0.0f;
    std::vector<AnimEvent> playerAnims;
    std::vector<NpcAnimEvent> npcAnims;
    std::vector<PosEvent> posHistory;
    std::vector<std::string> petalBursts;
    std::vector<std::string> chainedCutscenes;
    bool fadedOut = false;
    bool fadedIn = false;

private:
    nundertale::GameState& m_state;
    float m_playerX = 323.0f;
    float m_playerY = 30.0f;
};

} // namespace ru47_detail

inline void suite_ru47() {
    using namespace nundertale;
    using namespace ru47_detail;

    // --- 1. Verify Assets exist and have required frames / files --------------------
    const std::string grubSheet = slurp("assets/sprites/ow/grub.json");
    assert(grubSheet.find("\"fall\"") != std::string::npos);
    assert(grubSheet.find("\"stunned\"") != std::string::npos);
    assert(grubSheet.find("\"lying\"") != std::string::npos);
    assert(grubSheet.find("\"get_up\"") != std::string::npos);

    const std::string floofySheet = slurp("assets/sprites/ow/floofy.json");
    assert(floofySheet.find("\"landing_idle\"") != std::string::npos);
    assert(floofySheet.find("\"landing_impact\"") != std::string::npos);
    assert(floofySheet.find("\"knocked_out\"") != std::string::npos);

    // Audio thud asset exists
    const std::string pomf = slurp("assets/sfx/pomf.ogg");
    assert(!pomf.empty());

    // --- 2. Parse data/cutscenes/r1_landing.json ------------------------------------
    Cutscene landing;
    const std::string landingJson = slurp("data/cutscenes/r1_landing.json");
    assert(parseCutsceneJson(landingJson, landing));
    assert(!landing.steps.empty());
    assert(landing.skipFlag == "ruins.r1.landing_seen");

    // Order assertions
    int floofyIdle = -1;
    int playerFall = -1;
    int dropDown = -1;
    int thudSound = -1;
    int shake = -1;
    int petals = -1;
    int floofyImpact = -1;
    int playerStunned = -1;
    int floofyKnockedOut = -1;
    int playerLying = -1;
    int fadeOutIdx = -1;
    int blackWaitIdx = -1;
    int fadeInIdx = -1;
    int setSeenFlag = -1;
    int chainWake = -1;

    for (std::size_t i = 0; i < landing.steps.size(); ++i) {
        const CutsceneStep& step = landing.steps[i];
        const int at = static_cast<int>(i);

        // Rule: No text in r1_landing
        assert(step.type != CutsceneStepType::Dialog);

        if (step.type == CutsceneStepType::NpcAnim && step.npc == "floofy" && step.key == "landing_idle" && floofyIdle < 0) floofyIdle = at;
        if (step.type == CutsceneStepType::PlayerAnim && step.key == "fall" && playerFall < 0) playerFall = at;
        if (step.type == CutsceneStepType::MovePlayer &&
            std::fabs(step.y - 232.0f) < 1.0f && dropDown < 0) dropDown = at;
        if (step.type == CutsceneStepType::Sound && step.path == "assets/sfx/pomf.ogg" && thudSound < 0) thudSound = at;
        if (step.type == CutsceneStepType::Shake && shake < 0) shake = at;
        if (step.type == CutsceneStepType::WaterBurst && petals < 0) petals = at;
        if (step.type == CutsceneStepType::NpcAnim && step.npc == "floofy" && step.key == "landing_impact" && floofyImpact < 0) floofyImpact = at;
        if (step.type == CutsceneStepType::PlayerAnim && step.key == "stunned" && playerStunned < 0) playerStunned = at;
        if (step.type == CutsceneStepType::NpcAnim && step.npc == "floofy" && step.key == "knocked_out" && floofyKnockedOut < 0) floofyKnockedOut = at;
        if (step.type == CutsceneStepType::PlayerAnim && step.key == "lying" && playerLying < 0) playerLying = at;
        if (step.type == CutsceneStepType::FadeOut && fadeOutIdx < 0) fadeOutIdx = at;
        if (step.type == CutsceneStepType::Wait && step.seconds >= 1.9f && fadeOutIdx >= 0 && fadeInIdx < 0) blackWaitIdx = at;
        if (step.type == CutsceneStepType::FadeIn && fadeInIdx < 0) fadeInIdx = at;
        if (step.type == CutsceneStepType::SetFlag && step.key == "ruins.r1.landing_seen" && step.value == 1 && setSeenFlag < 0) setSeenFlag = at;
        if (step.type == CutsceneStepType::PlayCutscene && step.key == "r1_wake" && chainWake < 0) chainWake = at;
    }

    assert(floofyIdle >= 0);
    assert(playerFall >= 0);
    assert(dropDown > playerFall);
    assert(thudSound >= dropDown);
    assert(shake >= dropDown);
    assert(petals >= dropDown);
    assert(floofyImpact >= dropDown);
    assert(playerStunned >= dropDown);
    assert(floofyKnockedOut > floofyImpact);
    assert(playerLying > playerStunned);
    assert(fadeOutIdx > playerLying);
    assert(blackWaitIdx > fadeOutIdx);
    assert(fadeInIdx > blackWaitIdx);
    assert(setSeenFlag > fadeInIdx);
    assert(chainWake > setSeenFlag);

    // BUG-40's rule: after fade-in, Grub is lying, never standing (no PlayerAnim: standing or clear before r1_wake)
    for (std::size_t i = static_cast<std::size_t>(fadeInIdx); i < landing.steps.size(); ++i) {
        assert(landing.steps[i].type != CutsceneStepType::PlayerAnim || landing.steps[i].key == "lying");
    }

    // --- 3. Execute cutscene with LandingTestHost -----------------------------------
    {
        GameState state;
        LandingTestHost host(state);
        CutsceneRunner runner;
        runner.start(landing);

        int ticks = 0;
        while (runner.running() && ticks < 200) {
            runner.update(0.1f, host);
            ++ticks;
        }

        // Must chain into r1_wake
        assert(!host.chainedCutscenes.empty());
        assert(host.chainedCutscenes[0] == "r1_wake");
        assert(host.soundsPlayed.size() >= 1U && host.soundsPlayed[0] == "assets/sfx/pomf.ogg");
        assert(host.shakeCount >= 1);
        assert(!host.petalBursts.empty());
        assert(host.fadedOut);
        assert(host.fadedIn);
        assert(state.getFlag("ruins.r1.landing_seen", 0) == 1);
    }

    // --- 4. Verify Map ruins_fall.json Triggers -------------------------------------
    TiledMap map;
    assert(loadTiledMapFromFile("data/maps/ruins_fall.json", map));

    const TiledObject* triggerLanding = map.findObject("trigger_landing");
    assert(triggerLanding != nullptr);
    assert(tiledObjectType(*triggerLanding) == "trigger");
    assert(tiledStringProperty(triggerLanding->properties, "cutscene") == "r1_landing");
    assert(tiledStringProperty(triggerLanding->properties, "once") == "ruins.r1.woke");

    const TiledObject* northDoor = map.findObject("door_to_registration");
    assert(northDoor != nullptr);
    // The high new-game spawn intersects both objects. Runtime checks objects in
    // map order, so landing must start before the door can begin its transition.
    assert(triggerLanding->id < northDoor->id);

    const TiledObject* triggerWake = map.findObject("trigger_wake");
    assert(triggerWake != nullptr);
    assert(tiledObjectType(*triggerWake) == "trigger");
    assert(tiledStringProperty(triggerWake->properties, "cutscene") == "r1_wake");
    assert(tiledStringProperty(triggerWake->properties, "once") == "ruins.r1.woke");

    std::cout << "[PASS] ru47: R1 landing cutscene (C1-001) parsed, step order verified, executed cleanly." << std::endl;
}
