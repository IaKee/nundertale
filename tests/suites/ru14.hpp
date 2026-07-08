#pragma once

#include "core/Cell.hpp"

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

inline void suite_ru14() {
    using namespace nundertale;

    const std::vector<std::string> files = {
        "r3_furiell", "r4_hint", "r6_guide", "r7_cell", "r7_call",
        "r10_call", "r11_furiell", "r11_bed", "r12_boss"
    };
    const std::vector<std::string> furiellFiles = {
        "r3_furiell", "r4_hint", "r6_guide", "r7_cell", "r7_call",
        "r10_call", "r11_furiell", "r12_boss"
    };

    auto load = [](const std::string& id) {
        std::ifstream in("data/cutscenes/" + id + ".json", std::ios::binary);
        assert(in.good());
        std::stringstream ss;
        ss << in.rdbuf();
        return ss.str();
    };
    auto contains = [](const std::string& hay, const std::string& needle) {
        return hay.find(needle) != std::string::npos;
    };

    for (const auto& id : files) {
        const std::string text = load(id);
        assert(!text.empty());
        assert(text.front() == '{' || text.front() == '[' || text.front() == ' ' || text.front() == '\n');
        assert(contains(text, "\"steps\""));
        assert(!contains(text, "my child"));
        assert(!contains(text, "My child"));
        assert(!contains(text, "Pepe"));
    }

    for (const auto& id : furiellFiles) {
        const std::string text = load(id);
        assert(contains(text, "\"FURIELL\""));
        const bool voiced = contains(text, "pup") || contains(text, "hun") || contains(text, "uwu") ||
                            contains(text, "Hewwo") || contains(text, ">w<");
        assert(voiced);
    }

    // LORE exact strings kept.
    assert(contains(load("r4_hint"), "Make it pink, hun! Pink is for hugs! uwu"));
    assert(contains(load("r10_call"), "Do you prefer glitter or MORE glitter? ...No reason."));
    assert(contains(load("r7_call"), "...You don't have allergies to faux-fur, do you?"));
    assert(contains(load("r11_furiell"), "Do you prefer glitter or MORE glitter?"));

    // CELL replies.
    const auto& options = furiellOptions();
    assert(!options.empty());
    for (const auto& [label, reply] : options) {
        assert(!label.empty());
        assert(!reply.empty());
        assert(!contains(reply, "my child"));
        assert(!contains(reply, "Pepe"));
    }
    assert(options[0].second == "Hewwo? This is Furiell. ...Did you drink water today? Actual water? uwu");
    assert(options[1].second == "Make it pink, hun! Pink is always right. uwu");

    std::cout << "[PASS] NAR-035: Furiell uwu/pup/hun voice in Ruins cutscenes and CELL, no 'my child'/'Pepe'." << std::endl;
}
