#pragma once

#include "core/PlayerSheet.hpp"
#include <cassert>
#include <iostream>

inline void suite_ru55() {
    using namespace nundertale;

    // --- 1. Boundaries 19/20 and 79/80, plus 0/39/40/59/60/100 and clamping ---
    // FUR 0 (Gamer stage)
    const auto c0 = playerSheetForFur(0);
    assert(c0.sheet == "assets/sprites/ow/grub.png");
    assert(c0.sheet == "sprites/ow/grub.png");
    assert(c0.row_offset == 0);
    assert(c0.rowOffset == 0);
    assert(c0.stage == "gamer");

    // Negative clamps to Gamer stage
    const auto cNeg = playerSheetForFur(-5);
    assert(cNeg.sheet == "assets/sprites/ow/grub.png");
    assert(cNeg.row_offset == 0);
    assert(cNeg.stage == "gamer");

    // Boundary 19: Gamer stage
    const auto c19 = playerSheetForFur(19);
    assert(c19.sheet == "assets/sprites/ow/grub.png");
    assert(c19.sheet == "sprites/ow/grub.png");
    assert(c19.row_offset == 0);
    assert(c19.rowOffset == 0);
    assert(c19.stage == "gamer");

    // Boundary 20: Ears stage (row_offset 0)
    const auto c20 = playerSheetForFur(20);
    assert(c20.sheet == "assets/sprites/ow/grub_fur.png");
    assert(c20.sheet == "sprites/ow/grub_fur.png");
    assert(c20.row_offset == 0);
    assert(c20.rowOffset == 0);
    assert(c20.stage == "ears");

    // Boundary 39: Ears stage (row_offset 0)
    const auto c39 = playerSheetForFur(39);
    assert(c39.sheet == "assets/sprites/ow/grub_fur.png");
    assert(c39.row_offset == 0);
    assert(c39.stage == "ears");

    // Boundary 40: Paws stage (row_offset 6)
    const auto c40 = playerSheetForFur(40);
    assert(c40.sheet == "assets/sprites/ow/grub_fur.png");
    assert(c40.row_offset == 6);
    assert(c40.rowOffset == 6);
    assert(c40.stage == "paws");

    // Boundary 59: Paws stage (row_offset 6)
    const auto c59 = playerSheetForFur(59);
    assert(c59.sheet == "assets/sprites/ow/grub_fur.png");
    assert(c59.row_offset == 6);
    assert(c59.stage == "paws");

    // Boundary 60: Tail stage (row_offset 12)
    const auto c60 = playerSheetForFur(60);
    assert(c60.sheet == "assets/sprites/ow/grub_fur.png");
    assert(c60.row_offset == 12);
    assert(c60.rowOffset == 12);
    assert(c60.stage == "tail");

    // Boundary 79: Tail stage (row_offset 12)
    const auto c79 = playerSheetForFur(79);
    assert(c79.sheet == "assets/sprites/ow/grub_fur.png");
    assert(c79.sheet == "sprites/ow/grub_fur.png");
    assert(c79.row_offset == 12);
    assert(c79.rowOffset == 12);
    assert(c79.stage == "tail");

    // Boundary 80: Snout stage (row_offset 18)
    const auto c80 = playerSheetForFur(80);
    assert(c80.sheet == "assets/sprites/ow/grub_fur.png");
    assert(c80.sheet == "sprites/ow/grub_fur.png");
    assert(c80.row_offset == 18);
    assert(c80.rowOffset == 18);
    assert(c80.stage == "snout");

    // Upper boundary 100
    const auto c100 = playerSheetForFur(100);
    assert(c100.sheet == "assets/sprites/ow/grub_fur.png");
    assert(c100.row_offset == 18);
    assert(c100.stage == "snout");

    // Over-max clamps to 100 (snout)
    const auto c150 = playerSheetForFur(150);
    assert(c150.sheet == "assets/sprites/ow/grub_fur.png");
    assert(c150.row_offset == 18);

    // Pair helper
    const auto p19 = playerSheetPair(19);
    assert(p19.first == "assets/sprites/ow/grub.png");
    assert(p19.second == 0);
    const auto p80 = playerSheetPair(80);
    assert(p80.first == "assets/sprites/ow/grub_fur.png");
    assert(p80.second == 18);

    // --- 2. Animation mapping and fallbacks ---
    // Standard movements
    const auto rGamer = resolvePlayerRender(0, "stand_down");
    assert(rGamer.sheet == "assets/sprites/ow/grub.png");
    assert(rGamer.anim == "stand_down");
    assert(rGamer.row_offset == 0);

    const auto rEars = resolvePlayerRender(20, "walk_down");
    assert(rEars.sheet == "assets/sprites/ow/grub_fur.png");
    assert(rEars.anim == "ears_walk_down");
    assert(rEars.row_offset == 0);

    const auto rPaws = resolvePlayerRender(40, "stand_left");
    assert(rPaws.sheet == "assets/sprites/ow/grub_fur.png");
    assert(rPaws.anim == "paws_stand_left");
    assert(rPaws.row_offset == 6);

    const auto rTail = resolvePlayerRender(60, "walk_right");
    assert(rTail.sheet == "assets/sprites/ow/grub_fur.png");
    assert(rTail.anim == "tail_walk_right");
    assert(rTail.row_offset == 12);

    const auto rSnout = resolvePlayerRender(80, "stand_up");
    assert(rSnout.sheet == "assets/sprites/ow/grub_fur.png");
    assert(rSnout.anim == "snout_stand_up");
    assert(rSnout.row_offset == 18);

    // Lying & get_up in grub_fur
    const auto rLying = resolvePlayerRender(40, "lying");
    assert(rLying.sheet == "assets/sprites/ow/grub_fur.png");
    assert(rLying.anim == "paws_lying");
    assert(rLying.row_offset == 6);

    const auto rGetUp = resolvePlayerRender(60, "get_up");
    assert(rGetUp.sheet == "assets/sprites/ow/grub_fur.png");
    assert(rGetUp.anim == "tail_get_up");
    assert(rGetUp.row_offset == 12);

    // Fall & stunned: grub_fur has no such rows, fallback to grub.png
    const auto rFall = resolvePlayerRender(80, "fall");
    assert(rFall.sheet == "assets/sprites/ow/grub.png");
    assert(rFall.anim == "fall");
    assert(rFall.row_offset == 0);

    const auto rStunned = resolvePlayerRender(40, "stunned");
    assert(rStunned.sheet == "assets/sprites/ow/grub.png");
    assert(rStunned.anim == "stunned");
    assert(rStunned.row_offset == 0);

    std::cout << "[PASS] D-25 / ru55: FUR stage player sheet and row_offset mapping verified." << std::endl;
}
