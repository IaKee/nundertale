#pragma once

#include "combat/DustEffect.hpp"

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

inline void suite_ru16() {
    using namespace nundertale;

    // determinism
    const auto a = generateDustEffect(1234u, 30, true, 10.0f, 20.0f);
    const auto b = generateDustEffect(1234u, 30, true, 10.0f, 20.0f);
    assert(a.size() == b.size());
    for (std::size_t i = 0; i < a.size(); ++i) {
        assert(a[i].vx == b[i].vx && a[i].vy == b[i].vy && a[i].size == b[i].size);
        assert(a[i].kind == b[i].kind && a[i].color.r == b[i].color.r && a[i].color.g == b[i].color.g);
    }
    const auto c = generateDustEffect(99u, 30, true, 10.0f, 20.0f);
    bool differs = false;
    for (std::size_t i = 0; i < c.size(); ++i) differs = differs || c[i].vx != a[i].vx;
    assert(differs);

    // mix: stuffing + glitter, no red spark for non-exGamers
    const auto plain = generateDustEffect(7u, 30, false);
    assert(plain.size() == 30);
    int stuffing = 0, glitter = 0, spark = 0;
    for (const DustSeed& p : plain) {
        if (p.kind == DustKind::Stuffing) ++stuffing;
        else if (p.kind == DustKind::Glitter) {
            ++glitter;
            // white or pink flecks only
            assert(p.color.r == 255 && (p.color.g == 255 || p.color.g == 170));
        } else ++spark;
    }
    assert(spark == 0 && glitter == 10 && stuffing == 20);

    // exGamer: exactly one red spark flying right (toward the castle), upward
    const auto gamer = generateDustEffect(7u, 30, true, 50.0f, 60.0f);
    assert(gamer.size() == 31);
    int sparks = 0;
    for (const DustSeed& p : gamer) {
        if (p.kind != DustKind::RedSpark) continue;
        ++sparks;
        assert(p.color.r == 255 && p.color.g < 80 && p.color.b < 80);
        assert(p.vx > 100.0f && p.vy < 0.0f);
        assert(p.x == 50.0f && p.y == 60.0f);
    }
    assert(sparks == 1);

    // edges
    assert(generateDustEffect(1u, 0, false).empty());
    assert(generateDustEffect(1u, -5, false).empty());
    const auto sparkOnly = generateDustEffect(1u, 0, true);
    assert(sparkOnly.size() == 1 && sparkOnly[0].kind == DustKind::RedSpark);
    const auto one = generateDustEffect(1u, 1, false);
    assert(one.size() == 1 && one[0].kind == DustKind::Stuffing);
    const auto two = generateDustEffect(1u, 2, false);
    assert(two[0].kind == DustKind::Glitter && two[1].kind == DustKind::Stuffing);

    assert(isExGamerEnemyId("barky") && isExGamerEnemyId("napstablurk"));
    assert(!isExGamerEnemyId("furiell") && !isExGamerEnemyId(""));

    // data: furiell encounter music id is mapped
    auto slurp = [](const char* path) {
        std::ifstream f(path);
        std::stringstream ss;
        ss << f.rdbuf();
        return ss.str();
    };
    const std::string enc = slurp("data/encounters/furiell.json");
    assert(enc.find("\"music\": \"heartache_fluffier\"") != std::string::npos);
    assert(slurp("data/music.json").find("\"battle.heartache_fluffier\"") != std::string::npos);

    std::cout << "[PASS] C1-012/C1-022: plush dust generator is deterministic, mixes stuffing/glitter, "
                 "red spark only for exGamer; furiell music id mapped." << std::endl;
}
