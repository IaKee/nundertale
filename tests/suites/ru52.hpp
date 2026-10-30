#pragma once

#include "core/Cutscene.hpp"
#include "core/RgbKeyboard.hpp"
#include "core/RoomObjects.hpp"
#include "core/TiledMap.hpp"

#include <array>
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace ru52_detail {

inline std::string readText(const std::string& path) {
    std::ifstream input(path);
    assert(input.good());
    std::ostringstream text;
    text << input.rdbuf();
    return text.str();
}

inline nundertale::Cutscene loadCutscene(const std::string& id) {
    nundertale::Cutscene result;
    assert(nundertale::parseCutsceneJson(readText("data/cutscenes/" + id + ".json"), result));
    return result;
}

} // namespace ru52_detail

inline void suite_ru52() {
    using namespace nundertale;

    // 1. Puzzle logic: start state -> pink reachable in <= 3 presses (actually 2 presses).
    const int minPresses = rgbKeyboardMinimumPresses(RGB_KEY_START_MASK, RGB_KEY_TARGET_MASK);
    assert(minPresses > 0 && minPresses <= 3);
    assert(minPresses == 2);

    // Verify step-by-step resolution from start to pink
    const int step1 = pressRgbKeyboardKey(RGB_KEY_START_MASK, 1);
    const int step2 = pressRgbKeyboardKey(step1, 3);
    assert(!rgbKeyboardSolved(RGB_KEY_START_MASK));
    assert(!rgbKeyboardSolved(step1));
    assert(rgbKeyboardSolved(step2));

    // Target tint is pink, start tint is not pink
    const RgbKeyboardColor startTint = rgbKeyboardTint(RGB_KEY_START_MASK);
    const RgbKeyboardColor solvedTint = rgbKeyboardTint(RGB_KEY_TARGET_MASK);
    assert(!rgbKeyboardTintIsPink(startTint));
    assert(rgbKeyboardTintIsPink(solvedTint));

    // 2. Map verification for ruins_switches.json
    TiledMap room;
    assert(loadTiledMapFromFile("data/maps/ruins_switches.json", room));

    bool foundKeyboardFrame = false;
    bool foundShutterDoor = false;
    bool foundShutterBarrier = false;
    std::array<bool, RGB_KEY_COUNT> foundKeys{};
    int floorCableCount = 0;
    int litCableCount = 0;

    for (const TiledLayer& layer : room.layers) {
        if (layer.type != TiledLayerType::Objects) continue;
        for (const TiledObject& object : layer.objects) {
            const std::string sprite = tiledStringProperty(object.properties, "sprite");

            // Verify old art is no longer referenced anywhere
            assert(sprite.find("ruins_rgb_") == std::string::npos);
            assert(sprite.find("ruins_cables.png") == std::string::npos);

            // Verify mechanical keyboard frame
            if (object.name == "rgb_keyboard_frame") {
                foundKeyboardFrame = true;
                assert(sprite == "assets/props/ruins_mech_keyboard.png");
                assert(!tiledBoolProperty(object.properties, "solid", false));
            }

            // Verify mechanical keys
            const int key = tiledIntProperty(object.properties, "rgbKey", -1);
            if (key >= 0 && key < RGB_KEY_COUNT) {
                foundKeys[static_cast<std::size_t>(key)] = true;
                assert(sprite == "assets/props/ruins_mech_keys.png");
                assert(!tiledBoolProperty(object.properties, "solid", false));
                assert(tiledStringProperty(object.properties, "cutscene") == "r4_keys");
                assert(tiledStringProperty(object.properties, "altAnimIf") ==
                       "ruins.r4.key" + std::to_string(key) + " >= 1");
                const std::string anim = tiledStringProperty(object.properties, "anim");
                const std::string altAnim = tiledStringProperty(object.properties, "altAnim");
                assert(anim.ends_with("_off"));
                assert(altAnim.ends_with("_on"));
            }

            // Verify shutter door & barrier (door open state follows ruins.r4.solved)
            if (object.name == "shutter_door") {
                foundShutterDoor = true;
                assert(sprite == "assets/props/ruins_switch_door.png");
                assert(tiledStringProperty(object.properties, "anim") == "closed");
                assert(tiledStringProperty(object.properties, "altAnim") == "opening");
                assert(tiledStringProperty(object.properties, "altAnimIf") == "ruins.r4.solved >= 1");
            }
            if (object.name == "shutter_door_barrier") {
                foundShutterBarrier = true;
                assert(tiledBoolProperty(object.properties, "solid", false));
                assert(tiledStringProperty(object.properties, "hideIf") == "ruins.r4.solved >= 1");
            }

            // Verify floor cables
            if (sprite == "assets/props/ruins_floor_cables.png") {
                ++floorCableCount;
                assert(!tiledBoolProperty(object.properties, "solid", false));
                if (tiledStringProperty(object.properties, "altAnim") == "lit_h") {
                    ++litCableCount;
                    assert(tiledStringProperty(object.properties, "anim") == "straight_h");
                    assert(tiledStringProperty(object.properties, "altAnimIf") == "ruins.r4.solved >= 1");
                }
            }
        }
    }

    assert(foundKeyboardFrame);
    assert(foundShutterDoor);
    assert(foundShutterBarrier);
    for (bool found : foundKeys) assert(found);
    assert(floorCableCount >= 4);
    assert(litCableCount >= 2);

    // 3. Dialogue & thought checks
    const Cutscene hint = ru52_detail::loadCutscene("r4_hint");
    assert(hint.steps.size() >= 1U);
    assert(hint.steps[0].speaker == "FURIELL");
    assert(!hint.steps[0].lines.empty());
    assert(hint.steps[0].lines[0] == "Make it pink, hun! Pink is for hugs! uwu");

    const Cutscene thought = ru52_detail::loadCutscene("r4_keys");
    assert(thought.steps.size() >= 1U);
    assert(!thought.steps[0].lines.empty());
    assert(thought.steps[0].lines[0] == "* (You've done this a thousand times. In the RGB app.)");

    std::cout << "[PASS] C1-004 v3.5 (ru52): R4 rebuilt with mechanical keys, floor cables, and shutter door."
              << std::endl;
}
