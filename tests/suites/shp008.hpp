#pragma once

#include "scenes/ShopModel.hpp"
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

inline void suite_shp008() {
    using namespace nundertale;

    static bool s_alreadyRan = false;
    if (s_alreadyRan) return;
    s_alreadyRan = true;

    // Verify named constants
    static_assert(FUR_FANDOM_DISCOUNT_THRESHOLD == 60);
    static_assert(FUR_FANDOM_DISCOUNT_PERCENT == 10);
    static_assert(FURRY_WELCOME_DISCOUNT_PERCENT == 10);

    // 1. Baseline: no discounts (FUR < 60, furryRoute == false)
    {
        const ShopPriceContext ctxBaseline{ 0, false };
        assert(shopFinalPrice(0, ctxBaseline) == 0);
        assert(shopFinalPrice(1, ctxBaseline) == 1);
        assert(shopFinalPrice(2, ctxBaseline) == 2);
        assert(shopFinalPrice(9, ctxBaseline) == 9);
        assert(shopFinalPrice(10, ctxBaseline) == 10);
        assert(shopFinalPrice(100, ctxBaseline) == 100);

        // Boundary: FUR 59 vs 60
        const ShopPriceContext ctxFur59{ 59, false };
        assert(shopFinalPrice(0, ctxFur59) == 0);
        assert(shopFinalPrice(1, ctxFur59) == 1);
        assert(shopFinalPrice(2, ctxFur59) == 2);
        assert(shopFinalPrice(9, ctxFur59) == 9);
        assert(shopFinalPrice(10, ctxFur59) == 10);
        assert(shopFinalPrice(100, ctxFur59) == 100);
    }

    // 2. FUR >= 60 threshold (fandom discount -10%)
    {
        const ShopPriceContext ctxFur60{ 60, false };
        // 0G stays 0G
        assert(shopFinalPrice(0, ctxFur60) == 0);
        // Small price rounding: never below 1G for non-zero base
        assert(shopFinalPrice(1, ctxFur60) == 1);
        // 2G: (2 * 90) / 100 = 1G
        assert(shopFinalPrice(2, ctxFur60) == 1);
        // 9G: (9 * 90) / 100 = 8G
        assert(shopFinalPrice(9, ctxFur60) == 8);
        // Standard prices
        assert(shopFinalPrice(10, ctxFur60) == 9);
        assert(shopFinalPrice(20, ctxFur60) == 18);
        assert(shopFinalPrice(50, ctxFur60) == 45);
        assert(shopFinalPrice(100, ctxFur60) == 90);

        // High FUR values keep discount active
        const ShopPriceContext ctxFur100{ 100, false };
        assert(shopFinalPrice(100, ctxFur100) == 90);
        assert(shopFinalPrice(9, ctxFur100) == 8);
        assert(shopFinalPrice(1, ctxFur100) == 1);
    }

    // 3. Furry route on/off (welcome discount -10%)
    {
        const ShopPriceContext ctxFurryRoute{ 0, true };
        assert(shopFinalPrice(0, ctxFurryRoute) == 0);
        assert(shopFinalPrice(1, ctxFurryRoute) == 1);
        assert(shopFinalPrice(2, ctxFurryRoute) == 1);
        assert(shopFinalPrice(9, ctxFurryRoute) == 8);
        assert(shopFinalPrice(10, ctxFurryRoute) == 9);
        assert(shopFinalPrice(100, ctxFurryRoute) == 90);

        const ShopPriceContext ctxFurryRouteFur59{ 59, true };
        assert(shopFinalPrice(100, ctxFurryRouteFur59) == 90);
    }

    // 4. Both modifiers stacked (FUR >= 60 and furryRoute == true)
    // Modifiers multiply: 0.90 * 0.90 = 0.81 (19% total discount)
    {
        const ShopPriceContext ctxStacked{ 60, true };
        assert(shopFinalPrice(0, ctxStacked) == 0);
        assert(shopFinalPrice(1, ctxStacked) == 1); // never below 1G
        assert(shopFinalPrice(2, ctxStacked) == 1); // (2 * 81) / 100 = 1G
        assert(shopFinalPrice(9, ctxStacked) == 7); // (9 * 81) / 100 = 7G
        assert(shopFinalPrice(10, ctxStacked) == 8); // (10 * 81) / 100 = 8G
        assert(shopFinalPrice(15, ctxStacked) == 12); // (15 * 81) / 100 = 12G
        assert(shopFinalPrice(20, ctxStacked) == 16); // (20 * 81) / 100 = 16G
        assert(shopFinalPrice(50, ctxStacked) == 40); // (50 * 81) / 100 = 40G
        assert(shopFinalPrice(100, ctxStacked) == 81); // (100 * 81) / 100 = 81G

        const ShopPriceContext ctxStackedMax{ 100, true };
        assert(shopFinalPrice(100, ctxStackedMax) == 81);
    }

    // 5. Purchase deduction and boundary failures (exactly 1 gold short)
    {
        // 10G base with FUR 60 -> 9G final price
        const ShopItem item10{ "Test Biscuit", 10 };
        const ShopPriceContext ctxFur60{ 60, false };
        std::vector<std::string> inv;

        // Fails at exactly 1 gold short (8 gold when price is 9)
        int gold = 8;
        ShopPurchaseResult res = tryShopPurchase(item10, gold, inv, 8U, ctxFur60);
        assert(res == ShopPurchaseResult::NotEnoughGold);
        assert(gold == 8);
        assert(inv.empty());

        // Succeeds with exact price (9 gold)
        gold = 9;
        res = tryShopPurchase(item10, gold, inv, 8U, ctxFur60);
        assert(res == ShopPurchaseResult::Success);
        assert(gold == 0);
        assert(inv.size() == 1U && inv[0] == "Test Biscuit");
    }

    // Purchase with stacked discounts (100G base -> 81G final price)
    {
        const ShopItem item100{ "Test Armor", 100 };
        const ShopPriceContext ctxStacked{ 60, true };
        std::vector<std::string> inv;

        // Exactly 1 gold short (80 gold)
        int gold = 80;
        ShopPurchaseResult res = tryShopPurchase(item100, gold, inv, 8U, ctxStacked);
        assert(res == ShopPurchaseResult::NotEnoughGold);
        assert(gold == 80);
        assert(inv.empty());

        // Exactly enough gold (81 gold)
        gold = 81;
        res = tryShopPurchase(item100, gold, inv, 8U, ctxStacked);
        assert(res == ShopPurchaseResult::Success);
        assert(gold == 0);
        assert(inv.size() == 1U && inv[0] == "Test Armor");
    }

    // Purchase at small price: 9G base with FUR 60 -> 8G final price
    {
        const ShopItem item9{ "Test Tea", 9 };
        const ShopPriceContext ctxFur60{ 60, false };
        std::vector<std::string> inv;

        int gold = 7; // 1 gold short
        ShopPurchaseResult res = tryShopPurchase(item9, gold, inv, 8U, ctxFur60);
        assert(res == ShopPurchaseResult::NotEnoughGold);
        assert(gold == 7);

        gold = 8;
        res = tryShopPurchase(item9, gold, inv, 8U, ctxFur60);
        assert(res == ShopPurchaseResult::Success);
        assert(gold == 0);
        assert(inv.size() == 1U && inv[0] == "Test Tea");
    }

    // Purchase at 1G minimum floor (1G base with stacked discounts -> 1G final price)
    {
        const ShopItem item1{ "Test Pebble", 1 };
        const ShopPriceContext ctxStacked{ 80, true };
        std::vector<std::string> inv;

        int gold = 0; // 1 gold short
        ShopPurchaseResult res = tryShopPurchase(item1, gold, inv, 8U, ctxStacked);
        assert(res == ShopPurchaseResult::NotEnoughGold);
        assert(gold == 0);

        gold = 1;
        res = tryShopPurchase(item1, gold, inv, 8U, ctxStacked);
        assert(res == ShopPurchaseResult::Success);
        assert(gold == 0);
        assert(inv.size() == 1U && inv[0] == "Test Pebble");
    }

    // Inventory full check
    {
        const ShopItem item10{ "Test Biscuit", 10 };
        const ShopPriceContext ctxFur60{ 60, false };
        std::vector<std::string> inv(2U, "Junk");

        int gold = 100;
        ShopPurchaseResult res = tryShopPurchase(item10, gold, inv, 2U, ctxFur60);
        assert(res == ShopPurchaseResult::InventoryFull);
        assert(gold == 100);
        assert(inv.size() == 2U);
    }

    // Legacy overload without context (charges full base price)
    {
        const ShopItem item10{ "Test Biscuit", 10 };
        std::vector<std::string> inv;

        int gold = 9; // Full price is 10, so 9 fails
        ShopPurchaseResult res = tryShopPurchase(item10, gold, inv, 8U);
        assert(res == ShopPurchaseResult::NotEnoughGold);
        assert(gold == 9);

        gold = 10;
        res = tryShopPurchase(item10, gold, inv, 8U);
        assert(res == ShopPurchaseResult::Success);
        assert(gold == 0);
        assert(inv.size() == 1U);
    }

    // Explicit charged price overload
    {
        const ShopItem item{ "Explicit", 100 };
        std::vector<std::string> inv;
        int gold = 50;
        ShopPurchaseResult res = tryShopPurchase(item, gold, inv, 8U, 50);
        assert(res == ShopPurchaseResult::Success);
        assert(gold == 0);
    }

    std::cout << "[PASS] SHP-008: shop price modifiers, rounding, and purchase deductions verified." << std::endl;
}

namespace {
struct Shp008AutoRun {
    Shp008AutoRun() {
        suite_shp008();
    }
};

[[maybe_unused]] const Shp008AutoRun s_shp008AutoRun;
} // namespace
