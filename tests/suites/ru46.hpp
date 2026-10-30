#pragma once

#include "core/Cutscene.hpp"
#include "core/Portraits.hpp"
#include "tools/ContentLint.hpp"

#include <cassert>
#include <cmath>
#include <iostream>
#include <string>

inline void suite_ru46() {
    using namespace nundertale;

    Cutscene cutscene;
    assert(parseCutsceneJson(
        R"({"steps":[{"type":"dialog","speaker":"FURIELL","portrait":"furiell_neutral","lines":["Hello, pup!"]},{"type":"choice","portrait":"furiell_happy","lines":["Choose."],"options":["A","B"]}]})",
        cutscene));
    assert(cutscene.steps.size() == 2U);
    assert(cutscene.steps[0].portrait == "furiell_neutral");
    assert(cutscene.steps[1].portrait == "furiell_happy");

    const auto densityTwo = resolvePortrait("furiell_neutral");
    assert(densityTwo.has_value());
    assert(densityTwo->imagePath.find("assets/portraits/furiell_neutral.png") != std::string::npos);
    assert(densityTwo->sheet.density == 2);
    assert(std::fabs(portraitDrawScale(densityTwo->sheet.density) - 1.0f) < 0.0001f);

    const auto densityOne = resolvePortrait("floofy_cute");
    assert(densityOne.has_value());
    assert(densityOne->sheet.density == 1);
    assert(std::fabs(portraitDrawScale(densityOne->sheet.density) - 2.0f) < 0.0001f);

    assert(!resolvePortrait("does_not_exist").has_value());
    assert(!resolvePortrait("../furiell_neutral").has_value());

    const auto unknown = lintUnknownPortraits(
        R"({"steps":[{"type":"dialog","portrait":"does_not_exist","lines":["Hi."]}]})");
    assert(unknown.size() == 1U);
    assert(unknown.front().rule == "unknown-portrait");

    std::cout << "[PASS] BUG-63 / TXT-005: cutscene portraits resolve with density-aware scale."
              << std::endl;
}

