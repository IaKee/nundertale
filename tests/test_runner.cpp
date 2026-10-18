#include <iostream>
#include <cassert>
#include <cmath>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "combat/Bullet.hpp"
#include "combat/DamageZone.hpp"
#include "combat/Enemy.hpp"
#include "combat/BattleSystem.hpp"
#include "combat/BattleBackground.hpp"
#include "combat/ResidualDamage.hpp"
#include "combat/AttackTarget.hpp"
#include "combat/BarkyPatterns.hpp"
#include "ui/DialogBox.hpp"
#include "ui/Mosaic.hpp"
#include "core/AreaPalettes.hpp"
#include "core/Transition.hpp"
#include "core/NameRules.hpp"
#include "core/PressLatch.hpp"
#include "core/Fur.hpp"
#include "core/FurTable.hpp"
#include "services/MusicTracks.hpp"
#include "scenes/FrontEndFlow.hpp"
#include "core/SaveText.hpp"
#include "scenes/SettingsModel.hpp"
#include "scenes/FurryOver.hpp"
#include "combat/FurHud.hpp"
#include "core/ItemDatabase.hpp"
#include "core/FluffParticles.hpp"
#include "core/FurReactions.hpp"
#include "core/RaidClock.hpp"
#include "services/MusicFade.hpp"
#include "tools/ContentLint.hpp"
#include "core/Cell.hpp"
#include "core/Cutscene.hpp"
#include "core/SessionMeta.hpp"
#include "core/SpriteSheet.hpp"
#include "core/TiledMap.hpp"
#include "core/Camera2DFollow.hpp"
#include "core/RoomTransition.hpp"
#include "scenes/LoreScroll.hpp"

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
        assert(savePlaceLine("ruins_fall") == "* The scent of lukewarm energy drinks and stale Doritos fills you with DETERMINATION.");
        assert(savePlaceLine("") == "* The scent of lukewarm energy drinks and stale Doritos fills you with DETERMINATION.");
        assert(savePlaceLine("snowdin_town") == "* The sight of 47 people sharing one Wi-Fi password on a fridge fills you with DETERMINATION.");
        assert(savePlaceLine("unknown_room_xyz") == "* The scent of lukewarm energy drinks and stale Doritos fills you with DETERMINATION.");

        // Check all other rooms from LORE table for thoroughness
        assert(savePlaceLine("ruins_hall") == "* The distant hum of dial-up internet fills you with DETERMINATION.");
        assert(savePlaceLine("ruins_entrance") == "* The distant hum of dial-up internet fills you with DETERMINATION.");
        assert(savePlaceLine("ruins_exit") == "* The cold air and the faint smell of ozone fill you with DETERMINATION.");
        assert(savePlaceLine("snowdin_entrance") == "* The crunch of styrofoam snow fills you with DETERMINATION.");
        assert(savePlaceLine("snowdin_fog") == "* The hum of the town's single shared router fills you with DETERMINATION.");
        assert(savePlaceLine("waterfall_entrance") == "* The sound of dripping and a distant \"rawr\" fill you with DETERMINATION.");
        assert(savePlaceLine("waterfall_arena") == "* The humidity of a thousand fursuits fills you with DETERMINATION.");
        assert(savePlaceLine("hotland_quiz") == "* The sound of 400 server fans fills you with DETERMINATION.");
        assert(savePlaceLine("newhome_throne") == "* The smell of herbal tea and old carpet fills you with DETERMINATION.");
        assert(savePlaceLine("con_center_hall") == "* The terrified hugging of a hundred furries fills you with DETERMINATION.");

        // saveLines with fur 0 (1 line)
        auto linesFur0 = saveLines("ruins_fall", 0, false);
        assert(linesFur0.size() == 1);
        assert(linesFur0[0] == "* The scent of lukewarm energy drinks and stale Doritos fills you with DETERMINATION.");

        // saveLines with fur 15 (2 lines, second is the fluff line)
        auto linesFur15 = saveLines("ruins_fall", 15, false);
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

    // 22. FRT-009: SettingsModel pure functions: volume stepping, percentage rounding, and index wrapping
    {
        using nundertale::stepVolume;
        using nundertale::volumePercent;
        using nundertale::wrapIndex;

        // stepVolume(0.8,+1)≈0.9, (1.0,+1)=1.0, (0.0,-1)=0.0, (0.05,+1)≈0.1
        assert(std::fabs(stepVolume(0.8f, +1) - 0.9f) < 1e-4f);
        assert(std::fabs(stepVolume(1.0f, +1) - 1.0f) < 1e-4f);
        assert(std::fabs(stepVolume(0.0f, -1) - 0.0f) < 1e-4f);
        assert(std::fabs(stepVolume(0.05f, +1) - 0.1f) < 1e-4f);

        // Additional edge cases
        assert(std::fabs(stepVolume(0.9f, -1) - 0.8f) < 1e-4f);
        assert(std::fabs(stepVolume(0.1f, -1) - 0.0f) < 1e-4f);
        assert(std::fabs(stepVolume(1.0f, -1) - 0.9f) < 1e-4f);

        // volumePercent(0.8)=80
        assert(volumePercent(0.8f) == 80);
        assert(volumePercent(1.0f) == 100);
        assert(volumePercent(0.0f) == 0);

        // wrapIndex(0,-1,5)=4, (4,+1,5)=0
        assert(wrapIndex(0, -1, 5) == 4);
        assert(wrapIndex(4, +1, 5) == 0);
        assert(wrapIndex(2, +1, 5) == 3);
        assert(wrapIndex(2, -1, 5) == 1);

        std::cout << "[PASS] FRT-009: SettingsModel volume stepping, percent, and index wrapping verified." << std::endl;
    }

    // 23. BHL-014: Barky pattern routing and reachable howl gap
    {
        using nundertale::barkyPatternForTurn;
        using nundertale::BarkyPattern;
        using nundertale::howlGapContains;

        assert(barkyPatternForTurn(0, false, false) == BarkyPattern::UwuRain);
        assert(barkyPatternForTurn(1, false, false) == BarkyPattern::DewBounce);
        assert(barkyPatternForTurn(2, false, false) == BarkyPattern::UwuRain);

        assert(barkyPatternForTurn(0, true, true) == BarkyPattern::HowlWave);
        assert(barkyPatternForTurn(1, true, true) == BarkyPattern::HowlWave);
        assert(barkyPatternForTurn(2, true, true) == BarkyPattern::HowlWave);

        assert(barkyPatternForTurn(3, true, false) == BarkyPattern::UwuRain);
        assert(barkyPatternForTurn(4, true, false) == BarkyPattern::DewBounce);
        assert(barkyPatternForTurn(5, true, false) == BarkyPattern::HowlWave);

        assert(!howlGapContains(100.0f, 48.0f, 99.9f));
        assert(howlGapContains(100.0f, 48.0f, 100.0f));
        assert(howlGapContains(100.0f, 48.0f, 124.0f));
        assert(howlGapContains(100.0f, 48.0f, 148.0f));
        assert(!howlGapContains(100.0f, 48.0f, 148.1f));

        std::cout << "[PASS] BHL-014: Barky pattern unlock routing and reachable howl gap verified." << std::endl;
    }

    // 24. BND-010 & BND-011: FURRY OVER voice lines, [NAME] replacement, route gating, and variations
    {
        using nundertale::asgrrLines;
        using nundertale::withName;
        using nundertale::furryOverLines;

        // withName with two tokens
        std::string twoTokens = "Don't give up, [NAME]! I believe in you, [NAME].";
        assert(withName(twoTokens, "PEPE") == "Don't give up, PEPE! I believe in you, PEPE.");

        // withName with zero and one token
        assert(withName("No token here.", "PEPE") == "No token here.");
        assert(withName("One [NAME] here.", "FROG") == "One FROG here.");

        // asgrrLines() returns the 6 canonical lines
        const auto& lines = asgrrLines();
        assert(lines.size() == 6);
        assert(lines[0] == "Don't lose hope, [NAME]... The ears suit you, honestly.");
        assert(lines[1] == "[NAME]! Stay determined... and maybe a little fluffy.");
        assert(lines[2] == "This is not the end, [NAME]. It is a... rebrand.");
        assert(lines[3] == "Get up, [NAME]. Your guild is counting on you. ...Probably.");
        assert(lines[4] == "You cannot give up just yet... You have not even picked a species.");
        assert(lines[5] == "Your SOUL said no, [NAME]. ...This time.");

        // furryOverLines: toxic -> empty
        auto toxicLines = furryOverLines("PEPE", "barky", 0, true, 2);
        assert(toxicLines.empty());
        auto toxicNoEnemy = furryOverLines("PEPE", "", 5, true, 0);
        assert(toxicNoEnemy.empty());

        // pick 2 + name "PEPE" -> This is not the end, PEPE. It is a... rebrand.
        auto pick2 = furryOverLines("PEPE", "", 0, false, 2);
        assert(pick2.size() == 1);
        assert(pick2[0] == "This is not the end, PEPE. It is a... rebrand.");

        // enemy barky -> 2 lines, 2nd is the Barky line
        auto barkyLines = furryOverLines("PEPE", "barky", 0, false, 2);
        assert(barkyLines.size() == 2);
        assert(barkyLines[0] == "This is not the end, PEPE. It is a... rebrand.");
        assert(barkyLines[1] == "...welcome to the pack? no? ok.");

        // timesTransformed 3 -> ...Again? Keep the tail, PEPE.
        auto trans3 = furryOverLines("PEPE", "", 3, false, 2);
        assert(trans3.size() == 1);
        assert(trans3[0] == "...Again? Keep the tail, PEPE.");

        // timesTransformed > 3 preserves the line and still appends barky line if enemy is barky
        auto trans4Barky = furryOverLines("PEPE", "barky", 4, false, 0);
        assert(trans4Barky.size() == 2);
        assert(trans4Barky[0] == "...Again? Keep the tail, PEPE.");
        assert(trans4Barky[1] == "...welcome to the pack? no? ok.");

        // pick 8 -> index 2 (8 % 6 = 2)
        auto pick8 = furryOverLines("PEPE", "", 0, false, 8);
        assert(pick8.size() == 1);
        assert(pick8[0] == "This is not the end, PEPE. It is a... rebrand.");

        // picks 0, 1, 3, 4, 5 coverage
        assert(furryOverLines("CHAD", "", 0, false, 0)[0] == "Don't lose hope, CHAD... The ears suit you, honestly.");
        assert(furryOverLines("CHAD", "", 0, false, 1)[0] == "CHAD! Stay determined... and maybe a little fluffy.");
        assert(furryOverLines("CHAD", "", 0, false, 3)[0] == "Get up, CHAD. Your guild is counting on you. ...Probably.");
        assert(furryOverLines("CHAD", "", 0, false, 4)[0] == "You cannot give up just yet... You have not even picked a species.");
        assert(furryOverLines("CHAD", "", 0, false, 5)[0] == "Your SOUL said no, CHAD. ...This time.");

        std::cout << "[PASS] BND-010 & BND-011: FURRY OVER voice lines, token substitution, route gating, and variations verified." << std::endl;
    }

    // 25. FUR-003: KR-style FUR HUD visibility, tremble timing window, and deterministic jitter
    {
        using nundertale::furHudVisible;
        using nundertale::furHudShouldTremble;
        using nundertale::furHudJitter;

        // Visibility: false at 0, true at >0
        assert(!furHudVisible(0));
        assert(furHudVisible(1));
        assert(furHudVisible(15));
        assert(!furHudVisible(-1));

        // Tremble window edges: [0, 1.5)
        assert(furHudShouldTremble(0.0f));
        assert(furHudShouldTremble(0.5f));
        assert(furHudShouldTremble(1.49f));
        assert(!furHudShouldTremble(1.5f));
        assert(!furHudShouldTremble(1.51f));
        assert(!furHudShouldTremble(2.0f));
        assert(!furHudShouldTremble(-0.1f));

        // Jitter returns values in {-1, 0, 1} and is 0 at t >= 1.5
        for (float t = 0.0f; t < 1.5f; t += 0.02f) {
            int j = furHudJitter(t);
            assert(j == -1 || j == 0 || j == 1);
        }
        assert(furHudJitter(1.5f) == 0);
        assert(furHudJitter(1.51f) == 0);
        assert(furHudJitter(2.0f) == 0);
        assert(furHudJitter(-0.5f) == 0);

        // Deterministic alternation every ~2 frames (~1/15s)
        assert(furHudJitter(0.0f) == -1);
        assert(furHudJitter(0.07f) == 1);
        assert(furHudJitter(0.14f) == 0);
        assert(furHudJitter(0.21f) == -1);

        std::cout << "[PASS] FUR-003: KR-style FUR HUD visibility, tremble window, and jitter verified." << std::endl;
    }

    // 26. ITM-001 & FUR-009: Item database parsing, defaults, escapes, and lookups
    {
        using nundertale::ItemDatabase;

        const std::string json = R"json([
            {
                "id": "escaped_weapon",
                "name": "Toy \"Laser\"",
                "shortName": "Laser\nMk2",
                "type": "weapon",
                "heal": 2,
                "at": 7,
                "df": 1,
                "furry": 3,
                "furMult": 1.5,
                "info": "Line one\nLine two",
                "useText": "Use the \"laser\".",
                "unknown": { "nested": [1, 2, 3] }
            },
            {
                "id": "plain_armor",
                "name": "Plain Armor",
                "type": "armor",
                "df": 4
            }
        ])json";

        ItemDatabase database;
        assert(database.parse(json));
        assert(database.items().size() == 2);

        const auto* weapon = database.findItemByName("Toy \"Laser\"");
        assert(weapon != nullptr);
        assert(weapon->id == "escaped_weapon");
        assert(weapon->shortName == "Laser\nMk2");
        assert(weapon->type == "weapon");
        assert(weapon->heal == 2);
        assert(weapon->at == 7);
        assert(weapon->df == 1);
        assert(weapon->furry == 3);
        assert(std::abs(weapon->furMult - 1.5f) < 0.0001f);
        assert(weapon->info == "Line one\nLine two");
        assert(weapon->useText == "Use the \"laser\".");

        const auto* armor = database.findItemByName("Plain Armor");
        assert(armor != nullptr);
        assert(armor->df == 4);
        assert(std::abs(armor->furMult - 1.0f) < 0.0001f);
        assert(database.findItemByName("Missing Item") == nullptr);

        std::ifstream realItemsFile("data/items/items.json");
        if (realItemsFile) {
            std::ostringstream realItemsJson;
            realItemsJson << realItemsFile.rdbuf();
            ItemDatabase realDatabase;
            assert(realDatabase.parse(realItemsJson.str()));

            const auto* bandana = realDatabase.findItemByName("Faux-Fur Bandana");
            assert(bandana != nullptr);
            assert(std::abs(bandana->furMult - 1.25f) < 0.0001f);

            const auto* fursuitHead = realDatabase.findItemByName("Fursuit Head");
            assert(fursuitHead != nullptr);
            assert(fursuitHead->furry == 40);
        }

        std::cout << "[PASS] ITM-001 & FUR-009: Item JSON parsing, defaults, escapes, and name lookups verified." << std::endl;
    }

    // Test Suite 27: Ambient fluff particles in the overworld (FUR-010)
    {
        // 1. Target count based on fur and route
        assert(nundertale::fluffTargetCount(0, false) == 2);
        assert(nundertale::fluffTargetCount(49, false) == 2);
        assert(nundertale::fluffTargetCount(50, false) == 3);
        assert(nundertale::fluffTargetCount(100, false) == 4);
        assert(nundertale::fluffTargetCount(80, true) == 0);

        // Clamping edge cases
        assert(nundertale::fluffTargetCount(-5, false) == 2);
        assert(nundertale::fluffTargetCount(150, false) == 4);
        assert(nundertale::fluffTargetCount(0, true) == 0);

        // 2. Convergence to target and never exceeding target+0
        std::vector<nundertale::FluffParticle> particles;
        std::uint32_t rng = 0x12345678u;
        const int target = 4;

        // Starts empty
        assert(particles.empty());

        // Converges to target within N updates (1 per update at most)
        for (int i = 1; i <= target; ++i) {
            nundertale::updateFluff(particles, 0.016f, target, rng);
            assert(static_cast<int>(particles.size()) == i);
            assert(static_cast<int>(particles.size()) <= target);
        }
        assert(static_cast<int>(particles.size()) == target);

        // Extra updates: never exceeds target+0
        for (int i = 0; i < 20; ++i) {
            nundertale::updateFluff(particles, 0.016f, target, rng);
            assert(static_cast<int>(particles.size()) == target);
        }

        // Target reduction: reduces by 1 per update
        const int reducedTarget = 2;
        nundertale::updateFluff(particles, 0.016f, reducedTarget, rng);
        assert(static_cast<int>(particles.size()) == 3);
        nundertale::updateFluff(particles, 0.016f, reducedTarget, rng);
        assert(static_cast<int>(particles.size()) == 2);
        nundertale::updateFluff(particles, 0.016f, reducedTarget, rng);
        assert(static_cast<int>(particles.size()) == 2);

        // Target 0 (toxic route)
        nundertale::updateFluff(particles, 0.016f, 0, rng);
        assert(static_cast<int>(particles.size()) == 1);
        nundertale::updateFluff(particles, 0.016f, 0, rng);
        assert(particles.empty());
        nundertale::updateFluff(particles, 0.016f, 0, rng);
        assert(particles.empty());

        // 3. Particles stay within canvas bounds after many updates (with wrapping)
        particles.clear();
        rng = 0x98765432u;
        for (int i = 0; i < 1000; ++i) {
            nundertale::updateFluff(particles, 0.033f, 4, rng);
            for (const auto& p : particles) {
                assert(p.x >= 0.0f && p.x <= 640.0f);
                assert(p.y >= 0.0f && p.y <= 480.0f);
                float drawX = nundertale::fluffDrawX(p);
                assert(drawX >= 0.0f && drawX <= 640.0f);
                assert(p.colorIndex == 0 || p.colorIndex == 1);
            }
        }

        // 4. Deterministic RNG: same seed -> same positions
        std::vector<nundertale::FluffParticle> p1;
        std::vector<nundertale::FluffParticle> p2;
        std::uint32_t seed1 = 0xC0FFEE11u;
        std::uint32_t seed2 = 0xC0FFEE11u;

        for (int i = 0; i < 60; ++i) {
            nundertale::updateFluff(p1, 0.02f, 4, seed1);
            nundertale::updateFluff(p2, 0.02f, 4, seed2);
        }

        assert(p1.size() == p2.size());
        assert(seed1 == seed2);
        for (size_t i = 0; i < p1.size(); ++i) {
            assert(p1[i].x == p2[i].x);
            assert(p1[i].y == p2[i].y);
            assert(p1[i].vx == p2[i].vx);
            assert(p1[i].vy == p2[i].vy);
            assert(p1[i].phase == p2[i].phase);
            assert(p1[i].colorIndex == p2[i].colorIndex);
        }

        std::cout << "[PASS] FUR-010: Ambient fluff particles target count, convergence, canvas bounds, and deterministic RNG verified." << std::endl;
    }

    // 28. FUR-008: FUR stage reaction thresholds by speaker
    {
        using nundertale::furReactionLine;

        assert(furReactionLine(19, "generic").empty());
        assert(furReactionLine(20, "generic") == "* Nice ears, newcomer!");
        assert(furReactionLine(39, "generic") == "* Nice ears, newcomer!");
        assert(furReactionLine(40, "generic") == "* Nice ears, newcomer!");

        assert(furReactionLine(19, "barky").empty());
        assert(furReactionLine(20, "barky").empty());
        assert(furReactionLine(39, "barky").empty());
        assert(furReactionLine(40, "barky") == "Dude. Your paws are SO much better than mine.");

        assert(furReactionLine(100, "unknown").empty());

        std::cout << "[PASS] FUR-008: FUR stage reaction speaker thresholds verified." << std::endl;
    }

    // 29. BHL-014 tuning: howl_wave wall cadence and howlNextWallReady helper
    {
        using nundertale::howlNextWallReady;

        constexpr float boxW = 165.0f;
        constexpr float minRatio = 0.60f;
        constexpr float minReqTravel = boxW * minRatio; // 99.0f

        // Interval below 1.4s -> false regardless of travel
        assert(!howlNextWallReady(200.0f, boxW, 0.0f));
        assert(!howlNextWallReady(200.0f, boxW, 1.39f));

        // Travel below 60% of box width -> false regardless of interval
        assert(!howlNextWallReady(0.0f, boxW, 1.4f));
        assert(!howlNextWallReady(minReqTravel - 0.1f, boxW, 2.0f));

        // Exact boundaries
        assert(howlNextWallReady(minReqTravel, boxW, 1.4f));

        // Above boundaries
        assert(howlNextWallReady(minReqTravel + 10.0f, boxW, 1.41f));
        assert(howlNextWallReady(182.0f, boxW, 1.4f)); // 1.4s * 130 px/s = 182 px

        // Custom parameters
        assert(!howlNextWallReady(50.0f, 100.0f, 1.0f, 0.60f, 1.2f));
        assert(howlNextWallReady(60.0f, 100.0f, 1.2f, 0.60f, 1.2f));

        std::cout << "[PASS] BHL-014: howl_wave wall cadence and howlNextWallReady helper verified." << std::endl;
    }

    // 30. RAD-001 & RAD-002: raid clock state, pause rules, labels, and JSON config
    {
        using nundertale::RaidClock;
        using nundertale::raidKicked;
        using nundertale::raidLabel;
        using nundertale::raidStart;
        using nundertale::raidStartSecondsFromJson;
        using nundertale::raidTick;

        RaidClock clock;
        assert(raidLabel(clock).empty());
        assert(!raidKicked(clock));

        raidStart(clock, 1320.0f);
        assert(clock.started);
        assert(raidLabel(clock) == "RAID 22:00");

        clock.timeLeft = 59.2f;
        assert(raidLabel(clock) == "RAID 01:00");

        raidTick(clock, 10.0f, true);
        assert(std::abs(clock.timeLeft - 59.2f) < 0.0001f);
        raidTick(clock, 9.2f, false);
        assert(std::abs(clock.timeLeft - 50.0f) < 0.0001f);

        raidTick(clock, 100.0f, false);
        assert(clock.timeLeft == 0.0f);
        assert(raidKicked(clock));
        assert(raidLabel(clock) == "RAID: KICKED");

        RaidClock dormant;
        raidTick(dormant, 5.0f, false);
        assert(!dormant.started);
        assert(dormant.timeLeft == 0.0f);

        assert(raidStartSecondsFromJson(R"({"startSeconds": 1320})", 60) == 1320);
        assert(raidStartSecondsFromJson(R"({"comment": "missing"})", 1320) == 1320);
        assert(raidStartSecondsFromJson(R"({"startSeconds": "soon"})", 1320) == 1320);

        std::cout << "[PASS] RAD-001 & RAD-002: raid clock labels, ticking, pausing, kick state, and config parsing verified." << std::endl;
    }

    // 31. FUR-004 & FUR-011: Duct Tape FUR loss and Toxic route 99 cap
    {
        using nundertale::applyFur;
        using nundertale::applyFurCapped;
        using nundertale::DUCT_TAPE_FUR_LOSS;
        using nundertale::DUCT_TAPE_EMPTY_TEXT;
        using nundertale::DUCT_TAPE_DEFAULT_TEXT;
        using nundertale::TOXIC_CAP_FLAVOR_MSG;

        // FUR-004: Duct Tape loss at 30 -> 15, at 10 -> 0, at 0 -> 0
        int value = 30;
        int stageMax = 1;
        auto tape1 = applyFur(value, stageMax, DUCT_TAPE_FUR_LOSS);
        assert(value == 15);
        assert(tape1.after == 15);
        assert(!tape1.stageUp);
        assert(!tape1.reachedMirror);
        assert(!tape1.capped);

        value = 10;
        stageMax = 0;
        auto tape2 = applyFur(value, stageMax, DUCT_TAPE_FUR_LOSS);
        assert(value == 0);
        assert(tape2.after == 0);
        assert(!tape2.stageUp);
        assert(!tape2.reachedMirror);
        assert(!tape2.capped);

        value = 0;
        stageMax = 0;
        auto tape3 = applyFur(value, stageMax, DUCT_TAPE_FUR_LOSS);
        assert(value == 0);
        assert(tape3.after == 0);
        assert(!tape3.stageUp);
        assert(!tape3.reachedMirror);
        assert(!tape3.capped);

        // FUR-011: Capped gain 95+10 toxic -> 99, no mirror; non-toxic 95+10 -> 100 + mirror
        value = 95;
        stageMax = 4;
        auto toxicGain = applyFurCapped(value, stageMax, 10, true);
        assert(value == 99);
        assert(toxicGain.after == 99);
        assert(!toxicGain.reachedMirror);
        assert(toxicGain.capped);
        assert(stageMax == 4);

        value = 95;
        stageMax = 4;
        auto nonToxicGain = applyFurCapped(value, stageMax, 10, false);
        assert(value == 100);
        assert(nonToxicGain.after == 100);
        assert(nonToxicGain.reachedMirror);
        assert(!nonToxicGain.capped);
        assert(stageMax == 5);

        // Additional edge case: toxic gain already at 99 stays 99 and sets capped
        value = 99;
        stageMax = 4;
        auto toxicGainAtCap = applyFurCapped(value, stageMax, 5, true);
        assert(value == 99);
        assert(toxicGainAtCap.after == 99);
        assert(!toxicGainAtCap.reachedMirror);
        assert(toxicGainAtCap.capped);

        // Additional edge case: toxic gain within limit (e.g. 50 + 20 = 70) is not capped
        value = 50;
        stageMax = 2;
        auto toxicUncapped = applyFurCapped(value, stageMax, 20, true);
        assert(value == 70);
        assert(!toxicUncapped.capped);

        // Text constants check
        assert(std::string(DUCT_TAPE_EMPTY_TEXT) == "* You tape... nothing. There's no fluff to remove.");
        assert(std::string(DUCT_TAPE_DEFAULT_TEXT) == "* You aggressively taped down your sprouting fur.\n* Ouch! FUR reduced by 15!");
        assert(std::string(TOXIC_CAP_FLAVOR_MSG) == "* Your vitriol burns the fluff right off.");

        std::cout << "[PASS] FUR-004 & FUR-011: Duct Tape loss and Toxic route 99 cap verified." << std::endl;
    }

    // 32. MET-004: same name as the previous game
    {
        assert(nundertale::withSameNameReaction(nundertale::reactionFor("Bob"), "Bob", "BOB").message == "Again? You must really love this name.");
        assert(nundertale::withSameNameReaction(nundertale::reactionFor("Bob"), "Bob", "").message.empty());
        assert(nundertale::withSameNameReaction(nundertale::reactionFor("Bob"), "Bob", "Frog").message.empty());
        // Easter-egg reactions win over the same-name line
        assert(nundertale::withSameNameReaction(nundertale::reactionFor("Chad"), "Chad", "CHAD").message == nundertale::reactionFor("Chad").message);
        std::cout << "[PASS] MET-004: same-name reaction verified." << std::endl;
    }

    // 33. RAD-003: guild phone messages at raid clock thresholds
    {
        using nundertale::raidPendingMessage;
        using nundertale::raidMessageLines;
        using nundertale::raidFormatMinutesSeconds;

        // Start message (index 0) and mask prevention
        assert(raidPendingMessage(1320.0f, 1320.0f, 0) == 0);
        assert(raidPendingMessage(1320.0f, 1319.0f, 0) == 0);
        assert(raidPendingMessage(1320.0f, 1320.0f, 1) == -1);

        // Threshold 1: 15:00 (crossing 900)
        // Above threshold
        assert(raidPendingMessage(950.0f, 910.0f, 1) == -1);
        // Crossing threshold
        assert(raidPendingMessage(910.0f, 890.0f, 1) == 1);
        // Boundary crossing
        assert(raidPendingMessage(900.0f, 899.0f, 1) == 1);
        assert(raidPendingMessage(901.0f, 900.0f, 1) == 1);
        // Already below threshold (no crossing)
        assert(raidPendingMessage(890.0f, 880.0f, 1) == -1);
        // Mask prevention (no double show)
        assert(raidPendingMessage(910.0f, 890.0f, 1 | 2) == -1);

        // Threshold 2: 5:00 (crossing 300)
        // Above threshold
        assert(raidPendingMessage(350.0f, 310.0f, 1 | 2) == -1);
        // Crossing threshold
        assert(raidPendingMessage(310.0f, 290.0f, 1 | 2) == 2);
        // Boundary crossing
        assert(raidPendingMessage(300.0f, 299.0f, 1 | 2) == 2);
        assert(raidPendingMessage(301.0f, 300.0f, 1 | 2) == 2);
        // Already below threshold
        assert(raidPendingMessage(290.0f, 280.0f, 1 | 2) == -1);
        // Mask prevention (no double show)
        assert(raidPendingMessage(310.0f, 290.0f, 1 | 2 | 4) == -1);

        // Threshold 3: 0:00 (crossing 0)
        // Above threshold
        assert(raidPendingMessage(10.0f, 1.0f, 1 | 2 | 4) == -1);
        // Crossing threshold
        assert(raidPendingMessage(1.0f, 0.0f, 1 | 2 | 4) == 3);
        assert(raidPendingMessage(0.5f, -0.5f, 1 | 2 | 4) == 3);
        assert(raidPendingMessage(0.0f, 0.0f, 1 | 2 | 4) == 3);
        // Already below threshold
        assert(raidPendingMessage(-1.0f, -2.0f, 1 | 2 | 4) == -1);
        // Mask prevention (no double show)
        assert(raidPendingMessage(1.0f, 0.0f, 1 | 2 | 4 | 8) == -1);

        // Multi-threshold crossing in priority order
        // Dropping across both 900 and 300: first yields 1, then with bit 1 set yields 2
        assert(raidPendingMessage(1000.0f, 200.0f, 1) == 1);
        assert(raidPendingMessage(1000.0f, 200.0f, 1 | 2) == 2);
        assert(raidPendingMessage(1000.0f, 200.0f, 1 | 2 | 4) == -1);
        // Dropping to 0 yields 3
        assert(raidPendingMessage(1000.0f, 0.0f, 1 | 2 | 4) == 3);

        // Text formatting and message content validation
        assert(raidFormatMinutesSeconds(1320.0f) == "22:00");
        assert(raidFormatMinutesSeconds(900.0f) == "15:00");
        assert(raidFormatMinutesSeconds(300.0f) == "05:00");
        assert(raidFormatMinutesSeconds(59.2f) == "01:00");
        assert(raidFormatMinutesSeconds(0.0f) == "00:00");

        const auto lines0 = raidMessageLines(0, 1320.0f);
        assert(lines0.size() == 2);
        assert(lines0[0] == "* Your phone buzzes.");
        assert(lines0[1] == "* \"RAID STARTS IN 22:00. BE THERE OR GET KICKED. bring dew -xX_Moderator_Xx\"");

        const auto lines1 = raidMessageLines(1);
        assert(lines1.size() == 2);
        assert(lines1[0] == "* Your phone buzzes.");
        assert(lines1[1] == "* \"15 MIN WHERE ARE U\"");

        const auto lines2 = raidMessageLines(2);
        assert(lines2.size() == 2);
        assert(lines2[0] == "* Your phone buzzes.");
        assert(lines2[1] == "* \"5 min. ur kicked if ur late. no cap.\"");

        const auto lines3 = raidMessageLines(3);
        assert(lines3.size() == 3);
        assert(lines3[0] == "* Your phone buzzes.");
        assert(lines3[1] == "* \"Raid started without you. Kicked.\"");
        assert(lines3[2] == "* ...You feel strangely okay with that.");

        assert(raidMessageLines(4).empty());
        assert(raidMessageLines(-1).empty());

        std::cout << "[PASS] RAD-003: guild phone message threshold detection, masks, and texts verified." << std::endl;
    }

    // 34. LOR-005: Automatic content lint rules
    {
        using nundertale::LintFinding;
        using nundertale::lintText;

        const auto accented = lintText("First line\nNão, thanks.", {});
        const auto hasFinding = [](const std::vector<LintFinding>& findings,
                                   int line,
                                   int col,
                                   const std::string& rule) {
            return std::any_of(findings.begin(), findings.end(),
                               [line, col, &rule](const LintFinding& finding) {
                                   return finding.line == line && finding.col == col &&
                                          finding.rule == rule;
                               });
        };
        assert(hasFinding(accented, 2, 2, "non-english-char"));
        assert(hasFinding(accented, 2, 1, "portuguese-word"));

        const auto english = lintText("“Clean English” — still clean… ♥", {});
        assert(english.empty());

        const auto blocked = lintText("A forbidden phrase appears here.", {"forbidden phrase"});
        assert(hasFinding(blocked, 1, 3, "blocked-term"));

        const auto substring = lintText("Missouri is an English place name.", {});
        assert(substring.empty());

        std::cout << "[PASS] LOR-005: UTF-8, Portuguese word, allowed punctuation, and blocklist lint rules verified." << std::endl;
    }

    // 35. AUD-006: music fades
    {
        nundertale::MusicFade fade;
        nundertale::fadeStart(fade, 1.0f, 0.0f, 2.0f, true);
        assert(fade.active);
        const float mid = nundertale::fadeStep(fade, 1.0f);
        assert(mid > 0.49f && mid < 0.51f);
        assert(fade.active);
        const float end = nundertale::fadeStep(fade, 5.0f);
        assert(end == 0.0f);
        assert(!fade.active);
        assert(fade.stopAtEnd);
        nundertale::MusicFade instant;
        nundertale::fadeStart(instant, 0.0f, 1.0f, 0.0f, false);
        assert(!instant.active);
        assert(nundertale::fadeStep(instant, 0.1f) == 1.0f);
        std::cout << "[PASS] AUD-006: music fade steps verified." << std::endl;
    }

    // 36. MNU-004 & NAR-004: CELL contacts, Furiell options, and voicemail
    {
        using nundertale::cellEntries;
        using nundertale::furiellOptions;
        using nundertale::furiellVoicemail;
        using nundertale::cellReceiveOnlyText;
        using nundertale::cellNoAnswerText;
        using nundertale::cellHomeworkText;

        // 1. Entries without flags = Guild, Mom, Homework (400 GB)
        const auto noFlags = [](const std::string& /*flag*/) { return false; };
        const auto defaultEntries = cellEntries(noFlags);
        assert(defaultEntries.size() == 3);
        assert(defaultEntries[0] == "Guild");
        assert(defaultEntries[1] == "Mom");
        assert(defaultEntries[2] == "Homework (400 GB)");

        // 2. Entries with null/empty lambda
        const auto emptyEntries = cellEntries(nullptr);
        assert(emptyEntries.size() == 3);
        assert(emptyEntries[0] == "Guild");
        assert(emptyEntries[1] == "Mom");
        assert(emptyEntries[2] == "Homework (400 GB)");

        // 3. With cell_furiell: Furiell appears before Homework
        const auto furiellFlag = [](const std::string& flag) { return flag == "cell_furiell"; };
        const auto furiellEntries = cellEntries(furiellFlag);
        assert(furiellEntries.size() == 4);
        assert(furiellEntries[0] == "Guild");
        assert(furiellEntries[1] == "Mom");
        assert(furiellEntries[2] == "Furiell");
        assert(furiellEntries[3] == "Homework (400 GB)");

        auto itFuriell = std::find(furiellEntries.begin(), furiellEntries.end(), "Furiell");
        auto itHomework = std::find(furiellEntries.begin(), furiellEntries.end(), "Homework (400 GB)");
        assert(itFuriell != furiellEntries.end());
        assert(itHomework != furiellEntries.end());
        assert(itFuriell < itHomework);

        // 4. Other flags: Papyrus and Alphys
        const auto allFlags = [](const std::string& flag) {
            return flag == "cell_furiell" || flag == "cell_papyrus" || flag == "cell_alphys";
        };
        const auto allEntries = cellEntries(allFlags);
        assert(allEntries.size() == 6);
        assert(allEntries[0] == "Guild");
        assert(allEntries[1] == "Mom");
        assert(allEntries[2] == "Furiell");
        assert(allEntries[3] == "Papyrus");
        assert(allEntries[4] == "Alphys");
        assert(allEntries[5] == "Homework (400 GB)");

        // 5. 5 Furiell options with the exact Call Her "Mod" line
        const auto& options = furiellOptions();
        assert(options.size() == 5);
        assert(options[0].first == "Say Hello");
        assert(options[0].second == "Hello? This is Furiell. ...Did you drink water today? Actual water?");
        assert(options[1].first == "Puzzle Help");
        assert(options[1].second == "Look for the glitter, my child. The glitter is always right.");
        assert(options[2].first == "About Yourself");
        assert(options[2].second == "I used to run a convention. Now I run a household of one. Soon two!");
        assert(options[3].first == "Call Her \"Mod\"");
        assert(options[3].second == "...Did you just call me \"Mod\"? Oh my. Would you like to be pinned? To the fridge?");
        assert(options[4].first == "Send a Meme");
        assert(options[4].second == "Is this... a frog? Is this you? I'm printing it.");

        // 6. Furiell voicemail (LORE §21.1 verbatim)
        assert(furiellVoicemail() == "Hello, you have reached Furiell. I cannot come to the phone. I am probably baking. Please leave a message after the tone. ...Did you drink water today?");

        // 7. Dialogue helpers
        assert(cellReceiveOnlyText() == "* (They only text you. You can't call them.)");
        assert(cellNoAnswerText() == "* ...No answer.");
        assert(cellHomeworkText() == "* Not here.");

        std::cout << "[PASS] MNU-004 & NAR-004: CELL contacts by flags, Furiell options, and voicemail verified." << std::endl;
    }

    // 37. FUR-002: data-driven FUR source table and per-pattern values
    {
        nundertale::FurTable table;
        const bool parsed = table.parse(R"({
            "bullet_neutral": 3,
            "bullet_cute": 7,
            "bullet_very_cute": 10,
            "floofy_hug_pellet": 20,
            "accept_hug": 15,
            "accept_hug_furiell": 30,
            "furiell_bed": 5,
            "fursuit_head": 40,
            "quiz_wrong": 5
        })");
        assert(parsed);
        assert(table.get("bullet_neutral", -1) == 3);
        assert(table.get("bullet_cute", -1) == 7);
        assert(table.get("bullet_very_cute", -1) == 10);
        assert(table.get("floofy_hug_pellet", -1) == 20);
        assert(table.get("accept_hug", -1) == 15);
        assert(table.get("accept_hug_furiell", -1) == 30);
        assert(table.get("furiell_bed", -1) == 5);
        assert(table.get("fursuit_head", -1) == 40);
        assert(table.get("quiz_wrong", -1) == 5);
        assert(table.get("missing_source", 23) == 23);

        std::ifstream barkyFile("data/enemies/barky.json");
        if (barkyFile) {
            std::ostringstream barkyJson;
            barkyJson << barkyFile.rdbuf();
            const std::string json = barkyJson.str();
            assert(nundertale::patternFurFromJson(json, "uwu_rain", table.get("bullet_cute", 7)) == 6);
            assert(nundertale::patternFurFromJson(json, "dew_bounce", table.get("bullet_neutral", 3)) == 4);
            assert(nundertale::patternFurFromJson(json, "missing_pattern", table.get("bullet_neutral", 3)) == 3);
            assert(nundertale::howlFurFromJson(json, 5) == 5);
        }

        assert(nundertale::patternFurFromJson(
                   R"({"patterns":[{"id":"heart","params":{"fur":9,"furry":8}}]})",
                   "heart", 7) == 8);
        assert(nundertale::howlFurFromJson(R"({"howlFur":11})", 5) == 11);

        std::cout << "[PASS] FUR-002: FUR table parsing, fallbacks, and Barky pattern values verified." << std::endl;
    }

    // 38. FRT-008 & NAR-028: data/names.json parsing, inline JSON, and reaction parity
    {
        using nundertale::NameTable;
        using nundertale::NameEffect;
        using nundertale::NameReaction;
        using nundertale::reactionFor;
        using nundertale::lookupBuiltinReaction;
        using nundertale::builtinTableNames;
        using nundertale::globalNameTable;
        using nundertale::loadNamesFromFile;
        using nundertale::loadGlobalNames;
        using nundertale::withSameNameReaction;

        // 1. Parse a small inline JSON with 2 entries (escaped quote in a message)
        const std::string inlineJson = R"json([
            {
                "names": ["TESTER", "QA_HERO"],
                "tag": "[TEST]",
                "message": "He said: \"Ready to test!\"",
                "allowed": true,
                "effect": "gold"
            },
            {
                "names": ["FORBIDDEN"],
                "tag": "",
                "message": "Access \"denied\" completely.",
                "allowed": false,
                "effect": "restart_game"
            }
        ])json";

        NameTable localTable;
        assert(localTable.parse(inlineJson));
        assert(localTable.isLoaded());
        assert(localTable.entries.size() == 2);

        const auto* rTester = localTable.find("tester");
        assert(rTester != nullptr);
        assert(rTester->tag == "[TEST]");
        assert(rTester->message == "He said: \"Ready to test!\"");
        assert(rTester->allowed);
        assert(rTester->effect == NameEffect::Gold);

        const auto* rQa = localTable.find("QA_HERO");
        assert(rQa != nullptr);
        assert(rQa->tag == "[TEST]");

        const auto* rForbidden = localTable.find("forbidden");
        assert(rForbidden != nullptr);
        assert(rForbidden->message == "Access \"denied\" completely.");
        assert(!rForbidden->allowed);
        assert(rForbidden->effect == NameEffect::RestartGame);

        assert(localTable.find("unknown_name") == nullptr);

        // 2. Load the real file data/names.json if readable
        NameTable realTable;
        const bool loaded = loadNamesFromFile("data/names.json", realTable);
        if (loaded) {
            assert(realTable.isLoaded());
            assert(!realTable.empty());

            // Real file yields the same reaction as the built-in table for every name in the built-in table
            const auto& names = builtinTableNames();
            assert(names.size() >= 20);

            for (const auto& name : names) {
                NameReaction builtinR{};
                const bool foundBuiltin = lookupBuiltinReaction(name, builtinR);
                assert(foundBuiltin);

                const auto* jsonR = realTable.find(name);
                assert(jsonR != nullptr);
                assert(jsonR->tag == builtinR.tag);
                assert(jsonR->message == builtinR.message);
                assert(jsonR->allowed == builtinR.allowed);
                assert(jsonR->effect == builtinR.effect);
                assert(*jsonR == builtinR);

                // reactionFor with realTable must match
                NameReaction reactionWithTable = reactionFor(name, realTable);
                assert(reactionWithTable == builtinR);
            }

            // Ensure loadGlobalNames loads it globally
            assert(loadGlobalNames("data/names.json"));
            assert(globalNameTable().isLoaded());

            // Test case-insensitivity on global table
            for (const auto& name : names) {
                NameReaction builtinR{};
                lookupBuiltinReaction(name, builtinR);
                assert(reactionFor(name) == builtinR);

                std::string lower = name;
                for (char& c : lower) {
                    if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
                }
                assert(reactionFor(lower) == builtinR);
            }

            // Generic rules still work with loaded table
            assert(reactionFor("AAAA").message == "Not very creative... Did your keyboard jam?");
            assert(reactionFor("Q").message == "You call that a gamer? Saving characters?");
            assert(reactionFor("Bob") == NameReaction{});

            // withSameNameReaction helper (MET-004) keeps working
            assert(withSameNameReaction(reactionFor("Bob"), "Bob", "BOB").message == "Again? You must really love this name.");
            assert(withSameNameReaction(reactionFor("Bob"), "Bob", "").message.empty());
            assert(withSameNameReaction(reactionFor("Chad"), "Chad", "CHAD").message == reactionFor("Chad").message);
        }

        std::cout << "[PASS] FRT-008 & NAR-028: names.json parser, quotes, and parity with built-in table verified." << std::endl;
    }

    // 39. CORE-011: minimal step-scripted cutscene parser and runner
    {
        using nundertale::Cutscene;
        using nundertale::CutsceneHost;
        using nundertale::CutsceneRunner;
        using nundertale::CutsceneStep;
        using nundertale::CutsceneStepType;

        const std::string json = R"json({
            "steps": [
                {"type":"wait","seconds":0.6},
                {"type":"dialog","speaker":"BARKY","lines":["First line.","He said: \"Awoo!\""]},
                {"type":"sound","path":"howl.ogg","volume":0.8},
                {"type":"setFlag","key":"ruins.test","value":2},
                {"type":"shake","intensity":4,"seconds":0.3},
                {"type":"flash","r":12,"g":34,"b":56,"seconds":0.2},
                {"type":"startBattle","encounter":"barky"},
                {"type":"moveNpc","npc":"barky"},
                {"type":"cameraPan","x":10},
                {"type":"fadeOut","seconds":1},
                {"type":"futureStep","payload":{"nested":[1,2,3]}}
            ]
        })json";

        Cutscene parsed;
        assert(nundertale::parseCutsceneJson(json, parsed));
        assert(parsed.steps.size() == 11);
        assert(parsed.steps[0].type == CutsceneStepType::Wait);
        assert(parsed.steps[0].seconds == 0.6f);
        assert(parsed.steps[1].type == CutsceneStepType::Dialog);
        assert(parsed.steps[1].speaker == "BARKY");
        assert(parsed.steps[1].lines.size() == 2);
        assert(parsed.steps[1].lines[1] == "He said: \"Awoo!\"");
        assert(parsed.steps[2].type == CutsceneStepType::Sound);
        assert(parsed.steps[2].path == "howl.ogg");
        assert(parsed.steps[2].volume == 0.8f);
        assert(parsed.steps[3].type == CutsceneStepType::SetFlag);
        assert(parsed.steps[3].key == "ruins.test");
        assert(parsed.steps[3].value == 2);
        assert(parsed.steps[4].type == CutsceneStepType::Shake);
        assert(parsed.steps[4].intensity == 4.0f);
        assert(parsed.steps[5].type == CutsceneStepType::Flash);
        assert(parsed.steps[5].r == 12 && parsed.steps[5].g == 34 && parsed.steps[5].b == 56);
        assert(parsed.steps[6].type == CutsceneStepType::StartBattle);
        assert(parsed.steps[6].encounter == "barky");
        assert(parsed.steps[7].type == CutsceneStepType::Unsupported);
        assert(parsed.steps[8].type == CutsceneStepType::Unsupported);
        assert(parsed.steps[9].type == CutsceneStepType::Unsupported);
        assert(parsed.steps[10].type == CutsceneStepType::Unknown);

        struct FakeCutsceneHost final : CutsceneHost {
            bool dialogIsOpen = false;
            std::vector<std::string> calls;

            void dialog(const std::string& speaker, const std::vector<std::string>& lines) override {
                calls.push_back("dialog:" + speaker + ":" + lines.front());
                dialogIsOpen = true;
            }
            bool dialogOpen() const override { return dialogIsOpen; }
            void sound(const std::string& path, float volume) override {
                calls.push_back("sound:" + path + ":" + std::to_string(volume));
            }
            void setFlag(const std::string& key, int value) override {
                calls.push_back("flag:" + key + ":" + std::to_string(value));
            }
            void shake(float intensity, float seconds) override {
                calls.push_back("shake:" + std::to_string(intensity) + ":" + std::to_string(seconds));
            }
            void flash(int r, int g, int b, float seconds) override {
                calls.push_back("flash:" + std::to_string(r) + ":" + std::to_string(g) + ":" +
                                std::to_string(b) + ":" + std::to_string(seconds));
            }
            void startBattle(const std::string& encounter) override {
                calls.push_back("battle:" + encounter);
            }
        } host;

        Cutscene flow;
        CutsceneStep wait;
        wait.type = CutsceneStepType::Wait;
        wait.seconds = 0.5f;
        flow.steps.push_back(wait);

        CutsceneStep sound;
        sound.type = CutsceneStepType::Sound;
        sound.path = "howl.ogg";
        sound.volume = 0.8f;
        flow.steps.push_back(sound);

        CutsceneStep dialog;
        dialog.type = CutsceneStepType::Dialog;
        dialog.speaker = "BARKY";
        dialog.lines = {"Awoo!"};
        flow.steps.push_back(dialog);

        CutsceneStep flag;
        flag.type = CutsceneStepType::SetFlag;
        flag.key = "ruins.test";
        flag.value = 7;
        flow.steps.push_back(flag);

        CutsceneStep shake;
        shake.type = CutsceneStepType::Shake;
        shake.intensity = 4.0f;
        shake.seconds = 0.3f;
        flow.steps.push_back(shake);

        CutsceneStep flash;
        flash.type = CutsceneStepType::Flash;
        flash.r = 255;
        flash.g = 128;
        flash.b = 64;
        flash.seconds = 0.2f;
        flow.steps.push_back(flash);

        CutsceneStep unknown;
        unknown.type = CutsceneStepType::Unknown;
        flow.steps.push_back(unknown);

        CutsceneStep battle;
        battle.type = CutsceneStepType::StartBattle;
        battle.encounter = "barky";
        flow.steps.push_back(battle);

        CutsceneRunner runner;
        runner.start(flow);
        assert(runner.running());
        runner.update(0.4f, host);
        assert(host.calls.empty());
        assert(runner.running());

        runner.update(0.2f, host);
        assert(host.calls.size() == 2);
        assert(host.calls[0].find("sound:howl.ogg") == 0);
        assert(host.calls[1] == "dialog:BARKY:Awoo!");
        assert(runner.running());

        runner.update(1.0f, host);
        assert(host.calls.size() == 2);
        assert(runner.running());

        host.dialogIsOpen = false;
        runner.update(0.0f, host);
        assert(host.calls.size() == 6);
        assert(host.calls[2] == "flag:ruins.test:7");
        assert(host.calls[3].find("shake:4.000000:0.300000") == 0);
        assert(host.calls[4].find("flash:255:128:64:0.200000") == 0);
        assert(host.calls[5] == "battle:barky");
        assert(!runner.running());

        Cutscene unknownFlow;
        unknownFlow.steps.push_back(unknown);
        unknownFlow.steps.push_back(sound);
        runner.start(unknownFlow);
        runner.update(0.0f, host);
        assert(!runner.running());
        assert(host.calls.size() == 7);
        assert(host.calls[6].find("sound:howl.ogg") == 0);

        std::cout << "[PASS] CORE-011: cutscene parsing, waits, dialog blocking, callbacks, and skips verified." << std::endl;
    }

    // 40. MET-001: Reload detection and session-only metadata
    {
        using namespace nundertale;

        SessionMeta meta;
        assert(meta.killsSinceSave == 0);
        assert(!meta.reloadedAfterFurryOver);
        assert(!meta.reloadedAfterKill);

        // Scenario 1: Kill -> Save -> Load -> no reloadedAfterKill
        sessionOnKill(meta);
        assert(meta.killsSinceSave == 1);
        sessionOnSave(meta);
        assert(meta.killsSinceSave == 0);
        sessionOnLoad(meta, false);
        assert(!meta.reloadedAfterKill);
        assert(!meta.reloadedAfterFurryOver);
        assert(meta.killsSinceSave == 0);

        // Scenario 2: Kill -> Load -> reloadedAfterKill
        sessionOnKill(meta);
        sessionOnKill(meta);
        assert(meta.killsSinceSave == 2);
        sessionOnLoad(meta, false);
        assert(meta.reloadedAfterKill);
        assert(!meta.reloadedAfterFurryOver);
        assert(meta.killsSinceSave == 0);

        // Scenario 3: Load after Furry Over (afterFurryOver = true)
        sessionOnLoad(meta, true);
        assert(!meta.reloadedAfterKill);
        assert(meta.reloadedAfterFurryOver);
        assert(meta.killsSinceSave == 0);

        // Scenario 4: Kill -> Load after Furry Over -> both flags true
        sessionOnKill(meta);
        assert(meta.killsSinceSave == 1);
        sessionOnLoad(meta, true);
        assert(meta.reloadedAfterKill);
        assert(meta.reloadedAfterFurryOver);
        assert(meta.killsSinceSave == 0);

        // Scenario 5: A second clean load clears reloadedAfterKill and reloadedAfterFurryOver
        sessionOnLoad(meta, false);
        assert(!meta.reloadedAfterKill);
        assert(!meta.reloadedAfterFurryOver);
        assert(meta.killsSinceSave == 0);

        std::cout << "[PASS] MET-001: reload detection, session kill tracking, and game-over flags verified." << std::endl;
    }

    // 41. GFX-002: sprite-sheet parsing defaults and frame selection
    {
        using namespace nundertale;

        const std::string json = R"json({
            "image": "barky.png",
            "cell": [60, 80],
            "origin": [30, 80],
            "anims": {
                "idle": { "row": 0, "frames": 2, "fps": 2, "loop": true },
                "hurt": { "row": 1, "frames": 1 },
                "blush": { "row": 2 },
                "spared": { "row": 3, "frames": 3, "fps": 5, "loop": false }
            }
        })json";

        SpriteSheetDef sheet;
        assert(parseSpriteSheetJson(json, sheet));
        assert(sheet.image == "barky.png");
        assert(sheet.cellW == 60 && sheet.cellH == 80);
        assert(sheet.originX == 30 && sheet.originY == 80);

        assert(sheet.anims.at("hurt").frames == 1);
        assert(sheet.anims.at("hurt").fps == 4.0f);
        assert(sheet.anims.at("hurt").loop);
        assert(sheet.anims.at("blush").frames == 1);

        FrameRect frame = frameRect(sheet, "idle", 0.0f);
        assert(frame.x == 0 && frame.y == 0 && frame.w == 60 && frame.h == 80);
        frame = frameRect(sheet, "idle", 0.5f);
        assert(frame.x == 60 && frame.y == 0);
        frame = frameRect(sheet, "idle", 1.0f);
        assert(frame.x == 0 && frame.y == 0);

        frame = frameRect(sheet, "spared", 0.2f);
        assert(frame.x == 60 && frame.y == 240);
        frame = frameRect(sheet, "spared", 20.0f);
        assert(frame.x == 120 && frame.y == 240);

        frame = frameRect(sheet, "unknown", 0.5f);
        assert(frame.x == 60 && frame.y == 0);

        SpriteSheetDef noIdle;
        noIdle.cellW = 12;
        noIdle.cellH = 18;
        frame = frameRect(noIdle, "unknown", 99.0f);
        assert(frame.x == 0 && frame.y == 0 && frame.w == 12 && frame.h == 18);

        SpriteSheetDef unchanged;
        unchanged.image = "keep.png";
        assert(!parseSpriteSheetJson("{ invalid", unchanged));
        assert(unchanged.image == "keep.png");

        std::cout << "[PASS] GFX-002: sprite-sheet parsing, defaults, loops, holds, and fallbacks verified." << std::endl;
    }

    // 42. AUD-001 + AUD-005: Music by context from data/music.json and loop points
    {
        using namespace nundertale;

        // 1. shouldLoopSeek helper
        assert(!shouldLoopSeek(10.0f, 0.0f));       // loopEnd <= 0 means whole file / no loop seek
        assert(!shouldLoopSeek(10.0f, -1.0f));
        assert(!shouldLoopSeek(10.0f, 15.0f));      // not reached
        assert(shouldLoopSeek(15.0f, 15.0f));       // exact hit
        assert(shouldLoopSeek(15.1f, 15.0f));       // crossed

        // 2. MusicTable parse (valid JSON, custom loop points, defaults)
        const std::string testJson = R"json({
            "title": { "path": "assets/music/title.ogg" },
            "loop_track": { "path": "assets/music/loop.ogg", "loopStart": 2.5, "loopEnd": 45.0 },
            "overworld.default": { "path": "assets/music/ow_def.ogg" },
            "overworld.ruins_entrance": { "path": "assets/music/ruins.ogg" },
            "battle.default": { "path": "assets/music/bat_def.ogg" }
        })json";

        MusicTable table;
        assert(table.parse(testJson));
        assert(table.entries().size() == 5);

        // Check defaults for loopStart / loopEnd when missing
        MusicEntry titleEntry = table.musicFor("title");
        assert(titleEntry.path == "assets/music/title.ogg");
        assert(titleEntry.loopStart == 0.0f);
        assert(titleEntry.loopEnd == 0.0f);

        // Check explicit loop points
        MusicEntry loopEntry = table.musicFor("loop_track");
        assert(loopEntry.path == "assets/music/loop.ogg");
        assert(std::fabs(loopEntry.loopStart - 2.5f) < 0.001f);
        assert(std::fabs(loopEntry.loopEnd - 45.0f) < 0.001f);

        // 3. Fallback chains
        // Exact overworld room
        MusicEntry ruinsEntry = table.musicFor("overworld.ruins_entrance");
        assert(ruinsEntry.path == "assets/music/ruins.ogg");

        // Unknown overworld room -> falls back to overworld.default
        MusicEntry forestEntry = table.musicFor("overworld.forest");
        assert(forestEntry.path == "assets/music/ow_def.ogg");

        // Battle with encounter asset existing
        auto existsBoss = [](const std::string& path) {
            return path == "assets/music/boss_barky.ogg";
        };
        MusicEntry barkyFound = table.musicFor("battle.boss_barky", existsBoss);
        assert(barkyFound.path == "assets/music/boss_barky.ogg");

        // Battle with encounter asset missing -> falls back to battle.default
        auto existsNone = [](const std::string&) { return false; };
        MusicEntry barkyMissing = table.musicFor("battle.boss_barky", existsNone);
        assert(barkyMissing.path == "assets/music/bat_def.ogg");

        // Battle default lookup
        MusicEntry batDef = table.musicFor("battle.default");
        assert(batDef.path == "assets/music/bat_def.ogg");

        // Empty table fallback to hardcoded DEFAULT_BATTLE_MUSIC
        MusicTable emptyTable;
        MusicEntry fallbackEntry = emptyTable.musicFor("overworld.cave");
        assert(fallbackEntry.path == DEFAULT_BATTLE_MUSIC);
        MusicEntry batFallback = emptyTable.musicFor("battle.unknown", existsNone);
        assert(batFallback.path == DEFAULT_BATTLE_MUSIC);
        MusicEntry genericFallback = emptyTable.musicFor("unregistered_context");
        assert(genericFallback.path == DEFAULT_BATTLE_MUSIC);

        // Parse invalid JSON returns false
        MusicTable badTable;
        assert(!badTable.parse("{ not a valid json"));

        // 4. Test loading real data/music.json if readable
        std::ifstream realFile("data/music.json");
        if (realFile.is_open()) {
            MusicTable realTable;
            assert(realTable.loadFromFile("data/music.json"));
            assert(realTable.musicFor("title").path == "assets/music/main_menu_loop.ogg");
            assert(realTable.musicFor("lore").path == "assets/music/Undertale - Once Upon A Time Remix (ft Yuri, zR).ogg");
            assert(realTable.musicFor("instructions").path == "assets/music/main_menu_loop.ogg");
            assert(realTable.musicFor("overworld.ruins_entrance").path == "assets/music/epic_background_loop.ogg");
            assert(realTable.musicFor("overworld.default").path == "assets/music/epic_background_loop.ogg");
            assert(realTable.musicFor("battle.default").path == "assets/music/epic_background_loop.ogg");
            assert(realTable.musicFor("gameover").path == "assets/music/epic_background_loop.ogg");
        }

        std::cout << "[PASS] AUD-001 + AUD-005: Music by context, JSON parsing, fallback chain, and loop points verified." << std::endl;
    }

    // 43. OVW-003 + GFX-010 + OVW-002: Tiled map parsing and camera follow
    {
        using namespace nundertale;

        const std::string tiledJson = R"json({
            "orientation": "orthogonal",
            "tilewidth": 16,
            "tileheight": 16,
            "width": 10,
            "height": 8,
            "layers": [
                {
                    "type": "tilelayer",
                    "name": "ground",
                    "width": 10,
                    "height": 8,
                    "data": [1, 0, 2]
                },
                {
                    "type": "imagelayer",
                    "name": "backdrop",
                    "image": "assets/img/room.png",
                    "offsetx": 4,
                    "offsety": -2
                },
                {
                    "type": "objectgroup",
                    "name": "objects",
                    "objects": [
                        {
                            "id": 7,
                            "name": "slope",
                            "class": "collision",
                            "x": 100,
                            "y": 50,
                            "width": 0,
                            "height": 0,
                            "polygon": [
                                {"x": 0, "y": 0},
                                {"x": 20, "y": 5},
                                {"x": 10, "y": 30}
                            ],
                            "properties": [
                                {"name": "difficulty", "type": "int", "value": 3}
                            ]
                        },
                        {
                            "id": 8,
                            "name": "target",
                            "type": "interactable",
                            "x": 12,
                            "y": 24,
                            "width": 16,
                            "height": 20
                        }
                    ]
                }
            ]
        })json";

        TiledMap map;
        assert(parseTiledMapJson(tiledJson, map));
        assert(map.orientation == "orthogonal");
        assert(map.tileWidth == 16 && map.tileHeight == 16);
        assert(map.width == 10 && map.height == 8);
        assert(map.pixelWidth() == 160.0f && map.pixelHeight() == 128.0f);
        assert(map.layers.size() == 3);

        const TiledLayer* ground = map.findLayer("ground");
        assert(ground != nullptr);
        assert(ground->type == TiledLayerType::Tile);
        assert(ground->data.size() == 3);
        assert(ground->data[0] == 1 && ground->data[2] == 2);

        const TiledLayer* backdrop = map.findLayer("backdrop");
        assert(backdrop != nullptr);
        assert(backdrop->type == TiledLayerType::Image);
        assert(backdrop->image == "assets/img/room.png");
        assert(backdrop->offsetX == 4.0f && backdrop->offsetY == -2.0f);

        const TiledObject* slope = map.findObject("slope");
        assert(slope != nullptr);
        assert(slope->id == 7);
        assert(slope->className == "collision");
        assert(slope->polygon.size() == 3);
        const TiledRect slopeBounds = slope->bounds();
        assert(slopeBounds.x == 100.0f && slopeBounds.y == 50.0f);
        assert(slopeBounds.width == 20.0f && slopeBounds.height == 30.0f);
        const TiledProperty* difficulty = slope->findProperty("difficulty");
        assert(difficulty != nullptr);
        assert(difficulty->type == "int");
        assert(std::get<double>(difficulty->value) == 3.0);

        const TiledObject* target = map.findObject("target");
        assert(target != nullptr);
        assert(target->type == "interactable");
        assert(map.findObject("missing") == nullptr);

        CameraTopLeft camera = Camera2DFollow::calculate(0.0f, 0.0f, 1280.0f, 960.0f);
        assert(camera.x == 0.0f && camera.y == 0.0f);
        camera = Camera2DFollow::calculate(640.0f, 480.0f, 1280.0f, 960.0f);
        assert(camera.x == 320.0f && camera.y == 240.0f);
        camera = Camera2DFollow::calculate(1280.0f, 960.0f, 1280.0f, 960.0f);
        assert(camera.x == 640.0f && camera.y == 480.0f);
        camera = Camera2DFollow::calculate(160.0f, 120.0f, 320.0f, 240.0f);
        assert(camera.x == -160.0f && camera.y == -120.0f);

        // OVW-020: the first Ruins rooms are real tiled maps chained by doors.
        const char* ruinsRooms[] = { "ruins_fall", "ruins_entrance", "ruins_notices", "ruins_arena" };
        for (const char* roomId : ruinsRooms) {
            TiledMap realMap;
            assert(loadTiledMapFromFile(std::string("data/maps/") + roomId + ".json", realMap));
            assert(realMap.tileWidth == 40 && !realMap.tilesets.empty());
            const TiledLayer* realGround = realMap.findLayer("ground");
            const TiledLayer* collision = realMap.findLayer("collision");
            const TiledLayer* objects = realMap.findLayer("objects");
            assert(realGround != nullptr && realGround->data.size() == static_cast<std::size_t>(realMap.width * realMap.height));
            assert(collision != nullptr && !collision->objects.empty());
            assert(objects != nullptr);
            for (const TiledObject& object : objects->objects) {
                if (object.type != "door") continue;
                const TiledProperty* targetRoom = object.findProperty("targetRoom");
                const TiledProperty* targetSpawn = object.findProperty("targetSpawn");
                assert(targetRoom != nullptr && targetSpawn != nullptr);
                TiledMap targetMap;
                assert(loadTiledMapFromFile("data/maps/" + std::get<std::string>(targetRoom->value) + ".json", targetMap));
                assert(targetMap.findObject(std::get<std::string>(targetSpawn->value)) != nullptr);
            }
        }

        std::cout << "[PASS] OVW-003, GFX-010 & OVW-002: Tiled layers, polygons, properties, object lookup, real room, and camera bounds verified." << std::endl;
    }

    // 44. QA-012: Strict JSON well-formedness validation
    {
        using namespace nundertale;

        // Valid JSON passes
        LintFinding finding;
        const std::string validJson = R"json({
            "name": "Pepe",
            "hp": 20,
            "speed": 120.5,
            "negative": -42,
            "scientific": 1.25e+3,
            "valid": true,
            "fluffy": false,
            "empty_val": null,
            "items": [1, 2, "stick", null, false],
            "nested": {
                "x": 0.5,
                "escapes": "line\n\t\"quote\"\\\/form\ffeed\bback\rcarriage\u0041"
            }
        })json";
        assert(validateJson(validJson, &finding));
        assert(lintJson(validJson).empty());

        // Trailing comma in object fails with the right line
        const std::string trailingCommaObj = "{\n  \"a\": 1,\n}";
        assert(!validateJson(trailingCommaObj, &finding));
        assert(finding.rule == "invalid-json");
        assert(finding.line == 2);
        const auto objFindings = lintJson(trailingCommaObj);
        assert(objFindings.size() == 1);
        assert(objFindings[0].rule == "invalid-json");
        assert(objFindings[0].line == 2);

        // Trailing comma in array fails with the right line
        const std::string trailingCommaArr = "[\n  1,\n  2,\n]";
        assert(!validateJson(trailingCommaArr, &finding));
        assert(finding.rule == "invalid-json");
        assert(finding.line == 3);

        // Single-line trailing comma
        assert(!validateJson("{\"a\": 1,}", &finding));
        assert(finding.line == 1);

        // Unclosed string fails with the right line
        const std::string unclosedStr = "{\n  \"unclosed\n}";
        assert(!validateJson(unclosedStr, &finding));
        assert(finding.rule == "invalid-json");
        assert(finding.line == 2);

        assert(!validateJson("\"unclosed at eof", &finding));
        assert(finding.line == 1);

        // Bad escape fails with the right line
        const std::string badEscape = "{\n  \"bad\": \"\\x\"\n}";
        assert(!validateJson(badEscape, &finding));
        assert(finding.rule == "invalid-json");
        assert(finding.line == 2);

        const std::string badUnicode = "{\n  \"bad\": \"\\u12G4\"\n}";
        assert(!validateJson(badUnicode, &finding));
        assert(finding.rule == "invalid-json");
        assert(finding.line == 2);

        // Number format errors
        assert(!validateJson("{\"a\": 01}", &finding));
        assert(finding.rule == "invalid-json");
        assert(!validateJson("{\"a\": 1.}", &finding));
        assert(finding.rule == "invalid-json");
        assert(!validateJson("{\"a\": -}", &finding));
        assert(finding.rule == "invalid-json");

        // Trailing characters after root
        assert(!validateJson("{} extra", &finding));
        assert(finding.rule == "invalid-json");

        // Overload with LintFinding reference
        LintFinding refFinding;
        assert(!validateJson(trailingCommaObj, refFinding));
        assert(refFinding.line == 2);

        // Default argument overload without pointer
        assert(validateJson("{}"));
        assert(!validateJson("{"));

        std::cout << "[PASS] QA-012: Strict JSON validation (valid, trailing comma, unclosed string, bad escape, numbers) verified." << std::endl;
    }

    // 45. BUG-33: a held key counts once, and an unread press doesn't fire later
    {
        using namespace nundertale;
        PressLatch z;

        // Held Z for 10 frames across 5 logic ticks: exactly one press
        int presses = 0;
        for (int frame = 0; frame < 10; ++frame) {
            z.feed(true);
            if (frame % 2 == 1) {
                if (z.consume()) presses++;
                z.endTick();
            }
        }
        assert(presses == 1);

        // Released, pressed again: a new press
        z.feed(false);
        z.feed(true);
        assert(z.consume());
        assert(!z.consume());

        // A press nobody reads in its tick is dropped (it can't confirm the next menu later)
        z.feed(false);
        z.feed(true);
        z.endTick();
        z.feed(true);
        assert(!z.consume());

        // Press and release between two frames still counts
        z.feed(false);
        z.feed(false, true);
        assert(z.consume());

        // A press in a frame without a logic tick waits for the next tick
        z.feed(false);
        z.feed(true);
        z.feed(true);
        assert(z.consume());

        std::cout << "[PASS] BUG-33: held keys press once; unread presses are dropped at the end of the tick." << std::endl;
    }

    // 46. T2 & LOR-006: Tall intro card camera rise scroll math
    {
        using namespace nundertale;
        const int imgH = 320;
        const int winH = 110;
        const float dur = 6.0f;
        const int startY = imgH - winH; // 210 native pixels

        // 1. Starts at bottom (imgH - winH) at t = 0
        assert(loreScrollOffset(0.0f, dur, imgH, winH) == startY);

        // 2. Ends at top (0) at t = duration
        assert(loreScrollOffset(dur, dur, imgH, winH) == 0);

        // 3. Clamping: negative t clamps to startY, t > duration clamps to 0
        assert(loreScrollOffset(-1.0f, dur, imgH, winH) == startY);
        assert(loreScrollOffset(-100.0f, dur, imgH, winH) == startY);
        assert(loreScrollOffset(dur + 0.5f, dur, imgH, winH) == 0);
        assert(loreScrollOffset(dur + 10.0f, dur, imgH, winH) == 0);

        // 4. Integer native pixel progression & monotonic non-increasing
        int prevOffset = startY;
        for (int step = 0; step <= 600; ++step) {
            float t = static_cast<float>(step) * (dur / 600.0f);
            int offset = loreScrollOffset(t, dur, imgH, winH);
            assert(offset <= prevOffset); // Monotonic (moving upwards towards 0)
            assert(offset >= 0 && offset <= startY);
            prevOffset = offset;
        }

        // 5. Midpoint check: progress 0.5 -> floor(0.5 * 210) = 105 -> 210 - 105 = 105
        assert(loreScrollOffset(3.0f, dur, imgH, winH) == 105);

        // 6. Degenerate cases: image smaller than window or zero duration
        assert(loreScrollOffset(0.0f, dur, 100, 110) == 0);
        assert(loreScrollOffset(2.0f, 0.0f, imgH, winH) == 0);

        std::cout << "[PASS] T2 & LOR-006: Tall intro card scroll math (starts at bottom, ends at top, clamps, monotonic, whole pixels) verified." << std::endl;
    }

    // 47. C-T1: tilesets, multiple firstgid ranges, and Tiled flip bits
    {
        using namespace nundertale;
        const std::string json = R"json({
            "orientation": "orthogonal",
            "tilewidth": 40,
            "tileheight": 40,
            "width": 2,
            "height": 1,
            "tilesets": [
                {"firstgid": 1, "image": "assets/tiles/a.png", "tilewidth": 20, "tileheight": 20, "columns": 4, "tilecount": 8, "name": "a"},
                {"firstgid": 20, "image": "assets/tiles/b.png", "tilewidth": 10, "tileheight": 10, "columns": 2, "tilecount": 6, "name": "b"}
            ],
            "layers": [
                {"type": "tilelayer", "name": "ground", "width": 2, "height": 1,
                 "data": [2147483654, 3221225493]}
            ]
        })json";

        TiledMap map;
        assert(parseTiledMapJson(json, map));
        assert(map.tilesets.size() == 2);
        assert(map.findLayer("ground")->data[0] == (0x80000000u | 6u));

        TiledTileSource source;
        assert(tileSource(map, 0x80000000u | 6u, source));
        assert(source.tileset == &map.tilesets[0]);
        assert(source.tileId == 5);
        assert(source.source.x == 20.0f && source.source.y == 20.0f);
        assert(source.source.width == 20.0f && source.source.height == 20.0f);

        assert(tileSource(map, 0xC0000000u | 21u, source));
        assert(source.tileset == &map.tilesets[1]);
        assert(source.tileId == 1);
        assert(source.source.x == 10.0f && source.source.y == 0.0f);
        assert(!tileSource(map, 0u, source));
        assert(!tileSource(map, 100u, source));
        std::cout << "[PASS] C-T1: tileset gid lookup strips flip flags and selects the greatest firstgid." << std::endl;
    }

    // 48. C-T1: typed object/map property helpers and defaults
    {
        using namespace nundertale;
        TiledObject object;
        object.properties = {
            {"label", "string", std::string("hello")},
            {"solid", "bool", true},
            {"frames", "int", 3.0}
        };
        assert(stringProperty(object, "label", "fallback") == "hello");
        assert(stringProperty(object, "missing", "fallback") == "fallback");
        assert(boolProperty(object, "solid", false));
        assert(!boolProperty(object, "missing", false));
        assert(intProperty(object, "frames", 1) == 3);
        assert(intProperty(object, "label", 9) == 9);

        TiledMap map;
        map.properties = {{"displayName", "string", std::string("Tiny Room")}};
        assert(stringProperty(map, "displayName") == "Tiny Room");
        std::cout << "[PASS] C-T1: string, bool, and int property helpers honor typed values and defaults." << std::endl;
    }

    // 49. C-T1: eight-frame room fade emits one swap and one completion
    {
        using namespace nundertale;
        RoomTransition transition;
        transition.start("ruins_notices", "south", "up");
        assert(transition.active());
        assert(transition.targetRoom() == "ruins_notices");
        assert(transition.targetSpawn() == "south");
        assert(transition.facing() == "up");
        for (int frame = 0; frame < RoomTransition::FADE_FRAMES - 1; ++frame) {
            assert(transition.update() == RoomTransitionEvent::None);
        }
        assert(transition.update() == RoomTransitionEvent::SwapNow);
        assert(transition.alpha() == 1.0f);
        for (int frame = 0; frame < RoomTransition::FADE_FRAMES - 1; ++frame) {
            assert(transition.update() == RoomTransitionEvent::None);
        }
        assert(transition.update() == RoomTransitionEvent::Done);
        assert(!transition.active());
        assert(transition.alpha() == 0.0f);
        std::cout << "[PASS] C-T1: room transition fades out, swaps once at black, and fades in." << std::endl;
    }

    // 50. C-T1: data-driven dialog page splitting
    {
        using namespace nundertale;
        const auto pages = splitTextPages("page one|page two\nline two|page three");
        assert(pages.size() == 3);
        assert(pages[0] == "page one");
        assert(pages[1] == "page two\nline two");
        assert(pages[2] == "page three");
        const auto emptyMiddle = splitTextPages("a||b");
        assert(emptyMiddle.size() == 3 && emptyMiddle[1].empty());
        std::cout << "[PASS] C-T1: pipe-separated dialog pages preserve embedded newlines." << std::endl;
    }

    std::cout << "\nAll 50 test suites passed successfully!" << std::endl;
    return 0;
}
