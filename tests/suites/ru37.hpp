#pragma once

#include "core/WaterBurst.hpp"
#include "core/SpriteSheet.hpp"

#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

inline void suite_ru37() {
    using namespace nundertale;

    // 1. Initial state after spawn at a point
    {
        WaterBurst burst(120.0f, 150.0f, WaterBurstVariant::Battle);
        assert(std::abs(burst.getOriginX() - 120.0f) < 0.001f);
        assert(std::abs(burst.getOriginY() - 150.0f) < 0.001f);
        assert(std::abs(burst.getX() - 120.0f) < 0.001f);
        assert(std::abs(burst.getY() - 150.0f) < 0.001f);
        assert(burst.getElapsed() == 0.0f);
        assert(!burst.isDone());
        assert(!burst.isAnimDone());
        assert(burst.currentFrame() == 0);
        assert(burst.activeAnimFrame() == 0);
        assert(burst.getVariant() == WaterBurstVariant::Battle);
        assert(!burst.isOverworld());
        assert(burst.animName() == "battle");
        assert(burst.spriteSheetPath() == "assets/ui/water_burst.png");
        assert(burst.spriteSheetJson() == "assets/ui/water_burst.json");

        const auto& droplets = burst.getDroplets();
        assert(droplets.size() == static_cast<std::size_t>(kWaterBurstDefaultDropletCount));
        for (const auto& d : droplets) {
            assert(d.alive());
            assert(d.life > 0.0f);
            assert(std::abs(d.alpha() - 1.0f) < 0.001f);
            assert(d.color.r == 255 && d.color.g == 255 && d.color.b == 255);
        }
    }

    // 2. Timeline progression: 3 frames and done flag
    {
        WaterBurst burst(200.0f, 200.0f, WaterBurstVariant::Battle, 0x123456u);
        const float frameDuration = burst.frameDuration;

        // Still in frame 0
        burst.update(frameDuration * 0.5f);
        assert(!burst.isDone());
        assert(!burst.isAnimDone());
        assert(burst.currentFrame() == 0);
        assert(burst.activeAnimFrame() == 0);

        // Advance to frame 1
        burst.update(frameDuration * 0.6f);
        assert(!burst.isDone());
        assert(!burst.isAnimDone());
        assert(burst.currentFrame() == 1);
        assert(burst.activeAnimFrame() == 1);

        // Advance to frame 2
        burst.update(frameDuration * 1.0f);
        assert(!burst.isDone());
        assert(!burst.isAnimDone());
        assert(burst.currentFrame() == 2);
        assert(burst.activeAnimFrame() == 2);

        // Advance past 3-frame burst duration
        burst.update(frameDuration * 1.0f);
        assert(burst.isAnimDone());
        assert(burst.activeAnimFrame() == -1);
        assert(burst.currentFrame() == 2); // clamped
        // Falling droplets are still alive at this point:
        assert(!burst.isDone());

        // Step until all droplets have expired
        burst.update(0.50f);
        assert(burst.isAnimDone());
        assert(burst.isDone());

        for (const auto& d : burst.getDroplets()) {
            assert(!d.alive());
            assert(d.alpha() == 0.0f);
        }

        // Updating when done has no further effect
        const float finalElapsed = burst.getElapsed();
        burst.update(0.1f);
        assert(burst.getElapsed() == finalElapsed);
        assert(burst.isDone());
    }

    // 3. Droplet physics: initial spray and gravity acceleration
    {
        WaterBurst burst(100.0f, 100.0f, WaterBurstVariant::Battle, 0xABCDEFu);
        assert(burst.getDroplets().size() == 5);
        const float initialY = burst.getDroplets()[0].y;
        const float initialVy = burst.getDroplets()[0].vy;

        // In the first tick, upward velocity moves droplet up
        burst.update(0.05f);
        const float vyAfterTick = burst.getDroplets()[0].vy;
        // Gravity increases vy downwards
        assert(vyAfterTick > initialVy);

        // After substantial time, gravity has pulled the droplet below its initial position
        burst.update(0.40f);
        const float yAfterTime = burst.getDroplets()[0].y;
        assert(yAfterTime > initialY);
    }

    // 4. Overworld variant (light-blue tint)
    {
        WaterBurst owBurst(320.0f, 240.0f, WaterBurstVariant::Overworld);
        assert(owBurst.isOverworld());
        assert(owBurst.getVariant() == WaterBurstVariant::Overworld);
        assert(owBurst.animName() == "overworld");

        const auto& droplets = owBurst.getDroplets();
        assert(!droplets.empty());
        for (const auto& d : droplets) {
            // Overworld tint has blue component >= 210 and green >= 140
            assert(d.color.b >= 210);
            assert(d.color.g >= 140);
        }

        // Test boolean constructor overload
        WaterBurst owBurstBool(50.0f, 60.0f, true);
        assert(owBurstBool.isOverworld());

        WaterBurst battleBurstBool(50.0f, 60.0f, false);
        assert(!battleBurstBool.isOverworld());
    }

    // 5. Determinism and reproducibility with RNG seed
    {
        WaterBurst b1(10.0f, 20.0f, WaterBurstVariant::Battle, 0x424242u);
        WaterBurst b2(10.0f, 20.0f, WaterBurstVariant::Battle, 0x424242u);
        assert(b1.getDroplets().size() == b2.getDroplets().size());
        for (std::size_t i = 0; i < b1.getDroplets().size(); ++i) {
            assert(std::abs(b1.getDroplets()[i].x - b2.getDroplets()[i].x) < 0.0001f);
            assert(std::abs(b1.getDroplets()[i].y - b2.getDroplets()[i].y) < 0.0001f);
            assert(std::abs(b1.getDroplets()[i].vx - b2.getDroplets()[i].vx) < 0.0001f);
            assert(std::abs(b1.getDroplets()[i].vy - b2.getDroplets()[i].vy) < 0.0001f);
        }
    }

    // 6. Reset and re-spawn
    {
        WaterBurst burst(10.0f, 10.0f);
        burst.update(1.0f);
        assert(burst.isDone());

        burst.reset();
        assert(!burst.isDone());
        assert(burst.getElapsed() == 0.0f);
        assert(burst.getDroplets().empty());

        burst.spawn(80.0f, 90.0f, WaterBurstVariant::Overworld, 0x777u, 4);
        assert(!burst.isDone());
        assert(burst.getOriginX() == 80.0f);
        assert(burst.getOriginY() == 90.0f);
        assert(burst.isOverworld());
        assert(burst.getDroplets().size() == 4);
    }

    // 7. Verify JSON metadata with engine's SpriteSheetJsonParser
    {
        std::ifstream file("assets/ui/water_burst.json");
        assert(file.good());
        std::stringstream ss;
        ss << file.rdbuf();
        const std::string jsonContent = ss.str();
        assert(!jsonContent.empty());

        SpriteSheetDef def;
        SpriteSheetJsonParser parser(jsonContent);
        assert(parser.parse(def));
        assert(def.image == "water_burst.png");
        assert(def.cellW == 24);
        assert(def.cellH == 24);
        assert(def.originX == 12);
        assert(def.originY == 12);
        assert(def.anims.count("battle") == 1);
        assert(def.anims.at("battle").frames == 3);
        assert(def.anims.at("battle").row == 0);
        assert(def.anims.count("overworld") == 1);
        assert(def.anims.at("overworld").frames == 3);
        assert(def.anims.at("overworld").row == 1);
    }

    std::cout << "[PASS] ART-183 / T17 (ru37): WaterBurst timeline, 3-frame animation, droplet trajectory, done flag, and variants verified."
              << std::endl;
}
