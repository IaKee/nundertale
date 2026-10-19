#pragma once

#include "core/Bloodstain.hpp"

#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

inline void suite_ru45() {
    using namespace nundertale;

    // The pure timeline: lead -> walk -> embrace -> soul -> done, in order and adding up.
    assert(bloodstainPhaseAt(0.0f) == BloodstainPhase::Lead);
    assert(bloodstainPhaseAt(kBloodstainLeadSeconds + 0.1f) == BloodstainPhase::Walk);
    const float embraceStart = kBloodstainLeadSeconds + bloodstainWalkSeconds();
    assert(bloodstainPhaseAt(embraceStart + 0.1f) == BloodstainPhase::Embrace);
    const float soulStart = embraceStart + kBloodstainEmbraceSeconds;
    assert(bloodstainPhaseAt(soulStart + 0.1f) == BloodstainPhase::Soul);
    assert(bloodstainPhaseAt(bloodstainReplaySeconds() + 0.01f) == BloodstainPhase::Done);
    assert(std::fabs(bloodstainReplaySeconds() - (soulStart + bloodstainSoulSeconds())) < 1e-4f);

    assert(bloodstainGhostWalked(0.0f) == 0.0f);
    assert(std::fabs(bloodstainGhostWalked(embraceStart) - kBloodstainWalkDistance) < 1e-3f);
    assert(bloodstainGhostWalked(100.0f) == kBloodstainWalkDistance);
    assert(bloodstainSoulProgress(soulStart - 0.1f) == 0.0f);
    assert(bloodstainSoulProgress(soulStart + bloodstainSoulSeconds() * 0.5f) > 0.45f);
    assert(bloodstainSoulProgress(bloodstainReplaySeconds() + 5.0f) == 1.0f);
    assert(bloodstainGhostVisible(1.0f));
    assert(!bloodstainGhostVisible(-1.0f));
    assert(!bloodstainGhostVisible(bloodstainReplaySeconds() + 0.5f));
    assert(kBloodstainGhostAlpha > 0.0f && kBloodstainGhostAlpha < 0.8f);

    // The cutscene plays that timeline: ghost on, walk, soul, ghost off, then the narrator line last.
    std::ifstream file("data/cutscenes/r8_bloodstain.json");
    assert(file.good());
    std::ostringstream buffer;
    buffer << file.rdbuf();
    const std::string json = buffer.str();

    const auto at = [&json](const std::string& needle, std::size_t from) {
        return json.find(needle, from);
    };
    const std::size_t ghostOn = at("\"ruins.r8.ghost_on\", \"value\": 1", 0);
    const std::size_t walkAnim = at("\"walk_right\"", ghostOn);
    const std::size_t walkMove = at("\"speed\": 60", walkAnim);
    const std::size_t soulOn = at("\"ruins.r8.soul_on\", \"value\": 1", walkMove);
    const std::size_t soulMove = at("\"speed\": 30", soulOn);
    const std::size_t ghostOff = at("\"ruins.r8.ghost_on\", \"value\": 0", soulMove);
    const std::size_t line = at("* (They said yes.)", ghostOff);
    assert(ghostOn != std::string::npos && walkAnim != std::string::npos);
    assert(walkMove != std::string::npos && soulOn != std::string::npos);
    assert(soulMove != std::string::npos && ghostOff != std::string::npos);
    assert(line != std::string::npos);
    assert(json.find("* (They said yes.)", line + 1) == std::string::npos);
    assert(json.find("\"dialog\"", line) == std::string::npos);  // the dialog is the final step
    assert(kBloodstainWalkSpeed == 60.0f && kBloodstainSoulSpeed == 30.0f);

    // R8 places the stain and the (hidden) cast, wired to the cutscene.
    std::ifstream mapFile("data/maps/ruins_notices.json");
    assert(mapFile.good());
    std::ostringstream mapBuffer;
    mapBuffer << mapFile.rdbuf();
    const std::string map = mapBuffer.str();
    assert(map.find("\"bloodstain\"") != std::string::npos);
    assert(map.find("r8_bloodstain") != std::string::npos);
    assert(map.find("bloodstain_ghost") != std::string::npos);
    assert(map.find("bloodstain_hugger") != std::string::npos);
    assert(map.find("bloodstain_soul") != std::string::npos);

    std::cout << "[PASS] OVW-022: bloodstain replay timeline (ghost walks, hug, soul leaves, '(They said yes.)') "
                 "and the R8 placement." << std::endl;
}
