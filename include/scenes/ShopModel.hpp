#pragma once

#include "core/TiledMap.hpp"
#include <cstddef>
#include <fstream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace nundertale {

struct ShopItem {
    std::string item;
    int price = 0;
};

struct ShopTalkTopic {
    std::string label;
    std::vector<std::string> lines;
};

struct ShopData {
    std::string id;
    std::string name;
    std::string greeting;
    std::vector<ShopItem> items;
    std::vector<ShopTalkTopic> talk;
    std::string sellRefusal;
    std::string buyText;
    std::string noGold;
    std::string fullText;
    std::string exitText;
};

enum class ShopPurchaseResult {
    Success,
    NotEnoughGold,
    InventoryFull
};

struct ShopPriceContext {
    int fur = 0;
    bool furryRoute = false;
};

inline constexpr int FUR_FANDOM_DISCOUNT_THRESHOLD = 60;
inline constexpr int FUR_FANDOM_DISCOUNT_PERCENT = 10;
inline constexpr int FURRY_WELCOME_DISCOUNT_PERCENT = 10;

// SHP-008: Calculates the final shop price after applying conditional modifiers.
// Modifiers multiply:
// - FUR >= 60 gives the -10% fandom discount (FUR_FANDOM_DISCOUNT_PERCENT).
// - Furry route (`route.furry`) gives the -10% welcome discount (FURRY_WELCOME_DISCOUNT_PERCENT).
// Modifiers stack multiplicatively (e.g. 0.90 * 0.90 = 0.81).
// Rounding rule: the discounted price rounds down (floor via integer math).
// Non-zero base prices never round below 1G; 0G base price stays 0G.
inline int shopFinalPrice(int basePrice, const ShopPriceContext& context) {
    if (basePrice <= 0) {
        return 0;
    }

    int multNum = 100;
    int multDen = 100;

    if (context.fur >= FUR_FANDOM_DISCOUNT_THRESHOLD) {
        multNum *= (100 - FUR_FANDOM_DISCOUNT_PERCENT);
        multDen *= 100;
    }
    if (context.furryRoute) {
        multNum *= (100 - FURRY_WELCOME_DISCOUNT_PERCENT);
        multDen *= 100;
    }

    const auto scaled = (static_cast<long long>(basePrice) * multNum) / multDen;
    int finalPrice = static_cast<int>(scaled);
    if (finalPrice < 1) {
        finalPrice = 1;
    }
    return finalPrice;
}

inline int wrapShopIndex(int index, int delta, int count) {
    if (count <= 0) return 0;
    const int wrapped = (index + delta) % count;
    return wrapped < 0 ? wrapped + count : wrapped;
}

inline ShopPurchaseResult tryShopPurchase(const ShopItem& item, int& gold,
                                          std::vector<std::string>& inventory,
                                          std::size_t inventoryCapacity,
                                          const ShopPriceContext& priceContext) {
    const int finalPrice = shopFinalPrice(item.price, priceContext);
    if (gold < finalPrice) return ShopPurchaseResult::NotEnoughGold;
    if (inventory.size() >= inventoryCapacity) return ShopPurchaseResult::InventoryFull;
    gold -= finalPrice;
    inventory.push_back(item.item);
    return ShopPurchaseResult::Success;
}

inline ShopPurchaseResult tryShopPurchase(const ShopItem& item, int& gold,
                                          std::vector<std::string>& inventory,
                                          std::size_t inventoryCapacity,
                                          int chargedPrice) {
    if (gold < chargedPrice) return ShopPurchaseResult::NotEnoughGold;
    if (inventory.size() >= inventoryCapacity) return ShopPurchaseResult::InventoryFull;
    gold -= chargedPrice;
    inventory.push_back(item.item);
    return ShopPurchaseResult::Success;
}

inline ShopPurchaseResult tryShopPurchase(const ShopItem& item, int& gold,
                                          std::vector<std::string>& inventory,
                                          std::size_t inventoryCapacity) {
    return tryShopPurchase(item, gold, inventory, inventoryCapacity, ShopPriceContext{});
}

