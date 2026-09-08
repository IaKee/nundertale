#pragma once

#include "core/ToxicTrack.hpp"
#include "core/Cutscene.hpp"
#include "core/RoomObjects.hpp"

#include <cassert>
#include <cmath>
#include <iostream>

inline void suite_ru31() {
    using namespace nundertale;

    constexpr float epsilon = 0.0001f;

    GameState state;
    assert(toxicEffectBlend(state) == 0.0f);
    assert(toxicFadeValue(state) == 0.0f);

    // kill, kill, spare returns permanently to casual over four room entries.
    assert(toxicTrackOnKill(state, "froggsona"));
    assert(toxicTrackOnKill(state, "shysona"));
    assert(toxicTrack(state) == 1);
    assert(toxicFadeValue(state) == 1.0f);
    assert(toxicTrackOnSpare(state, "modsmal"));
    assert(toxicTrack(state) == 2);
    assert(toxicFadeRoomsRemaining(state) == TOXIC_FADE_ROOM_COUNT);
    assert(toxicEffectBlend(state) == 1.0f);
    assert(toxicFadeValue(state) == 1.0f);

    for (int room = 1; room <= TOXIC_FADE_ROOM_COUNT; ++room) {
        assert(toxicTrackOnRoomEntry(state));
        const float expected = static_cast<float>(TOXIC_FADE_ROOM_COUNT - room) /
                               static_cast<float>(TOXIC_FADE_ROOM_COUNT);
        assert(std::fabs(toxicEffectBlend(state) - expected) < epsilon);
        assert(std::fabs(toxicFadeValue(state) - expected) < epsilon);
    }
    assert(!toxicTrackOnRoomEntry(state));
    assert(toxicFadeRoomsRemaining(state) == 0);
    assert(toxicFadeValue(state) == 0.0f);
    assert(overworldMusicPitch(state, "ruins_hall") == 1.0f);

    // An ended track cannot restart or extend its fade.
    assert(!toxicTrackOnKill(state, "lurkeye"));
    assert(!toxicTrackOnSpare(state, "froggsona"));
    assert(toxicTrack(state) == 2);
    assert(toxicFadeRoomsRemaining(state) == 0);

    // Excluded encounters neither start nor end the track and never start a fade.
    GameState excluded;
    assert(!toxicTrackOnKill(excluded, "fursuit_dummy"));
    assert(!toxicTrackOnSpare(excluded, "gladowos"));
    assert(toxicTrack(excluded) == 0);
    assert(toxicFadeRoomsRemaining(excluded) == 0);
    assert(toxicTrackOnKill(excluded, "froggsona"));
    assert(!toxicTrackOnSpare(excluded, "fursuit_dummy"));
    assert(!toxicTrackOnSpare(excluded, "gladowos"));
    assert(toxicTrack(excluded) == 1);
    assert(toxicEffectBlend(excluded) == 1.0f);

    // Corrupt save values are clamped and room entry can never make the counter negative.
    GameState corrupt;
    corrupt.setFlag(std::string(TOXIC_TRACK_FLAG), 2);
    corrupt.setFlag(std::string(TOXIC_KILLS_FLAG), 9);
    corrupt.setFlag(std::string(TOXIC_FADE_ROOMS_FLAG), -7);
    assert(toxicFadeRoomsRemaining(corrupt) == 0);
    assert(toxicFadeValue(corrupt) == 0.0f);
    assert(!toxicTrackOnRoomEntry(corrupt));
    assert(corrupt.getFlag(std::string(TOXIC_FADE_ROOMS_FLAG)) == 0);

    // A higher last stage fades fractionally while the shared effect blend stays normalized.
    GameState highStage;
    for (int kill = 0; kill < 9; ++kill) {
        assert(toxicTrackOnKill(highStage, "froggsona"));
    }
    assert(toxicCorruptionStage(highStage) == 2);
    assert(toxicTrackOnSpare(highStage, "shysona"));
    assert(toxicFadeValue(highStage) == 2.0f);
    assert(toxicTrackOnRoomEntry(highStage));
    assert(std::fabs(toxicFadeValue(highStage) - 1.5f) < epsilon);
    assert(std::fabs(overworldMusicPitch(highStage, "ruins_hall") - 0.8125f) < epsilon);

    const ToxicFadeState fade = toxicFadeState(highStage);
    const ToxicHudState hud = toxicHudState(highStage);
    assert(fade.fading);
    assert(fade.roomsRemaining == 3);
    assert(std::fabs(fade.value - 1.5f) < epsilon);
    assert(std::fabs(fade.blend - 0.75f) < epsilon);
    assert(std::fabs(hud.corruption - fade.value) < epsilon);
    assert(!hud.active);

    // T62: a scripted battle end is consumed exactly once when the overworld resumes.
    std::string pending = "r2_floofy_end";
    assert(takePendingCutscene(pending) == "r2_floofy_end");
    assert(pending.empty());
    assert(takePendingCutscene(pending).empty());

    // The overworld water effect is a data-driven cutscene step. Both `target` (preferred) and
    // `npc` use the existing target field so content can aim it at Grub or a named room object.
    Cutscene waterScene;
    assert(parseCutsceneJson(
        R"({"steps":[{"type":"waterBurst","target":"player"}]})", waterScene));
    assert(waterScene.steps.size() == 1U);
    assert(waterScene.steps[0].type == CutsceneStepType::WaterBurst);
    assert(waterScene.steps[0].npc == "player");

    class WaterHost final : public CutsceneHost {
    public:
        void dialog(const std::string&, const std::vector<std::string>&) override {}
        bool dialogOpen() const override { return false; }
        void sound(const std::string&, float) override {}
        void setFlag(const std::string&, int) override {}
        void shake(float, float) override {}
        void flash(int, int, int, float) override {}
        void startBattle(const std::string&) override {}
        void setFur(int) override {}
        void healFull() override {}
        void facePlayer(const std::string&) override {}
        void playerAnim(const std::string&, const std::string&, float) override {}
        void waterBurst(const std::string& target) override { lastTarget = target; }
        std::string lastTarget;
    } waterHost;
    CutsceneRunner waterRunner;
    waterRunner.start(std::move(waterScene));
    waterRunner.update(0.0f, waterHost);
    assert(!waterRunner.running());
    assert(waterHost.lastTarget == "player");

    // R1's Floofy/pile-dent pair must be re-evaluated from flags, not cached per visit. A copy
    // models save/load reconstruction and must produce the same post-return visibility.
    GameState landing;
    const auto landingFlags = [&landing](const std::string& key) {
        return landing.getFlag(key, 0);
    };
    const std::string leftLanding = "ruins.floofy.left_landing >= 1";
    assert(roomObjectConditionVisible("", leftLanding, landingFlags));       // Floofy shown
    assert(!roomObjectConditionVisible(leftLanding, "", landingFlags));     // dent hidden
    landing.setFlag("ruins.floofy.left_landing", 1);
    assert(!roomObjectConditionVisible("", leftLanding, landingFlags));      // Floofy hidden
    assert(roomObjectConditionVisible(leftLanding, "", landingFlags));       // dent shown
    GameState loadedLanding = landing;
    const auto loadedFlags = [&loadedLanding](const std::string& key) {
        return loadedLanding.getFlag(key, 0);
    };
    assert(!roomObjectConditionVisible("", leftLanding, loadedFlags));
    assert(roomObjectConditionVisible(leftLanding, "", loadedFlags));

    std::cout << "[PASS] PRG-032/T62: toxic fade, battle end hook, water burst step, and R1 visibility verified."
              << std::endl;
}
