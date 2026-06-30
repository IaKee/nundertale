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

inline int wrapShopIndex(int index, int delta, int count) {
    if (count <= 0) return 0;
    const int wrapped = (index + delta) % count;
    return wrapped < 0 ? wrapped + count : wrapped;
}

inline ShopPurchaseResult tryShopPurchase(const ShopItem& item, int& gold,
                                          std::vector<std::string>& inventory,
                                          std::size_t inventoryCapacity) {
    if (gold < item.price) return ShopPurchaseResult::NotEnoughGold;
    if (inventory.size() >= inventoryCapacity) return ShopPurchaseResult::InventoryFull;
    gold -= item.price;
    inventory.push_back(item.item);
    return ShopPurchaseResult::Success;
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
