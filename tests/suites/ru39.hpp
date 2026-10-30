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

namespace ru39_detail {

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

} // namespace ru39_detail

inline void suite_ru39() {
    using namespace nundertale;

    // Five-key Lights-Out: an edge toggles two keys, an inner key toggles three.
    assert(RGB_KEY_COUNT == 5);
    assert(rgbKeyboardPressMask(0) == 0b00011);
    assert(rgbKeyboardPressMask(2) == 0b01110);
    assert(rgbKeyboardPressMask(4) == 0b11000);
    assert(rgbKeyboardPressMask(-1) == 0);
    assert(pressRgbKeyboardKey(0b10101, 2) == 0b11011);

    // The shipped state is exactly two presses from pink (keys 1 and 3, in either order).
    const int afterFirst = pressRgbKeyboardKey(RGB_KEY_START_MASK, 1);
    const int afterSecond = pressRgbKeyboardKey(afterFirst, 3);
    assert(!rgbKeyboardSolved(RGB_KEY_START_MASK));
    assert(!rgbKeyboardSolved(afterFirst));
    assert(rgbKeyboardSolved(afterSecond));
    assert(rgbKeyboardMinimumPresses(RGB_KEY_START_MASK, RGB_KEY_TARGET_MASK) == 2);
    for (int key = 0; key < RGB_KEY_COUNT; ++key) {
        assert(!rgbKeyboardSolved(pressRgbKeyboardKey(RGB_KEY_START_MASK, key)));
    }
    assert(!rgbKeyboardTintIsPink(rgbKeyboardTint(RGB_KEY_START_MASK)));
    assert(rgbKeyboardTintIsPink(rgbKeyboardTint(RGB_KEY_TARGET_MASK)));

    // The generated room contains the mechanical keys, mechanical keyboard frame, floor cables, and shutter door.
    TiledMap room;
    assert(loadTiledMapFromFile("data/maps/ruins_switches.json", room));
    std::array<bool, RGB_KEY_COUNT> foundKeys{};
    bool foundFrame = false;
    bool foundShutter = false;
    int barrierPieces = 0;
    int floorCables = 0;
    for (const TiledLayer& layer : room.layers) {
        if (layer.type != TiledLayerType::Objects) continue;
        for (const TiledObject& object : layer.objects) {
            assert(object.name != "sw1" && object.name != "sw2" && object.name != "sw3");
            const std::string sprite = tiledStringProperty(object.properties, "sprite");
            assert(sprite.find("ruins_rgb_") == std::string::npos);
            assert(sprite.find("ruins_cables.png") == std::string::npos);

            const int key = tiledIntProperty(object.properties, "rgbKey", -1);
            if (key >= 0) {
                assert(key < RGB_KEY_COUNT);
                foundKeys[static_cast<std::size_t>(key)] = true;
                assert(sprite == "assets/props/ruins_mech_keys.png");
                assert(tiledStringProperty(object.properties, "cutscene") == "r4_keys");
                assert(tiledStringProperty(object.properties, "altAnimIf") ==
                       "ruins.r4.key" + std::to_string(key) + " >= 1");
            }
            if (object.name == "rgb_keyboard_frame") {
                foundFrame = true;
                assert(sprite == "assets/props/ruins_mech_keyboard.png");
            }
            if (object.name == "shutter_door") {
                foundShutter = true;
                assert(sprite == "assets/props/ruins_switch_door.png");
                assert(tiledStringProperty(object.properties, "anim") == "closed");
                assert(tiledStringProperty(object.properties, "altAnim") == "opening");
                assert(tiledStringProperty(object.properties, "altAnimIf") == "ruins.r4.solved >= 1");
            }
            if (object.name == "shutter_door_barrier") {
                ++barrierPieces;
                assert(tiledBoolProperty(object.properties, "solid", false));
                assert(tiledStringProperty(object.properties, "hideIf") == "ruins.r4.solved >= 1");
            }
            if (sprite == "assets/props/ruins_floor_cables.png") {
                ++floorCables;
            }
        }
    }
    for (bool found : foundKeys) assert(found);
    assert(foundFrame);
    assert(foundShutter);
    assert(barrierPieces == 1);
    assert(floorCables >= 4);

    const Cutscene hint = ru39_detail::loadCutscene("r4_hint");
    assert(hint.steps.size() == 1U);
    assert(hint.steps[0].speaker == "FURIELL");
    assert(hint.steps[0].lines.size() == 1U);
    assert(hint.steps[0].lines[0] == "Make it pink, hun! Pink is for hugs! uwu");

    const Cutscene thought = ru39_detail::loadCutscene("r4_keys");
    assert(thought.steps.size() == 2U);
    assert(thought.steps[0].lines.size() == 1U);
    assert(thought.steps[0].lines[0] == "* (You've done this a thousand times. In the RGB app.)");
    assert(thought.steps[1].type == CutsceneStepType::SetFlag);
    assert(thought.steps[1].key == "ruins.r4.keys_thought" && thought.steps[1].value == 1);

    std::cout << "[PASS] C1-004: five-key RGB Lights-Out toggles neighbors, starts two presses from pink, "
                 "opens the shutter door, and retires the old switches."
              << std::endl;
}
