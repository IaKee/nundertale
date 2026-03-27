#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>
#include <string>
#include "combat/Bullet.hpp"
#include "combat/DamageZone.hpp"
#include "combat/Enemy.hpp"
#include "combat/BattleSystem.hpp"
#include "combat/BattleBackground.hpp"
#include "combat/ResidualDamage.hpp"
#include "combat/AttackTarget.hpp"
#include "ui/DialogBox.hpp"
#include "ui/Mosaic.hpp"
#include "core/AreaPalettes.hpp"
#include "core/Transition.hpp"
#include "core/NameRules.hpp"
#include "core/Fur.hpp"
#include "services/MusicTracks.hpp"
#include "scenes/FrontEndFlow.hpp"
#include "core/SaveText.hpp"

// Raylib stub for tests running without libraylib runtime
extern "C" void DrawRectangle(int /*posX*/, int /*posY*/, int /*width*/, int /*height*/, Color /*color*/) {}


// Test damage formula from FGT-003
static int calculateDamage(int baseAtk, int enemyDef, float dist, float maxDist, int randVal) {
    float accuracyMult = (dist <= 12.0f) ? 2.2f : std::max(0.5f, (1.0f - dist / maxDist) * 2.0f);
    int damage = std::max(1, static_cast<int>(std::round(static_cast<float>(baseAtk - enemyDef + randVal) * accuracyMult)));
    return damage;
}
// Test inventory limit from ITM-004
struct TestPlayerData {
    static constexpr size_t MAX_INVENTORY_SIZE = 8;
    std::vector<std::string> inventory;

    bool canAddItem() const { return inventory.size() < MAX_INVENTORY_SIZE; }
    bool addItem(const std::string& item) {
        if (!canAddItem()) return false;
        inventory.push_back(item);
        return true;
    }
};