namespace shop_detail {

inline bool readString(const tiled_detail::JsonValue& object, const char* key, std::string& output) {
    const tiled_detail::JsonValue* value = object.field(key);
    if (value == nullptr || value->kind != tiled_detail::JsonValue::Kind::String) return false;
    output = value->string;
    return true;
}

inline bool readStringArray(const tiled_detail::JsonValue& object, const char* key,
                            std::vector<std::string>& output) {
    const tiled_detail::JsonValue* value = object.field(key);
    if (value == nullptr || value->kind != tiled_detail::JsonValue::Kind::Array) return false;
    output.clear();
    for (const tiled_detail::JsonValue& entry : value->array) {
        if (entry.kind != tiled_detail::JsonValue::Kind::String) return false;
        output.push_back(entry.string);
    }
    return true;
}

inline bool readPrice(const tiled_detail::JsonValue& object, const char* key, int& output) {
    const tiled_detail::JsonValue* value = object.field(key);
    if (value == nullptr || value->kind != tiled_detail::JsonValue::Kind::Number ||
        value->number < 0.0 || value->number > 100000.0 ||
        value->number != static_cast<double>(static_cast<int>(value->number))) {
        return false;
    }
    output = static_cast<int>(value->number);
    return true;
}

} // namespace shop_detail

inline bool parseShopJson(const std::string& jsonText, ShopData& output) {
    tiled_detail::JsonValue root;
    if (!tiled_detail::JsonParser(jsonText).parse(root) ||
        root.kind != tiled_detail::JsonValue::Kind::Object) {
        return false;
    }

    ShopData parsed;
    if (!shop_detail::readString(root, "id", parsed.id) || parsed.id.empty() ||
        !shop_detail::readString(root, "name", parsed.name) ||
        !shop_detail::readString(root, "greeting", parsed.greeting) ||
        !shop_detail::readString(root, "sellRefusal", parsed.sellRefusal) ||
        !shop_detail::readString(root, "buyText", parsed.buyText) ||
        !shop_detail::readString(root, "noGold", parsed.noGold) ||
        !shop_detail::readString(root, "fullText", parsed.fullText) ||
        !shop_detail::readString(root, "exitText", parsed.exitText)) {
        return false;
    }

    const tiled_detail::JsonValue* items = root.field("items");
    const tiled_detail::JsonValue* talk = root.field("talk");
    if (items == nullptr || items->kind != tiled_detail::JsonValue::Kind::Array ||
        talk == nullptr || talk->kind != tiled_detail::JsonValue::Kind::Array) {
        return false;
    }

    for (const tiled_detail::JsonValue& entry : items->array) {
        if (entry.kind != tiled_detail::JsonValue::Kind::Object) return false;
        ShopItem item;
        if (!shop_detail::readString(entry, "item", item.item) || item.item.empty() ||
            !shop_detail::readPrice(entry, "price", item.price)) {
            return false;
        }
        parsed.items.push_back(std::move(item));
    }

    for (const tiled_detail::JsonValue& entry : talk->array) {
        if (entry.kind != tiled_detail::JsonValue::Kind::Object) return false;
        ShopTalkTopic topic;
        if (!shop_detail::readString(entry, "label", topic.label) || topic.label.empty() ||
            !shop_detail::readStringArray(entry, "lines", topic.lines) || topic.lines.empty()) {
            return false;
        }
        parsed.talk.push_back(std::move(topic));
    }

    if (parsed.items.empty()) return false;
    output = std::move(parsed);
    return true;
}

inline bool loadShopData(const std::string& path, ShopData& output) {
    std::ifstream file(path);
    if (!file.is_open()) return false;
    std::stringstream buffer;
    buffer << file.rdbuf();
    return parseShopJson(buffer.str(), output);
}

} // namespace nundertale
