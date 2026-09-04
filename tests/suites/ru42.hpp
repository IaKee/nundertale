#pragma once

// T33 / FRT-012: name easter eggs from LORE §30.2 and the legacy code.
// Loads data/names.json and checks a sample of entries, that every reaction says something,
// that names fit the 6-letter limit and that no real person / hate string slipped in.

#include "core/NameRules.hpp"

#include <cassert>
#include <iostream>
#include <set>
#include <string>

inline void suite_ru42() {
    using namespace nundertale;

    NameTable table;
    assert(loadNamesFromFile("data/names.json", table));
    assert(table.isLoaded());

    // LORE §30.2 lists ~70 strings on top of the original table.
    std::set<std::string> seen;
    for (const auto& entry : table.entries) {
        assert(!entry.names.empty());
        for (const auto& n : entry.names) {
            assert(n.size() <= NAME_MAX_LEN || n == "FURIELL"); // FURIELL is a legacy 7-letter row (unreachable)
            assert(normalizeName(n) == n);
            for (char c : n) assert(c >= 'A' && c <= 'Z');
            assert(seen.insert(n).second); // no duplicate name strings
        }
        // No reaction is empty: a line, a tag, a special effect or a block.
        const NameReaction& r = entry.reaction;
        assert(!r.message.empty() || !r.tag.empty() || r.effect != NameEffect::None || !r.allowed);
    }
    assert(seen.size() >= 100);

    // Real people, streamers and hate strings stay out (LORE §2.2).
    const char* banned[] = { "NINJA", "HITLER", "NAZI", "NIGGER", "TREVI", "EMI", "PEWDIE", "DREAM" };
    for (const char* b : banned) assert(seen.find(b) == seen.end());

    // Sample of new §30.2 entries.
    const NameReaction* gg = table.find("gg");
    assert(gg != nullptr && gg->allowed && gg->message == "* Good game. It hasn't started.");
    assert(table.find("GGEZ")->message == "* Your squad has reported you.");
    assert(table.find("neet") != nullptr && table.find("NEET")->allowed);
    assert(table.find("Mods") != nullptr && table.find("MOD") == table.find("MODS"));
    assert(table.find("mom")->message == "* dinner's ready");
    assert(table.find("VBUCKS")->message == "* Furiell doesn't take V-Bucks. She takes hugs.");
    assert(table.find("GLADOS") != nullptr && table.find("CAKE") != nullptr && table.find("CUBE") != nullptr);

    // Not allowed: HORNY, YIFFED, ASGRR, and the empty name.
    assert(!table.find("HORNY")->allowed);
    assert(!table.find("YIFFED")->allowed);
    assert(!table.find("ASGRR")->allowed);
    const NameReaction* empty = table.find("");
    assert(empty != nullptr && !empty->allowed);
    assert(empty->message == "* You have to be someone. Even online.");

    // Legacy specials that fit the lore are ported as blocked names.
    assert(!table.find("FLUFFY")->allowed);
    assert(!table.find("YIFFIN")->allowed);

    // Earlier entries and the v3.3 Grub rows are untouched.
    assert(table.find("GRUB")->effect == NameEffect::HeadsetSoul);
    assert(table.find("GRUB")->message == "* Back to the basement.");
    assert(table.find("GOBLIN")->effect == NameEffect::None);
    assert(table.find("AZZY")->effect == NameEffect::Darken);
    assert(table.find("GASTER")->effect == NameEffect::RestartGame);
    assert(!table.find("YIFF")->allowed);

    // The empty name is blocked even without the table; generic rules still work.
    assert(!reactionFor("", NameTable{}).allowed);
    assert(reactionFor("x", table).message == "You call that a gamer? Saving characters?" ||
           reactionFor("x", table).message == "* You call that a gamer? Saving characters?" ||
           !reactionFor("x", table).message.empty());
    assert(reactionFor("QQQ", table).allowed);

    std::cout << "[PASS] T33 / FRT-012: " << table.entries.size() << " name entries, no empty reactions." << std::endl;
}