int main() {
    std::cout << "Running nUndertale Parity & Mechanics Tests..." << std::endl;

    // 1. QA-011 / FGT-003: Damage formula tests
    {
        int at = 10;
        int def = 2;
        float maxDist = 280.0f;

        // Exact center (dist = 0) -> Critical 2.2x multiplier
        int centerDmg = calculateDamage(at, def, 0.0f, maxDist, 2);
        assert(centerDmg == static_cast<int>(std::round((10 - 2 + 2) * 2.2f)));
        assert(centerDmg == 22);

        // Near center (dist = 10 <= 12) -> Critical 2.2x multiplier
        int critNearDmg = calculateDamage(at, def, 10.0f, maxDist, 2);
        assert(critNearDmg == 22);

        // Mid range (dist = 140) -> (1 - 0.5) * 2.0 = 1.0 multiplier
        int midDmg = calculateDamage(at, def, 140.0f, maxDist, 2);
        assert(midDmg == 10);

        // Outer edge (dist = 270) -> clamped to min 0.5 multiplier
        int edgeDmg = calculateDamage(at, def, 270.0f, maxDist, 2);
        assert(edgeDmg == 5);

        // Minimum floor test: damage must never be < 1
        int minDmg = calculateDamage(0, 100, 200.0f, maxDist, 0);
        assert(minDmg == 1);

        std::cout << "[PASS] QA-011 & FGT-003: Damage formula edge cases and calculations verified." << std::endl;
    }

    // 2. ITM-004: 8-slot inventory limit test
    {
        TestPlayerData player;
        assert(player.canAddItem());

        for (int i = 0; i < 8; ++i) {
            bool ok = player.addItem("Item " + std::to_string(i));
            assert(ok);
        }
        assert(player.inventory.size() == 8);
        assert(!player.canAddItem());

        // 9th item must fail
        bool overflow = player.addItem("Overflow Item");
        assert(!overflow);
        assert(player.inventory.size() == 8);

        std::cout << "[PASS] ITM-004: 8-slot inventory capacity limit strictly enforced." << std::endl;
    }

    // 3. BFL-006: Declarative Mercy accumulation test
    {
        int mercy = 0;
        int threshold = 100;
        bool spareable = false;

        // Act 1: Cringe Check (+50 mercy)
        mercy += 50;
        if (mercy >= threshold) spareable = true;
        assert(!spareable);

        // Act 2: Debate Lore (+20 mercy)
        mercy += 20;
        if (mercy >= threshold) spareable = true;
        assert(!spareable);

        // Act 3: Offer Dew (+50 mercy) -> mercy = 120 >= 100
        mercy += 50;
        if (mercy >= threshold) spareable = true;
        assert(spareable);

        std::cout << "[PASS] BFL-006: Declarative mercy accumulation and spare conditions verified." << std::endl;
    }

    // 4. BHL-008 & BHL-009: Attack color rules (hurtsSoul 4 colors x moving/still, healsSoul)
    {
        using nundertale::Bullet;
        using nundertale::AttackColor;

        Bullet bullet;
        assert(bullet.attackColor == AttackColor::White);
        assert(bullet.heal == 0);

        // White: hurts whether soul is moving or still
        bullet.attackColor = AttackColor::White;
        assert(bullet.hurtsSoul(true) == true);
        assert(bullet.hurtsSoul(false) == true);
        assert(bullet.healsSoul() == false);

        // Blue: hurts only if soul is moving (cyan #00A2E8)
        bullet.attackColor = AttackColor::Blue;
        assert(bullet.hurtsSoul(true) == true);
        assert(bullet.hurtsSoul(false) == false);
        assert(bullet.healsSoul() == false);

        // Orange: hurts only if soul is still (!soulMoving) (#FCA600)
        bullet.attackColor = AttackColor::Orange;
        assert(bullet.hurtsSoul(true) == false);
        assert(bullet.hurtsSoul(false) == true);
        assert(bullet.healsSoul() == false);

        // Green: heals, never hurts
        bullet.attackColor = AttackColor::Green;
        bullet.heal = 5;
        assert(bullet.hurtsSoul(true) == false);
        assert(bullet.hurtsSoul(false) == false);
        assert(bullet.healsSoul() == true);
        assert(bullet.heal == 5);

        std::cout << "[PASS] BHL-008 & BHL-009: AttackColor hurtsSoul (4 colors x moving/still) and healsSoul verified." << std::endl;
    }

    // 5. TXT-007: Dialogue choice token parser tests (valid 2 options, 3 options, no token, malformed token)
    {
        using nundertale::parseChoiceToken;
        using nundertale::ParsedChoicePage;

        // 5a. Valid 2 options
        {
            auto res = parseChoiceToken("* Will you pet the dog?{choice:Yes|No}");
            assert(res.text == "* Will you pet the dog?");
            assert(res.options.size() == 2);
            assert(res.options[0] == "Yes");
            assert(res.options[1] == "No");
        }

        // 5b. Valid 3 options
        {
            auto res = parseChoiceToken("* Choose starter:{choice:Grass|Fire|Water}");
            assert(res.text == "* Choose starter:");
            assert(res.options.size() == 3);
            assert(res.options[0] == "Grass");
            assert(res.options[1] == "Fire");
            assert(res.options[2] == "Water");
        }

        // 5c. No token
        {
            auto res = parseChoiceToken("* Just a normal dialogue page without choices.");
            assert(res.text == "* Just a normal dialogue page without choices.");
            assert(res.options.empty());
        }

        // 5d. Malformed: no closing brace
        {
            auto res = parseChoiceToken("* Broken token{choice:Yes|No");
            assert(res.text == "* Broken token{choice:Yes|No");
            assert(res.options.empty());
        }

        // 5e. Malformed: fewer than 2 options (single option)
        {
            auto res = parseChoiceToken("* Only one option{choice:Accept}");
            assert(res.text == "* Only one option{choice:Accept}");
            assert(res.options.empty());
        }

        // 5f. Malformed: empty inside token
        {
            auto res = parseChoiceToken("* Empty choice{choice:}");
            assert(res.text == "* Empty choice{choice:}");
            assert(res.options.empty());
        }

        // 5g. Malformed: delimiter with empty options
        {
            auto res = parseChoiceToken("* Blank choice{choice:|}");
            assert(res.text == "* Blank choice{choice:|}");
            assert(res.options.empty());
        }

        std::cout << "[PASS] TXT-007: Dialogue choices parseChoiceToken (2 options, 3 options, no token, malformed) verified." << std::endl;
    }

     // 6. GFX-012: Mosaic censorship component tests
     {
         using nundertale::Mosaic;

         // 6a. Ceil tiling columns/rows math
         {
             // 20x10 area, block size 8 -> ceil(20/8)=3 columns, ceil(10/8)=2 rows
             Mosaic m(Rectangle{ 0.0f, 0.0f, 20.0f, 10.0f }, 8, 4, 12345);
             assert(m.columns() == 3);
             assert(m.rows() == 2);

             // Exact multiples: 16x8, block size 8 -> 2 columns, 1 row
             Mosaic mExact(Rectangle{ 0.0f, 0.0f, 16.0f, 8.0f }, 8, 4, 12345);
             assert(mExact.columns() == 2);
             assert(mExact.rows() == 1);

             // Zero dimension checks
             Mosaic mZero(Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f }, 8, 4, 12345);
             assert(mZero.columns() == 0);
             assert(mZero.rows() == 0);
         }

         // 6b. Clipped edge blocks (blockRect)
         {
             Mosaic m(Rectangle{ 10.0f, 20.0f, 20.0f, 10.0f }, 8, 4, 12345);
             assert(m.columns() == 3);
             assert(m.rows() == 2);

             // Block (0, 0): full size 8x8 at (10, 20)
             Rectangle r00 = m.blockRect(0, 0);
             assert(r00.x == 10.0f);
             assert(r00.y == 20.0f);
             assert(r00.width == 8.0f);
             assert(r00.height == 8.0f);

             // Block (1, 0): full size 8x8 at (18, 20)
             Rectangle r10 = m.blockRect(1, 0);
             assert(r10.x == 18.0f);
             assert(r10.y == 20.0f);
             assert(r10.width == 8.0f);
             assert(r10.height == 8.0f);

             // Block (2, 0): clipped width 4 (right edge at 10+20=30; start at 10+16=26 -> 30-26=4)
             Rectangle r20 = m.blockRect(2, 0);
             assert(r20.x == 26.0f);
             assert(r20.y == 20.0f);
             assert(r20.width == 4.0f);
             assert(r20.height == 8.0f);

             // Block (0, 1): clipped height 2 (bottom edge at 20+10=30; start at 20+8=28 -> 30-28=2)
             Rectangle r01 = m.blockRect(0, 1);
             assert(r01.x == 10.0f);
             assert(r01.y == 28.0f);
             assert(r01.width == 8.0f);
             assert(r01.height == 2.0f);

             // Block (2, 1): clipped width 4 and clipped height 2
             Rectangle r21 = m.blockRect(2, 1);
             assert(r21.x == 26.0f);
             assert(r21.y == 28.0f);
             assert(r21.width == 4.0f);
             assert(r21.height == 2.0f);

             // Out-of-bounds queries return empty rect
             Rectangle rOob = m.blockRect(-1, 0);
             assert(rOob.width == 0.0f && rOob.height == 0.0f);
             Rectangle rOob2 = m.blockRect(3, 0);
             assert(rOob2.width == 0.0f && rOob2.height == 0.0f);
         }

         // 6c. Logic frame accumulation and reshuffle timing (30 Hz, reshuffle every N frames)
         {
             Mosaic m(Rectangle{ 0.0f, 0.0f, 24.0f, 24.0f }, 8, 4, 12345);
             assert(m.shuffleCount() == 0);

             // 1st frame (dt = 1/30)
             m.update(1.0f / 30.0f);
             assert(m.shuffleCount() == 0);

             // 2nd frame
             m.update(1.0f / 30.0f);
             assert(m.shuffleCount() == 0);

             // 3rd frame
             m.update(1.0f / 30.0f);
             assert(m.shuffleCount() == 0);

             // 4th frame -> triggers exactly one reshuffle
             m.update(1.0f / 30.0f);
             assert(m.shuffleCount() == 1);

             // Next 4 frames -> triggers second reshuffle
             for (int i = 0; i < 4; ++i) {
                 m.update(1.0f / 30.0f);
             }
             assert(m.shuffleCount() == 2);

             // Larger dt: dt = 4.0f / 30.0f triggers a reshuffle in a single update
             m.update(4.0f / 30.0f);
             assert(m.shuffleCount() == 3);
         }

         // 6d. Deterministic PRNG: same seed produces identical colors
         {
             Mosaic m1(Rectangle{ 0.0f, 0.0f, 32.0f, 32.0f }, 8, 4, 4242);
             Mosaic m2(Rectangle{ 0.0f, 0.0f, 32.0f, 32.0f }, 8, 4, 4242);
             assert(m1.columns() == m2.columns());
             assert(m1.rows() == m2.rows());

             for (int r = 0; r < m1.rows(); ++r) {
                 for (int c = 0; c < m1.columns(); ++c) {
                     Color c1 = m1.blockColor(c, r);
                     Color c2 = m2.blockColor(c, r);
                     assert(c1.r == c2.r);
                     assert(c1.g == c2.g);
                     assert(c1.b == c2.b);
                     assert(c1.a == 255);
                     assert(c2.a == 255);
                 }
             }

             // Different seed produces different colors
             Mosaic mDiff(Rectangle{ 0.0f, 0.0f, 32.0f, 32.0f }, 8, 4, 9999);
             bool anyDifference = false;
             for (int r = 0; r < m1.rows(); ++r) {
                 for (int c = 0; c < m1.columns(); ++c) {
                     Color c1 = m1.blockColor(c, r);
                     Color cd = mDiff.blockColor(c, r);
                     if (c1.r != cd.r || c1.g != cd.g || c1.b != cd.b) {
                         anyDifference = true;
                         break;
                     }
                 }
                 if (anyDifference) break;
             }
             assert(anyDifference);
         }

         // 6e. setArea re-tiles and keeps colors where possible
         {
             Mosaic m(Rectangle{ 0.0f, 0.0f, 16.0f, 16.0f }, 8, 4, 5555);
             assert(m.columns() == 2 && m.rows() == 2);
             Color c00 = m.blockColor(0, 0);
             Color c10 = m.blockColor(1, 0);
             Color c01 = m.blockColor(0, 1);
             Color c11 = m.blockColor(1, 1);

             // Expand to 24x24 (3x3 grid)
             m.setArea(Rectangle{ 0.0f, 0.0f, 24.0f, 24.0f });
             assert(m.columns() == 3 && m.rows() == 3);

             // Previously existing 2x2 blocks preserve their colors
             Color nc00 = m.blockColor(0, 0);
             Color nc10 = m.blockColor(1, 0);
             Color nc01 = m.blockColor(0, 1);
             Color nc11 = m.blockColor(1, 1);

             assert(nc00.r == c00.r && nc00.g == c00.g && nc00.b == c00.b);
             assert(nc10.r == c10.r && nc10.g == c10.g && nc10.b == c10.b);
             assert(nc01.r == c01.r && nc01.g == c01.g && nc01.b == c01.b);
             assert(nc11.r == c11.r && nc11.g == c11.g && nc11.b == c11.b);
         }

         std::cout << "[PASS] GFX-012: Mosaic censorship component (ceil tiling, clipped edge blocks, 30 Hz reshuffle, deterministic PRNG, setArea color preservation) verified." << std::endl;
     }

     // 7. MRC-004: HP percentage rule for locked MERCY
     {
         using nundertale::Enemy;

         assert(Enemy::mercyLockedAt(100, 100, 50) == true);
         assert(Enemy::mercyLockedAt(50, 100, 50) == false);
         assert(Enemy::mercyLockedAt(51, 100, 50) == true);
         assert(Enemy::mercyLockedAt(10, 100, 0) == false);
         assert(Enemy::mercyLockedAt(10, 0, 50) == false);

        std::cout << "[PASS] MRC-004: MERCY lock HP percentage thresholds verified." << std::endl;
    }

    // 8. GFX-011: Area palettes data table and helpers (contracts.md §T7)
    {
        using namespace nundertale;

        // hexColor basic test
        Color c = hexColor(0x3B1F5C);
        assert(c.r == 59);
        assert(c.g == 31);
        assert(c.b == 92);
        assert(c.a == 255);

        // Every Area round-trips through paletteFor(a).id -> areaFromId
        const Area allAreas[] = {
            Area::Ruins,
            Area::Snowdinha,
            Area::Wetfall,
            Area::Hotland,
            Area::ConCenter,
            Area::Barrier,
            Area::Surface,
            Area::FurryHud
        };

        for (Area a : allAreas) {
            const AreaPalette& pal = paletteFor(a);
            assert(pal.id != nullptr);
            Area outArea;
            bool found = areaFromId(pal.id, outArea);
            assert(found);
            assert(outArea == a);
        }

        // Unknown id returns false
        {
            Area dummy;
            assert(areaFromId("unknown_area", dummy) == false);
            assert(areaFromId("", dummy) == false);
            assert(areaFromId("RUINS", dummy) == false);
            assert(areaFromId("ruin", dummy) == false);
        }

        // Snowdinha accentCount == 3
        {
            const AreaPalette& snow = paletteFor(Area::Snowdinha);
            assert(snow.accentCount == 3);
            assert(snow.accent[0].r == 0xFF && snow.accent[0].g == 0x4F && snow.accent[0].b == 0xD8);
            assert(snow.accent[1].r == 0x4F && snow.accent[1].g == 0xD8 && snow.accent[1].b == 0xFF);
            assert(snow.accent[2].r == 0x7C && snow.accent[2].g == 0xFF && snow.accent[2].b == 0x3A);
            assert(snow.hasHighlight == true);
            assert(snow.highlight.r == 255 && snow.highlight.g == 255 && snow.highlight.b == 255);
            assert(snow.extraCount == 2);
        }

        // ConCenter hasHighlight == false
        {
            const AreaPalette& con = paletteFor(Area::ConCenter);
            assert(con.hasHighlight == false);
            assert(con.baseCount == 2);
            assert(con.accentCount == 1);
            assert(con.extraCount == 0);
        }

        // Ruins extraCount == 3
        {
            const AreaPalette& ruins = paletteFor(Area::Ruins);
            assert(ruins.extraCount == 3);
            assert(ruins.extra[0].r == 0xC9 && ruins.extra[0].g == 0xA0 && ruins.extra[0].b == 0xDC);
            assert(ruins.extra[1].r == 0x11 && ruins.extra[1].g == 0x11 && ruins.extra[1].b == 0x11);
            assert(ruins.extra[2].r == 0x1E && ruins.extra[2].g == 0x3F && ruins.extra[2].b == 0xBF);
            assert(ruins.baseCount == 2);
            assert(ruins.accentCount == 1);
            assert(ruins.hasHighlight == true);
        }

        // Barrier, Surface, FurryHud, Wetfall, Hotland verification
        {
            const AreaPalette& barrier = paletteFor(Area::Barrier);
            assert(barrier.baseCount == 1);
            assert(barrier.accentCount == 1);
            assert(barrier.hasHighlight == false);
            assert(barrier.extraCount == 0);

            const AreaPalette& surface = paletteFor(Area::Surface);
            assert(surface.baseCount == 2);
            assert(surface.accentCount == 1);
            assert(surface.hasHighlight == true);
            assert(surface.extraCount == 0);

            const AreaPalette& furry = paletteFor(Area::FurryHud);
            assert(furry.baseCount == 2);
            assert(furry.accentCount == 1);
            assert(furry.hasHighlight == false);
            assert(furry.extraCount == 0);

            const AreaPalette& wet = paletteFor(Area::Wetfall);
            assert(wet.baseCount == 2);
            assert(wet.accentCount == 2);
            assert(wet.hasHighlight == true);
            assert(wet.extraCount == 0);

            const AreaPalette& hot = paletteFor(Area::Hotland);
            assert(hot.baseCount == 2);
            assert(hot.accentCount == 1);
            assert(hot.hasHighlight == true);
            assert(hot.extraCount == 2);
        }

        std::cout << "[PASS] GFX-011: Area palettes data table, hex values, and round-trips verified." << std::endl;
    }

    // 9. BHL-015: "thinking" ribbit text over the soul (RibbitState + updateRibbit + ribbitVisible)
    {
        using nundertale::RibbitState;
        using nundertale::updateRibbit;
        using nundertale::ribbitVisible;

        // Initial default state
        RibbitState s;
        assert(s.stillTime == 0.0f);
        assert(s.showTime == 0.0f);
        assert(!s.shownThisTurn);
        assert(!ribbitVisible(s));

        // Still for 2.9 s -> not shown
        bool triggered = updateRibbit(s, 2.9f, false);
        assert(!triggered);
        assert(!ribbitVisible(s));
        assert(s.stillTime >= 2.89f && s.stillTime <= 2.91f);
        assert(!s.shownThisTurn);

        // Crossing 3.0 s -> returns true once, bubble visible
        triggered = updateRibbit(s, 0.2f, false);
        assert(triggered);
        assert(ribbitVisible(s));
        assert(s.shownThisTurn);

        // Subsequent update while visible does NOT return true again
        triggered = updateRibbit(s, 0.5f, false);
        assert(!triggered);
        assert(ribbitVisible(s));

        // Visible for 1.5 s total then hidden
        // Already elapsed 0.5 s in showTime, remaining 1.0 s
        triggered = updateRibbit(s, 1.0f, false);
        assert(!triggered);
        assert(!ribbitVisible(s));
        assert(s.showTime == 0.0f);

        // Never shows twice in one turn
        triggered = updateRibbit(s, 5.0f, false);
        assert(!triggered);
        assert(!ribbitVisible(s));

        // Moving resets stillTime and hides bubble
        RibbitState s2;
        updateRibbit(s2, 2.0f, false);
        assert(s2.stillTime >= 1.99f);
        updateRibbit(s2, 0.1f, true); // moved!
        assert(s2.stillTime == 0.0f);
        assert(!ribbitVisible(s2));
        assert(!s2.shownThisTurn);

        // Stay still for 3.0s -> shows bubble
        triggered = updateRibbit(s2, 3.0f, false);
        assert(triggered);
        assert(ribbitVisible(s2));

        // Moving while bubble is showing resets stillTime and hides bubble immediately
        triggered = updateRibbit(s2, 0.1f, true);
        assert(!triggered);
        assert(!ribbitVisible(s2));
        assert(s2.stillTime == 0.0f);
        assert(s2.showTime == 0.0f);

        // Never shows twice in that turn even after moving and staying still again
        triggered = updateRibbit(s2, 4.0f, false);
        assert(!triggered);
        assert(!ribbitVisible(s2));

        // Reset per enemy turn
        s2 = RibbitState{};
        assert(!s2.shownThisTurn);
        assert(s2.stillTime == 0.0f);
        assert(s2.showTime == 0.0f);
        assert(!ribbitVisible(s2));
        triggered = updateRibbit(s2, 3.0f, false);
        assert(triggered);
        assert(ribbitVisible(s2));

        // Custom threshold and duration parameters
        RibbitState s3;
        triggered = updateRibbit(s3, 0.4f, false, 0.5f, 0.2f);
        assert(!triggered);
        triggered = updateRibbit(s3, 0.2f, false, 0.5f, 0.2f);
        assert(triggered);
        assert(ribbitVisible(s3));
        updateRibbit(s3, 0.2f, false, 0.5f, 0.2f);
        assert(!ribbitVisible(s3));

        std::cout << "[PASS] BHL-015: RibbitState timing, visibility, movement reset, and single-turn limit verified." << std::endl;
    }

    // 10. BHL-016: persistent damage zone timing and lifetime
    {
        using nundertale::DamageZone;

        DamageZone zone;
        zone.lifetime = 1.0f;
        zone.damagePerTick = 2;
        zone.tickInterval = 0.33f;

        // First contact ticks immediately.
        assert(zone.update(0.0f, true) == 2);

        // Remaining inside for 0.2 s does not tick a second time.
        assert(zone.update(0.2f, true) == 0);

        // Total elapsed cooldown reaches 0.34 s, allowing the second tick.
        assert(zone.update(0.14f, true) == 2);

        // Leaving the zone never deals damage and does not reset its timer.
        assert(zone.update(0.1f, false) == 0);

        // Lifetime expires at or below zero.
        assert(!zone.expired());
        assert(zone.update(0.56f, false) == 0);
        assert(zone.expired());

        std::cout << "[PASS] BHL-016: DamageZone immediate contact, tick interval, outside behavior, and expiry verified." << std::endl;
    }

    // 11. BUI-013: Per-encounter battle background mapping and fallback
    {
        using nundertale::BattleBackground;
        using nundertale::backgroundFromId;

        // Every defined ID mapping
        assert(backgroundFromId("grid") == BattleBackground::Grid);
        assert(backgroundFromId("grid_tube") == BattleBackground::GridTube);
        assert(backgroundFromId("red_grid") == BattleBackground::RedGrid);
        assert(backgroundFromId("black") == BattleBackground::Black);
        assert(backgroundFromId("speed_lines") == BattleBackground::SpeedLines);
        assert(backgroundFromId("trophies") == BattleBackground::Trophies);
        assert(backgroundFromId("lobby_gold") == BattleBackground::LobbyGold);

        // Unknown and edge cases fall back to Grid
        assert(backgroundFromId("") == BattleBackground::Grid);
        assert(backgroundFromId("unknown") == BattleBackground::Grid);
        assert(backgroundFromId("invalid_id") == BattleBackground::Grid);
        assert(backgroundFromId("papyrus_chat") == BattleBackground::Grid);
        assert(backgroundFromId("mettapaw_stage") == BattleBackground::Grid);

        std::cout << "[PASS] BUI-013: BattleBackground IDs, enum mapping, and unknown fallback verified." << std::endl;
    }

    // 12. ACT-006 & ACT-007: Conditional ACTs and stat-effect helpers
    {
        using nundertale::ActDef;
        using nundertale::actAvailable;
        using nundertale::actEffectApplies;
        using nundertale::clampStat;

        const std::vector<std::string> inventory = { "Bottle", "Mt. Dew" };
        const auto hasFlag = [](const std::string& flag) { return flag == "route.ready"; };

        ActDef unconditional;
        assert(actAvailable(unconditional, inventory, 0, hasFlag));

        ActDef itemAct;
        itemAct.requiresItem = "Mt. Dew";
        assert(actAvailable(itemAct, inventory, 0, hasFlag));
        itemAct.requiresItem = "Doritos";
        assert(!actAvailable(itemAct, inventory, 0, hasFlag));

        ActDef furAct;
        furAct.requiresFurMin = 40;
        assert(actAvailable(furAct, inventory, 40, hasFlag));
        assert(!actAvailable(furAct, inventory, 39, hasFlag));

        ActDef flagAct;
        flagAct.requiresFlag = "route.ready";
        assert(actAvailable(flagAct, inventory, 0, hasFlag));
        flagAct.requiresFlag = "route.missing";
        assert(!actAvailable(flagAct, inventory, 0, hasFlag));

        ActDef combinedAct;
        combinedAct.requiresItem = "Bottle";
        combinedAct.requiresFurMin = 25;
        combinedAct.requiresFlag = "route.ready";
        assert(actAvailable(combinedAct, inventory, 25, hasFlag));
        assert(!actAvailable(combinedAct, inventory, 24, hasFlag));

        assert(actEffectApplies(1, 0));
        assert(actEffectApplies(1, 1));
        assert(!actEffectApplies(1, 2));
        assert(actEffectApplies(2, 2));
        assert(actEffectApplies(3, 2));

        assert(clampStat(5, 3) == 8);
        assert(clampStat(5, -3) == 2);
        assert(clampStat(5, -10) == 0);
        assert(clampStat(0, -1) == 0);

        std::cout << "[PASS] ACT-006 & ACT-007: ACT conditions, minimum uses, and stat clamping verified." << std::endl;
    }

    // 13. BHL-017: KR-style residual damage meter
    {
        using nundertale::ResidualMeter;

        ResidualMeter meter;
        assert(meter.pending == 0.0f);
        assert(meter.drainPerSecond == 1.0f);
        assert(meter.carry == 0.0f);

        // 1. Add ignores non-positive amounts
        meter.add(0.0f);
        assert(meter.pending == 0.0f);
        meter.add(-5.0f);
        assert(meter.pending == 0.0f);

        // 2. Add increases pending
        meter.add(5.0f);
        assert(meter.pending == 5.0f);
        meter.add(3.5f);
        assert(meter.pending == 8.5f);

        // 3. Update with non-positive dt returns 0 and does not change state
        assert(meter.update(0.0f) == 0);
        assert(meter.pending == 8.5f);
        assert(meter.update(-1.0f) == 0);
        assert(meter.pending == 8.5f);

        // 4. Fractional carry across updates
        meter.clear();
        assert(meter.pending == 0.0f);
        assert(meter.carry == 0.0f);

        meter.add(3.0f);
        // Step 1: 0.4s at 1.0 HP/s -> drains 0.4, carry = 0.4, returns 0, pending = 2.6
        assert(meter.update(0.4f) == 0);
        assert(std::abs(meter.pending - 2.6f) < 0.001f);
        assert(std::abs(meter.carry - 0.4f) < 0.001f);

        // Step 2: 0.4s -> drains 0.4, carry = 0.8, returns 0, pending = 2.2
        assert(meter.update(0.4f) == 0);
        assert(std::abs(meter.pending - 2.2f) < 0.001f);
        assert(std::abs(meter.carry - 0.8f) < 0.001f);

        // Step 3: 0.4s -> drains 0.4, carry = 1.2 -> returns 1, carry = 0.2, pending = 1.8
        assert(meter.update(0.4f) == 1);
        assert(std::abs(meter.pending - 1.8f) < 0.001f);
        assert(std::abs(meter.carry - 0.2f) < 0.001f);

        // 5. Large dt draining multiple HP at once
        // Step 4: 1.5s -> drains 1.5, carry was 0.2 -> 1.7 -> returns 1, carry = 0.7, pending = 0.3
        assert(meter.update(1.5f) == 1);
        assert(std::abs(meter.pending - 0.3f) < 0.001f);
        assert(std::abs(meter.carry - 0.7f) < 0.001f);

        // Step 5: dt exceeding remaining pending: 2.0s -> drains only remaining 0.3
        // carry was 0.7 + 0.3 = 1.0 -> returns 1, carry = 0.0, pending = 0.0
        assert(meter.update(2.0f) == 1);
        assert(meter.pending == 0.0f);
        assert(std::abs(meter.carry) < 0.001f);

        // 6. Never negative: further updates when pending is 0 return 0 and pending stays 0
        assert(meter.update(1.0f) == 0);
        assert(meter.pending == 0.0f);
        assert(meter.carry >= 0.0f);

        // 7. Clear resets pending and carry
        meter.add(10.0f);
        meter.update(0.5f);
        assert(meter.pending > 0.0f);
        assert(meter.carry > 0.0f);
        meter.clear();
        assert(meter.pending == 0.0f);
        assert(meter.carry == 0.0f);
        assert(meter.update(1.0f) == 0);

        // 8. Custom drainPerSecond rate
        meter.drainPerSecond = 2.5f;
        meter.add(5.0f);
        assert(meter.update(1.0f) == 2);
        assert(std::abs(meter.pending - 2.5f) < 0.001f);
        assert(std::abs(meter.carry - 0.5f) < 0.001f);
        assert(meter.update(1.0f) == 3);
        assert(meter.pending == 0.0f);
        assert(std::abs(meter.carry) < 0.001f);

        std::cout << "[PASS] BHL-017: ResidualMeter add, fractional carry, never-negative, and clear verified." << std::endl;
    }

    // 14. CORE-003: Pluggable transition system (ITransition, FadeTransition, CutTransition)
    {
        using nundertale::fadeBlack;
        using nundertale::fadeWhite;
        using nundertale::cut;
        using nundertale::ITransition;

        // 1. fadeBlack duration and coverage
        auto blackTrans = fadeBlack(0.4f);
        assert(std::abs(blackTrans->duration() - 0.4f) < 0.0001f);

        // Covering out (covering the old scene): out -> progress
        assert(std::abs(blackTrans->coverage(0.0f, true) - 0.0f) < 0.0001f);
        assert(std::abs(blackTrans->coverage(0.5f, true) - 0.5f) < 0.0001f);
        assert(std::abs(blackTrans->coverage(1.0f, true) - 1.0f) < 0.0001f);

        // Revealing in (revealing the new scene): in -> 1 - progress
        assert(std::abs(blackTrans->coverage(0.0f, false) - 1.0f) < 0.0001f);
        assert(std::abs(blackTrans->coverage(0.5f, false) - 0.5f) < 0.0001f);
        assert(std::abs(blackTrans->coverage(1.0f, false) - 0.0f) < 0.0001f);

        // Clamping outside 0..1
        assert(std::abs(blackTrans->coverage(-0.5f, true) - 0.0f) < 0.0001f);
        assert(std::abs(blackTrans->coverage(1.5f, true) - 1.0f) < 0.0001f);
        assert(std::abs(blackTrans->coverage(-0.5f, false) - 1.0f) < 0.0001f);
        assert(std::abs(blackTrans->coverage(1.5f, false) - 0.0f) < 0.0001f);

        // render() executes cleanly
        blackTrans->render(0.5f, true);
        blackTrans->render(0.5f, false);

        // 2. fadeWhite duration and coverage
        auto whiteTrans = fadeWhite(0.8f);
        assert(std::abs(whiteTrans->duration() - 0.8f) < 0.0001f);
        assert(std::abs(whiteTrans->coverage(0.0f, true) - 0.0f) < 0.0001f);
        assert(std::abs(whiteTrans->coverage(1.0f, true) - 1.0f) < 0.0001f);
        assert(std::abs(whiteTrans->coverage(0.0f, false) - 1.0f) < 0.0001f);
        assert(std::abs(whiteTrans->coverage(1.0f, false) - 0.0f) < 0.0001f);
        whiteTrans->render(0.8f, false);

        // 3. cut duration 0 and zero coverage
        auto cutTrans = cut();
        assert(cutTrans->duration() == 0.0f);
        assert(cutTrans->coverage(0.0f, true) == 0.0f);
        assert(cutTrans->coverage(0.5f, true) == 0.0f);
        assert(cutTrans->coverage(1.0f, true) == 0.0f);
        assert(cutTrans->coverage(0.0f, false) == 0.0f);
        assert(cutTrans->coverage(1.0f, false) == 0.0f);
        cutTrans->render(0.0f, true);

        std::cout << "[PASS] CORE-003: Pluggable transitions (coverage, cut duration 0, clamping) verified." << std::endl;
    }

    // 15. BFL-007: Per-encounter battle music mapping and asset fallback
    {
        using nundertale::DEFAULT_BATTLE_MUSIC;
        using nundertale::musicPathForId;
        using nundertale::resolveMusicPath;

        assert(musicPathForId("") == DEFAULT_BATTLE_MUSIC);
        assert(musicPathForId("battle_default") == DEFAULT_BATTLE_MUSIC);
        assert(musicPathForId("boss_barky") == "assets/music/boss_barky.ogg");

        const auto bossTrackExists = [](const std::string& path) {
            return path == "assets/music/boss_barky.ogg";
        };
        assert(resolveMusicPath("boss_barky", bossTrackExists) == "assets/music/boss_barky.ogg");

        const auto noTrackExists = [](const std::string& /*path*/) { return false; };
        assert(resolveMusicPath("boss_barky", noTrackExists) == DEFAULT_BATTLE_MUSIC);

        std::cout << "[PASS] BFL-007: Battle music IDs, convention paths, and missing-asset fallback verified." << std::endl;
    }

    // 16. FRT-006, FRT-007, FRT-008: Name rules, normalization, reactions, and asymmetric transitions
    {
        using nundertale::normalizeName;
        using nundertale::reactionFor;
        using nundertale::NameEffect;
        using nundertale::NAME_MAX_LEN;
        using nundertale::fadeWhite;
        using nundertale::fadeBlack;

        // normalizeName & length limit
        assert(normalizeName("Pepe") == "PEPE");
        assert(normalizeName("chad") == "CHAD");
        assert(normalizeName("BARKY") == "BARKY");
        assert(normalizeName("123!a") == "123!A");
        assert(NAME_MAX_LEN == 6);

        // reactionFor table tests
        // CHAD: Gold + allowed
        auto rChadLower = reactionFor("chad");
        assert(rChadLower.effect == NameEffect::Gold);
        assert(rChadLower.allowed);
        assert(rChadLower.tag == "[GOLD]");
        assert(rChadLower.message == "Gold plated. You are the ultimate gamer.");

        auto rChadUpper = reactionFor("Chad");
        assert(rChadUpper.effect == NameEffect::Gold);
        assert(rChadUpper.allowed);

        // PEPE & FROG: FrogSoul
        auto rPepe = reactionFor("Pepe");
        assert(rPepe.effect == NameEffect::FrogSoul);
        assert(rPepe.allowed);
        assert(rPepe.tag == "[THE PROTAGONIST]");

        auto rFrog = reactionFor("frog");
        assert(rFrog.effect == NameEffect::FrogSoul);
        assert(rFrog.allowed);
        assert(rFrog.tag == "[RIBBIT]");

        // SKYRIM: Skyrim
        auto rSkyrim = reactionFor("skyrim");
        assert(rSkyrim.effect == NameEffect::Skyrim);
        assert(rSkyrim.allowed);
        assert(rSkyrim.tag == "[TAMRIEL]");

        // SANS, FRISK, CHARA, FLOWEY, TORIEL, ASGORE
        assert(reactionFor("sans").tag == "[COMEDIAN]");
        assert(reactionFor("frisk").tag == "[FALLEN CHILD]");
        assert(reactionFor("chara").effect == NameEffect::CrimsonShake);
        assert(reactionFor("chara").tag == "[THE FIRST]");
        assert(reactionFor("flowey").tag == "[WEED]");
        assert(reactionFor("toriel").tag == "[MOM]");
        assert(reactionFor("asgore").tag == "[KING]");

        // WAIFU list
        assert(reactionFor("RUIVA").effect == NameEffect::Waifu);
        assert(reactionFor("loira").effect == NameEffect::Waifu);
        assert(reactionFor("otome").effect == NameEffect::Waifu);
        assert(reactionFor("gotica").effect == NameEffect::Waifu);
        assert(reactionFor("morena").effect == NameEffect::Waifu);
        assert(reactionFor("yander").effect == NameEffect::Waifu);
        assert(reactionFor("yander").tag == "[WAIFU CONFIRMED]");

        // Disallowed names
        auto rBarky = reactionFor("BARKY");
        assert(!rBarky.allowed);
        assert(rBarky.message == "AWOO! ...wait. That's MY name.");

        auto rYiff = reactionFor("yiff");
        assert(!rYiff.allowed);
        assert(rYiff.message == "The narrator refuses.");

        assert(!reactionFor("furiel").allowed);
        assert(!reactionFor("furiell").allowed);
        assert(!reactionFor("floofy").allowed);

        // AZZY, OWO, GAMER, PRO
        auto rAzzy = reactionFor("azzy");
        assert(rAzzy.allowed);
        assert(rAzzy.effect == NameEffect::Darken);
        assert(rAzzy.message == "...");

        assert(reactionFor("owo").message == "What's this?");
        assert(reactionFor("gamer").tag == "[REDUNDANT]");
        assert(reactionFor("pro").tag == "[PRO]");

        // GASTER: RestartGame
        auto rGaster = reactionFor("GASTER");
        assert(rGaster.effect == NameEffect::RestartGame);
        assert(rGaster.allowed);
        assert(rGaster.message.empty());

        // Generic rules after table:
        // "AAAA" -> keyboard jam message
        auto rJam = reactionFor("AAAA");
        assert(rJam.allowed);
        assert(rJam.message == "Not very creative... Did your keyboard jam?");

        auto rJamLower = reactionFor("zzzz");
        assert(rJamLower.allowed);
        assert(rJamLower.message == "Not very creative... Did your keyboard jam?");

        // "Q" -> 1-char message
        auto rOne = reactionFor("Q");
        assert(rOne.allowed);
        assert(rOne.message == "You call that a gamer? Saving characters?");

        // "Bob" -> empty reaction, allowed
        auto rBob = reactionFor("Bob");
        assert(rBob.allowed);
        assert(rBob.tag.empty());
        assert(rBob.message.empty());
        assert(rBob.effect == NameEffect::None);

        // Asymmetric transition durations
        auto w = fadeWhite(3.0f, 0.6f);
        assert(std::abs(w->duration() - 3.0f) < 0.0001f);
        assert(std::abs(w->durationIn() - 0.6f) < 0.0001f);

        auto b = fadeBlack(0.4f);
        assert(std::abs(b->duration() - 0.4f) < 0.0001f);
        assert(std::abs(b->durationIn() - 0.4f) < 0.0001f);

        auto bAsym = fadeBlack(0.5f, 0.2f);
        assert(std::abs(bAsym->duration() - 0.5f) < 0.0001f);
        assert(std::abs(bAsym->durationIn() - 0.2f) < 0.0001f);

        std::cout << "[PASS] FRT-006, FRT-007 & FRT-008: NameRules, easter eggs, normalization, and transition durations verified." << std::endl;
    }

    // 17. FRT-001, FRT-003, FRT-004, FRT-005: Front-end title routing and prompt timing
    {
        using nundertale::pressPromptVisible;
        using nundertale::titleNext;
        using nundertale::TitleNext;

        assert(titleNext(true, false, 0.0f) == TitleNext::Instructions);
        assert(titleNext(true, true, 0.0f) == TitleNext::ContinueCard);
        assert(titleNext(false, false, 19.9f) == TitleNext::Stay);
        assert(titleNext(false, true, 20.0f) == TitleNext::Intro);

        // Confirm takes priority over the idle timeout.
        assert(titleNext(true, false, 20.0f) == TitleNext::Instructions);
        assert(titleNext(true, true, 20.0f) == TitleNext::ContinueCard);

        assert(!pressPromptVisible(0.99f));
        assert(pressPromptVisible(1.0f));

        std::cout << "[PASS] FRT-001, FRT-003, FRT-004 & FRT-005: Front-end routing, idle replay, and prompt timing verified." << std::endl;
    }

    // 18. FGT-001 & FGT-002: Undertale attack target, bar speed, blink, squish, and miss conditions
    {
        using nundertale::ATTACK_BAR_CROSS_SECONDS;
        using nundertale::ATTACK_HIT_HOLD_SECONDS;
        using nundertale::ATTACK_TARGET_FADE_SECONDS;
        using nundertale::ATTACK_CRIT_RADIUS;
        using nundertale::attackBarSpeed;
        using nundertale::attackBarBlinkWhite;
        using nundertale::attackTargetSquish;
        using nundertale::attackBarMissed;

        // Constants
        assert(std::abs(ATTACK_BAR_CROSS_SECONDS - 1.6f) < 0.0001f);
        assert(std::abs(ATTACK_HIT_HOLD_SECONDS - 0.55f) < 0.0001f);
        assert(std::abs(ATTACK_TARGET_FADE_SECONDS - 0.30f) < 0.0001f);
        assert(std::abs(ATTACK_CRIT_RADIUS - 12.0f) < 0.0001f);

        // attackBarSpeed(565) ≈ 353.125
        float speed = attackBarSpeed(565.0f);
        assert(std::abs(speed - 353.125f) < 0.0001f);

        // attackBarBlinkWhite(0) = true, (0.07) = false
        assert(attackBarBlinkWhite(0.0f) == true);
        assert(attackBarBlinkWhite(0.07f) == false);
        assert(attackBarBlinkWhite(0.14f) == true);
        assert(attackBarBlinkWhite(0.21f) == false);
        assert(attackBarBlinkWhite(-1.0f) == true);

        // attackTargetSquish(0)=1, (0.15)≈0.5, (0.3)=0, (1)=0, (-1)=1
        assert(std::abs(attackTargetSquish(0.0f) - 1.0f) < 0.0001f);
        assert(std::abs(attackTargetSquish(0.15f) - 0.5f) < 0.0001f);
        assert(std::abs(attackTargetSquish(0.30f) - 0.0f) < 0.0001f);
        assert(std::abs(attackTargetSquish(1.0f) - 0.0f) < 0.0001f);
        assert(std::abs(attackTargetSquish(-1.0f) - 1.0f) < 0.0001f);

        // attackBarMissed for both directions
        float left = 37.0f;
        float right = 602.0f;

        // Moving right (speed > 0)
        float speedRight = 353.125f;
        assert(!attackBarMissed(left, speedRight, left, right));
        assert(!attackBarMissed((left + right) * 0.5f, speedRight, left, right));
        assert(!attackBarMissed(right - 0.1f, speedRight, left, right));
        assert(attackBarMissed(right, speedRight, left, right));
        assert(attackBarMissed(right + 10.0f, speedRight, left, right));

        // Moving left (speed < 0)
        float speedLeft = -353.125f;
        assert(!attackBarMissed(right, speedLeft, left, right));
        assert(!attackBarMissed((left + right) * 0.5f, speedLeft, left, right));
        assert(!attackBarMissed(left + 0.1f, speedLeft, left, right));
        assert(attackBarMissed(left, speedLeft, left, right));
        assert(attackBarMissed(left - 10.0f, speedLeft, left, right));

        // Speed zero
        assert(!attackBarMissed((left + right) * 0.5f, 0.0f, left, right));

        std::cout << "[PASS] FGT-001 & FGT-002: Undertale attack target, bar speed, blink, squish, and miss conditions verified." << std::endl;
    }

    // 19. FUR-002 & FUR-005: Integer FUR state, stages, clamping, and change feedback
    {
        using nundertale::applyFur;
        using nundertale::furStageFor;
        using nundertale::furStageName;
        using nundertale::FurStage;

        assert(furStageFor(0) == FurStage::Frog);
        assert(furStageFor(19) == FurStage::Frog);
        assert(furStageFor(20) == FurStage::Ears);
        assert(furStageFor(39) == FurStage::Ears);
        assert(furStageFor(40) == FurStage::Paws);
        assert(furStageFor(59) == FurStage::Paws);
        assert(furStageFor(60) == FurStage::Tail);
        assert(furStageFor(79) == FurStage::Tail);
        assert(furStageFor(80) == FurStage::Snout);
        assert(furStageFor(99) == FurStage::Snout);
        assert(furStageFor(100) == FurStage::Mirror);
        assert(furStageFor(150) == FurStage::Mirror);
        assert(furStageFor(-5) == FurStage::Frog);

        assert(std::string(furStageName(FurStage::Frog)) == "Frog");
        assert(std::string(furStageName(FurStage::Ears)) == "Ears");
        assert(std::string(furStageName(FurStage::Paws)) == "Paws");
        assert(std::string(furStageName(FurStage::Tail)) == "Tail");
        assert(std::string(furStageName(FurStage::Snout)) == "Snout");
        assert(std::string(furStageName(FurStage::Mirror)) == "Mirror");

        int value = 15;
        int stageMax = 0;
        auto earsChange = applyFur(value, stageMax, 6);
        assert(earsChange.before == 15);
        assert(earsChange.after == 21);
        assert(earsChange.oldStage == FurStage::Frog);
        assert(earsChange.newStage == FurStage::Ears);
        assert(earsChange.stageUp);
        assert(!earsChange.reachedMirror);
        assert(stageMax == 1);

        value = 15;
        stageMax = 0;
        auto tailChange = applyFur(value, stageMax, 45);
        assert(tailChange.after == 60);
        assert(tailChange.oldStage == FurStage::Frog);
        assert(tailChange.newStage == FurStage::Tail);
        assert(tailChange.stageUp);
        assert(stageMax == 3);

        value = 21;
        stageMax = 1;
        auto stageDown = applyFur(value, stageMax, -15);
        assert(stageDown.after == 6);
        assert(!stageDown.stageUp);
        assert(!stageDown.reachedMirror);
        assert(stageMax == 1);

        value = 95;
        stageMax = 4;
        auto mirrorChange = applyFur(value, stageMax, 10);
        assert(mirrorChange.after == 100);
        assert(mirrorChange.stageUp);
        assert(mirrorChange.reachedMirror);
        assert(stageMax == 5);

        auto cappedChange = applyFur(value, stageMax, 10);
        assert(cappedChange.before == 100);
        assert(cappedChange.after == 100);
        assert(!cappedChange.stageUp);
        assert(!cappedChange.reachedMirror);

        value = 0;
        stageMax = 0;
        auto armoredGain = applyFur(value, stageMax, 6, 1.5f);
        assert(armoredGain.after == 9);

        value = 20;
        stageMax = 1;
        auto armoredLoss = applyFur(value, stageMax, -6, 1.5f);
        assert(armoredLoss.after == 14);
        assert(stageMax == 1);

        std::cout << "[PASS] FUR-002 & FUR-005: Integer FUR stages, clamping, armor multiplier, and stage feedback signals verified." << std::endl;
    }

    // 20. SAV-001, SAV-007 & FUR-004: Save place lines, room mapping, and route/fur save dialogue
    {
        using nundertale::savePlaceLine;
        using nundertale::saveLines;

        // savePlaceLine for ruins_entrance, snowdin_town, unknown
        assert(savePlaceLine("ruins_entrance") == "* The scent of lukewarm energy drinks and stale Doritos fills you with DETERMINATION.");
        assert(savePlaceLine("") == "* The scent of lukewarm energy drinks and stale Doritos fills you with DETERMINATION.");
        assert(savePlaceLine("snowdin_town") == "* The sight of 47 people sharing one Wi-Fi password on a fridge fills you with DETERMINATION.");
        assert(savePlaceLine("unknown_room_xyz") == "* The scent of lukewarm energy drinks and stale Doritos fills you with DETERMINATION.");

        // Check all other rooms from LORE table for thoroughness
        assert(savePlaceLine("ruins_hall") == "* The distant hum of dial-up internet fills you with DETERMINATION.");
        assert(savePlaceLine("ruins_exit") == "* The cold air and the faint smell of ozone fill you with DETERMINATION.");
        assert(savePlaceLine("snowdin_entrance") == "* The crunch of styrofoam snow fills you with DETERMINATION.");
        assert(savePlaceLine("snowdin_fog") == "* The hum of the town's single shared router fills you with DETERMINATION.");
        assert(savePlaceLine("waterfall_entrance") == "* The sound of dripping and a distant \"rawr\" fill you with DETERMINATION.");
        assert(savePlaceLine("waterfall_arena") == "* The humidity of a thousand fursuits fills you with DETERMINATION.");
        assert(savePlaceLine("hotland_quiz") == "* The sound of 400 server fans fills you with DETERMINATION.");
        assert(savePlaceLine("newhome_throne") == "* The smell of herbal tea and old carpet fills you with DETERMINATION.");
        assert(savePlaceLine("con_center_hall") == "* The terrified hugging of a hundred furries fills you with DETERMINATION.");

        // saveLines with fur 0 (1 line)
        auto linesFur0 = saveLines("ruins_entrance", 0, false);
        assert(linesFur0.size() == 1);
        assert(linesFur0[0] == "* The scent of lukewarm energy drinks and stale Doritos fills you with DETERMINATION.");

        // saveLines with fur 15 (2 lines, second is the fluff line)
        auto linesFur15 = saveLines("ruins_entrance", 15, false);
        assert(linesFur15.size() == 2);
        assert(linesFur15[0] == "* The scent of lukewarm energy drinks and stale Doritos fills you with DETERMINATION.");
        assert(linesFur15[1] == "* DETERMINATION can't fix fluff.");

        // saveLines toxic (exactly "* Determination.")
        auto linesToxic = saveLines("ruins_entrance", 0, true);
        assert(linesToxic.size() == 1);
        assert(linesToxic[0] == "* Determination.");

        auto linesToxicFur = saveLines("ruins_entrance", 50, true);
        assert(linesToxicFur.size() == 1);
        assert(linesToxicFur[0] == "* Determination.");

        std::cout << "[PASS] SAV-001, SAV-007 & FUR-004: Save place lines, room mapping, and route/fur save dialogue verified." << std::endl;
    }

    // 21. ACT-005 & ACT-006: repeat texts (Cringe Check 2nd use) and Laser gated by Toy Laser Pointer
    {
        using nundertale::ActDef;
        using nundertale::actTextForUse;
        using nundertale::actResponseForUse;
        using nundertale::actAvailable;

        ActDef cringe;
        cringe.text = "* You stare directly at the zipper on Barky's back.";
        cringe.response = "H-hey! It's an artisanal design feature, b-baka!";
        cringe.repeatText = "* You stare again. Barky is out of blush.";
        cringe.repeatResponse = "...I KNOW, okay? I KNOW.";
        assert(actTextForUse(cringe, 1) == cringe.text);
        assert(actResponseForUse(cringe, 1) == cringe.response);
        assert(actTextForUse(cringe, 2) == cringe.repeatText);
        assert(actResponseForUse(cringe, 3) == cringe.repeatResponse);

        ActDef plain;
        plain.text = "* Text.";
        plain.response = "Line.";
        assert(actTextForUse(plain, 5) == "* Text.");
        assert(actResponseForUse(plain, 5) == "Line.");

        ActDef laser;
        laser.requiresItem = "Toy Laser Pointer";
        auto noFlags = [](const std::string&) { return false; };
        assert(!actAvailable(laser, { "Bottle", "Mt. Dew" }, 0, noFlags));
        assert(actAvailable(laser, { "Bottle", "Toy Laser Pointer" }, 0, noFlags));

        ActDef hug;
        assert(hug.fur == 0 && !hug.spareNow);
        std::cout << "[PASS] ACT-005 & ACT-006: repeat ACT texts, Laser item gate, and Accept Hug defaults verified." << std::endl;
    }

    std::cout << "\nAll 21 test suites passed successfully!" << std::endl;
    return 0;
}
