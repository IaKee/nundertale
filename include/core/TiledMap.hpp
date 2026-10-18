#pragma once

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <limits>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace nundertale {

struct TiledPoint {
    float x = 0.0f;
    float y = 0.0f;
};

struct TiledRect {
    float x = 0.0f;
    float y = 0.0f;
    float width = 0.0f;
    float height = 0.0f;
};

using TiledPropertyValue = std::variant<std::monostate, bool, double, std::string>;

struct TiledProperty {
    std::string name;
    std::string type;
    TiledPropertyValue value;
};

struct TiledObject {
    int id = 0;
    std::string name;
    std::string type;
    std::string className;
    float x = 0.0f;
    float y = 0.0f;
    float width = 0.0f;
    float height = 0.0f;
    std::vector<TiledPoint> polygon;
    std::vector<TiledProperty> properties;

    TiledRect bounds() const {
        if (polygon.empty()) return {x, y, width, height};

        float minX = polygon.front().x;
        float minY = polygon.front().y;
        float maxX = minX;
        float maxY = minY;
        for (const TiledPoint& point : polygon) {
            minX = std::min(minX, point.x);
            minY = std::min(minY, point.y);
            maxX = std::max(maxX, point.x);
            maxY = std::max(maxY, point.y);
        }
        return {x + minX, y + minY, maxX - minX, maxY - minY};
    }

    const TiledProperty* findProperty(const std::string& propertyName) const {
        for (const TiledProperty& property : properties) {
            if (property.name == propertyName) return &property;
        }
        return nullptr;
    }
};

struct TiledTileset {
    int firstGid = 0;
    std::string image;
    int tileWidth = 0;
    int tileHeight = 0;
    int columns = 0;
    int tileCount = 0;
};

struct TiledTileSource {
    const TiledTileset* tileset = nullptr;
    TiledRect source;
    int tileId = 0;
};

enum class TiledLayerType {
    Tile,
    Image,
    Objects,
    Unknown
};

struct TiledLayer {
    TiledLayerType type = TiledLayerType::Unknown;
    std::string name;
    int width = 0;
    int height = 0;
    std::vector<std::uint32_t> data;
    std::string image;
    float offsetX = 0.0f;
    float offsetY = 0.0f;
    std::vector<TiledObject> objects;

    const TiledObject* findObject(const std::string& objectName) const {
        for (const TiledObject& object : objects) {
            if (object.name == objectName) return &object;
        }
        return nullptr;
    }
};

struct TiledMap {
    std::string orientation;
    int tileWidth = 0;
    int tileHeight = 0;
    int width = 0;
    int height = 0;
    std::vector<TiledProperty> properties;
    std::vector<TiledTileset> tilesets;
    std::vector<TiledLayer> layers;

    float pixelWidth() const {
        return static_cast<float>(width * tileWidth);
    }

    float pixelHeight() const {
        return static_cast<float>(height * tileHeight);
    }

    const TiledLayer* findLayer(const std::string& layerName) const {
        for (const TiledLayer& layer : layers) {
            if (layer.name == layerName) return &layer;
        }
        return nullptr;
    }

    const TiledObject* findObject(const std::string& objectName) const {
        for (const TiledLayer& layer : layers) {
            const TiledObject* object = layer.findObject(objectName);
            if (object != nullptr) return object;
        }
        return nullptr;
    }

    const TiledProperty* findProperty(const std::string& propertyName) const {
        for (const TiledProperty& property : properties) {
            if (property.name == propertyName) return &property;
        }
        return nullptr;
    }
};

inline std::string stringProperty(const std::vector<TiledProperty>& properties,
                                  const std::string& name,
                                  const std::string& fallback = {}) {
    for (const TiledProperty& property : properties) {
        if (property.name == name) {
            const std::string* value = std::get_if<std::string>(&property.value);
            return value == nullptr ? fallback : *value;
        }
    }
    return fallback;
}

inline bool boolProperty(const std::vector<TiledProperty>& properties,
                         const std::string& name,
                         bool fallback = false) {
    for (const TiledProperty& property : properties) {
        if (property.name == name) {
            const bool* value = std::get_if<bool>(&property.value);
            return value == nullptr ? fallback : *value;
        }
    }
    return fallback;
}

inline int intProperty(const std::vector<TiledProperty>& properties,
                       const std::string& name,
                       int fallback = 0) {
    for (const TiledProperty& property : properties) {
        if (property.name == name) {
            const double* value = std::get_if<double>(&property.value);
            if (value == nullptr || std::floor(*value) != *value ||
                *value < static_cast<double>(std::numeric_limits<int>::min()) ||
                *value > static_cast<double>(std::numeric_limits<int>::max())) {
                return fallback;
            }
            return static_cast<int>(*value);
        }
    }
    return fallback;
}

inline std::string stringProperty(const TiledObject& object,
                                  const std::string& name,
                                  const std::string& fallback = {}) {
    return stringProperty(object.properties, name, fallback);
}

inline bool boolProperty(const TiledObject& object, const std::string& name, bool fallback = false) {
    return boolProperty(object.properties, name, fallback);
}

inline int intProperty(const TiledObject& object, const std::string& name, int fallback = 0) {
    return intProperty(object.properties, name, fallback);
}

inline std::string stringProperty(const TiledMap& map,
                                  const std::string& name,
                                  const std::string& fallback = {}) {
    return stringProperty(map.properties, name, fallback);
}

inline bool boolProperty(const TiledMap& map, const std::string& name, bool fallback = false) {
    return boolProperty(map.properties, name, fallback);
}

inline int intProperty(const TiledMap& map, const std::string& name, int fallback = 0) {
    return intProperty(map.properties, name, fallback);
}

inline std::string tiledObjectType(const TiledObject& object) {
    return object.type.empty() ? object.className : object.type;
}

inline bool tileSource(const TiledMap& map, std::uint32_t rawGid, TiledTileSource& output) {
    constexpr std::uint32_t FLIP_FLAGS = 0xE0000000u;
    const std::uint32_t gid = rawGid & ~FLIP_FLAGS;
    if (gid == 0u) return false;

    const TiledTileset* selected = nullptr;
    for (const TiledTileset& tileset : map.tilesets) {
        if (tileset.firstGid > 0 && static_cast<std::uint32_t>(tileset.firstGid) <= gid &&
            (selected == nullptr || tileset.firstGid > selected->firstGid)) {
            selected = &tileset;
        }
    }
    if (selected == nullptr || selected->columns <= 0 || selected->tileWidth <= 0 ||
        selected->tileHeight <= 0) {
        return false;
    }

    const std::uint32_t local = gid - static_cast<std::uint32_t>(selected->firstGid);
    if (selected->tileCount > 0 && local >= static_cast<std::uint32_t>(selected->tileCount)) {
        return false;
    }
    const std::uint32_t columns = static_cast<std::uint32_t>(selected->columns);
    output.tileset = selected;
    output.tileId = static_cast<int>(local);
    output.source = {
        static_cast<float>((local % columns) * static_cast<std::uint32_t>(selected->tileWidth)),
        static_cast<float>((local / columns) * static_cast<std::uint32_t>(selected->tileHeight)),
        static_cast<float>(selected->tileWidth),
        static_cast<float>(selected->tileHeight)
    };
    return true;
}

inline std::vector<std::string> splitTextPages(const std::string& text) {
    std::vector<std::string> pages;
    std::size_t start = 0;
    while (true) {
        const std::size_t separator = text.find('|', start);
        pages.push_back(text.substr(start, separator - start));
        if (separator == std::string::npos) break;
        start = separator + 1;
    }
    return pages;
}

namespace tiled_detail {

struct JsonValue {
    enum class Kind {
        Null,
        Boolean,
        Number,
        String,
        Array,
        Object
    };

    Kind kind = Kind::Null;
    bool boolean = false;
    double number = 0.0;
    std::string string;
    std::vector<JsonValue> array;
    std::map<std::string, JsonValue> object;

    const JsonValue* field(const std::string& key) const {
        const auto found = object.find(key);
        return found == object.end() ? nullptr : &found->second;
    }
};

class JsonParser {
public:
    explicit JsonParser(const std::string& text) : m_text(text) {}

    bool parse(JsonValue& output) {
        JsonValue parsed;
        skipWhitespace();
        if (!parseValue(parsed)) return false;
        skipWhitespace();
        if (!atEnd()) return false;
        output = std::move(parsed);
        return true;
    }

private:
    bool parseValue(JsonValue& output) {
        skipWhitespace();
        if (atEnd()) return false;
        if (m_text[m_pos] == '{') return parseObject(output);
        if (m_text[m_pos] == '[') return parseArray(output);
        if (m_text[m_pos] == '"') {
            output.kind = JsonValue::Kind::String;
            return parseString(output.string);
        }
        if (m_text[m_pos] == 't') return parseLiteral("true", JsonValue::Kind::Boolean, output, true);
        if (m_text[m_pos] == 'f') return parseLiteral("false", JsonValue::Kind::Boolean, output, false);
        if (m_text[m_pos] == 'n') return parseLiteral("null", JsonValue::Kind::Null, output, false);
        return parseNumber(output);
    }

    bool parseObject(JsonValue& output) {
        if (!consume('{')) return false;
        output.kind = JsonValue::Kind::Object;
        output.object.clear();
        skipWhitespace();
        if (consume('}')) return true;

        while (true) {
            std::string key;
            if (!parseString(key)) return false;
            skipWhitespace();
            if (!consume(':')) return false;
            JsonValue value;
            if (!parseValue(value)) return false;
            output.object[std::move(key)] = std::move(value);
            skipWhitespace();
            if (consume('}')) return true;
            if (!consume(',')) return false;
            skipWhitespace();
        }
    }

    bool parseArray(JsonValue& output) {
        if (!consume('[')) return false;
        output.kind = JsonValue::Kind::Array;
        output.array.clear();
        skipWhitespace();
        if (consume(']')) return true;

        while (true) {
            JsonValue value;
            if (!parseValue(value)) return false;
            output.array.push_back(std::move(value));
            skipWhitespace();
            if (consume(']')) return true;
            if (!consume(',')) return false;
            skipWhitespace();
        }
    }

    bool parseString(std::string& output) {
        skipWhitespace();
        if (!consume('"')) return false;
        output.clear();
        while (!atEnd()) {
            const char ch = m_text[m_pos++];
            if (ch == '"') return true;
            if (ch != '\\') {
                output.push_back(ch);
                continue;
            }
            if (atEnd()) return false;
            const char escaped = m_text[m_pos++];
            switch (escaped) {
                case '"': output.push_back('"'); break;
                case '\\': output.push_back('\\'); break;
                case '/': output.push_back('/'); break;
                case 'b': output.push_back('\b'); break;
                case 'f': output.push_back('\f'); break;
                case 'n': output.push_back('\n'); break;
                case 'r': output.push_back('\r'); break;
                case 't': output.push_back('\t'); break;
                default: return false;
            }
        }
        return false;
    }

    bool parseNumber(JsonValue& output) {
        const char* begin = m_text.c_str() + m_pos;
        char* end = nullptr;
        const double value = std::strtod(begin, &end);
        if (end == begin || !std::isfinite(value)) return false;
        m_pos = static_cast<std::size_t>(end - m_text.c_str());
        output.kind = JsonValue::Kind::Number;
        output.number = value;
        return true;
    }

    bool parseLiteral(const char* literal,
                      JsonValue::Kind kind,
                      JsonValue& output,
                      bool boolean) {
        const std::string token(literal);
        if (m_text.compare(m_pos, token.size(), token) != 0) return false;
        m_pos += token.size();
        output.kind = kind;
        output.boolean = boolean;
        return true;
    }

    void skipWhitespace() {
        while (!atEnd() && std::isspace(static_cast<unsigned char>(m_text[m_pos])) != 0) ++m_pos;
    }

    bool consume(char expected) {
        if (!atEnd() && m_text[m_pos] == expected) {
            ++m_pos;
            return true;
        }
        return false;
    }

    bool atEnd() const { return m_pos >= m_text.size(); }

    const std::string& m_text;
    std::size_t m_pos = 0;
};

inline bool stringField(const JsonValue& object, const char* key, std::string& output) {
    const JsonValue* value = object.field(key);
    if (value == nullptr || value->kind != JsonValue::Kind::String) return false;
    output = value->string;
    return true;
}

inline bool optionalStringField(const JsonValue& object, const char* key, std::string& output) {
    const JsonValue* value = object.field(key);
    if (value == nullptr) return true;
    if (value->kind != JsonValue::Kind::String) return false;
    output = value->string;
    return true;
}

inline bool numberField(const JsonValue& object, const char* key, double& output) {
    const JsonValue* value = object.field(key);
    if (value == nullptr || value->kind != JsonValue::Kind::Number) return false;
    output = value->number;
    return true;
}

inline bool optionalNumberField(const JsonValue& object, const char* key, double& output) {
    const JsonValue* value = object.field(key);
    if (value == nullptr) return true;
    if (value->kind != JsonValue::Kind::Number) return false;
    output = value->number;
    return true;
}

inline bool toInt(double value, int& output) {
    if (std::floor(value) != value ||
        value < static_cast<double>(std::numeric_limits<int>::min()) ||
        value > static_cast<double>(std::numeric_limits<int>::max())) {
        return false;
    }
    output = static_cast<int>(value);
    return true;
}

inline bool toUInt32(double value, std::uint32_t& output) {
    if (std::floor(value) != value || value < 0.0 ||
        value > static_cast<double>(std::numeric_limits<std::uint32_t>::max())) {
        return false;
    }
    output = static_cast<std::uint32_t>(value);
    return true;
}

inline bool intField(const JsonValue& object, const char* key, int& output) {
    double value = 0.0;
    return numberField(object, key, value) && toInt(value, output);
}

inline bool optionalIntField(const JsonValue& object, const char* key, int& output) {
    const JsonValue* value = object.field(key);
    if (value == nullptr) return true;
    return value->kind == JsonValue::Kind::Number && toInt(value->number, output);
}

inline bool parsePoint(const JsonValue& value, TiledPoint& point) {
    if (value.kind != JsonValue::Kind::Object) return false;
    double x = 0.0;
    double y = 0.0;
    if (!numberField(value, "x", x) || !numberField(value, "y", y)) return false;
    point = {static_cast<float>(x), static_cast<float>(y)};
    return true;
}

inline bool parseProperty(const JsonValue& value, TiledProperty& property) {
    if (value.kind != JsonValue::Kind::Object || !stringField(value, "name", property.name)) return false;
    if (!optionalStringField(value, "type", property.type)) return false;
    const JsonValue* raw = value.field("value");
    if (raw == nullptr) return false;
    switch (raw->kind) {
        case JsonValue::Kind::Null: property.value = std::monostate{}; return true;
        case JsonValue::Kind::Boolean: property.value = raw->boolean; return true;
        case JsonValue::Kind::Number: property.value = raw->number; return true;
        case JsonValue::Kind::String: property.value = raw->string; return true;
        default: return false;
    }
}

inline bool parseProperties(const JsonValue& owner, std::vector<TiledProperty>& properties) {
    const JsonValue* values = owner.field("properties");
    if (values == nullptr) return true;
    if (values->kind != JsonValue::Kind::Array) return false;
    for (const JsonValue& rawProperty : values->array) {
        TiledProperty property;
        if (!parseProperty(rawProperty, property)) return false;
        properties.push_back(std::move(property));
    }
    return true;
}

inline bool parseObject(const JsonValue& value, TiledObject& object) {
    if (value.kind != JsonValue::Kind::Object || !intField(value, "id", object.id)) return false;
    if (!optionalStringField(value, "name", object.name) ||
        !optionalStringField(value, "type", object.type) ||
        !optionalStringField(value, "class", object.className)) {
        return false;
    }

    double x = 0.0;
    double y = 0.0;
    double width = 0.0;
    double height = 0.0;
    if (!optionalNumberField(value, "x", x) || !optionalNumberField(value, "y", y) ||
        !optionalNumberField(value, "width", width) || !optionalNumberField(value, "height", height)) {
        return false;
    }
    object.x = static_cast<float>(x);
    object.y = static_cast<float>(y);
    object.width = static_cast<float>(width);
    object.height = static_cast<float>(height);

    if (const JsonValue* polygon = value.field("polygon")) {
        if (polygon->kind != JsonValue::Kind::Array) return false;
        for (const JsonValue& rawPoint : polygon->array) {
            TiledPoint point;
            if (!parsePoint(rawPoint, point)) return false;
            object.polygon.push_back(point);
        }
    }

    return parseProperties(value, object.properties);
}

inline bool parseTileset(const JsonValue& value, TiledTileset& tileset) {
    if (value.kind != JsonValue::Kind::Object ||
        !intField(value, "firstgid", tileset.firstGid) ||
        !stringField(value, "image", tileset.image) ||
        !intField(value, "tilewidth", tileset.tileWidth) ||
        !intField(value, "tileheight", tileset.tileHeight) ||
        !intField(value, "columns", tileset.columns) ||
        !intField(value, "tilecount", tileset.tileCount)) {
        return false;
    }
    return tileset.firstGid > 0 && tileset.tileWidth > 0 && tileset.tileHeight > 0 &&
           tileset.columns > 0 && tileset.tileCount > 0 && !tileset.image.empty();
}

inline bool parseLayer(const JsonValue& value, TiledLayer& layer) {
    if (value.kind != JsonValue::Kind::Object) return false;
    std::string type;
    if (!stringField(value, "type", type) || !optionalStringField(value, "name", layer.name)) return false;

    if (type == "tilelayer") {
        layer.type = TiledLayerType::Tile;
        if (!optionalIntField(value, "width", layer.width) || !optionalIntField(value, "height", layer.height)) return false;
        const JsonValue* data = value.field("data");
        if (data == nullptr || data->kind != JsonValue::Kind::Array) return false;
        for (const JsonValue& entry : data->array) {
            std::uint32_t tile = 0;
            if (entry.kind != JsonValue::Kind::Number || !toUInt32(entry.number, tile)) return false;
            layer.data.push_back(tile);
        }
    } else if (type == "imagelayer") {
        layer.type = TiledLayerType::Image;
        if (!stringField(value, "image", layer.image)) return false;
        double offsetX = 0.0;
        double offsetY = 0.0;
        if (!optionalNumberField(value, "offsetx", offsetX) ||
            !optionalNumberField(value, "offsety", offsetY)) {
            return false;
        }
        layer.offsetX = static_cast<float>(offsetX);
        layer.offsetY = static_cast<float>(offsetY);
    } else if (type == "objectgroup") {
        layer.type = TiledLayerType::Objects;
        const JsonValue* objects = value.field("objects");
        if (objects == nullptr || objects->kind != JsonValue::Kind::Array) return false;
        for (const JsonValue& rawObject : objects->array) {
            TiledObject object;
            if (!parseObject(rawObject, object)) return false;
            layer.objects.push_back(std::move(object));
        }
    } else {
        layer.type = TiledLayerType::Unknown;
    }
    return true;
}

} // namespace tiled_detail

inline bool parseTiledMapJson(const std::string& text, TiledMap& output) {
    tiled_detail::JsonValue root;
    if (!tiled_detail::JsonParser(text).parse(root) ||
        root.kind != tiled_detail::JsonValue::Kind::Object) {
        return false;
    }

    TiledMap parsed;
    if (!tiled_detail::stringField(root, "orientation", parsed.orientation) ||
        parsed.orientation != "orthogonal" ||
        !tiled_detail::intField(root, "tilewidth", parsed.tileWidth) ||
        !tiled_detail::intField(root, "tileheight", parsed.tileHeight) ||
        !tiled_detail::intField(root, "width", parsed.width) ||
        !tiled_detail::intField(root, "height", parsed.height) ||
        parsed.tileWidth <= 0 || parsed.tileHeight <= 0 || parsed.width <= 0 || parsed.height <= 0) {
        return false;
    }

    const tiled_detail::JsonValue* layers = root.field("layers");
    if (layers == nullptr || layers->kind != tiled_detail::JsonValue::Kind::Array) return false;
    for (const tiled_detail::JsonValue& rawLayer : layers->array) {
        TiledLayer layer;
        if (!tiled_detail::parseLayer(rawLayer, layer)) return false;
        parsed.layers.push_back(std::move(layer));
    }

    if (!tiled_detail::parseProperties(root, parsed.properties)) return false;

    if (const tiled_detail::JsonValue* tilesets = root.field("tilesets")) {
        if (tilesets->kind != tiled_detail::JsonValue::Kind::Array) return false;
        for (const tiled_detail::JsonValue& rawTileset : tilesets->array) {
            TiledTileset tileset;
            if (!tiled_detail::parseTileset(rawTileset, tileset)) return false;
            parsed.tilesets.push_back(std::move(tileset));
        }
    }

    output = std::move(parsed);
    return true;
}

inline bool loadTiledMapFromFile(const std::string& path, TiledMap& output) {
    std::ifstream file(path);
    if (!file) return false;
    std::ostringstream json;
    json << file.rdbuf();
    return parseTiledMapJson(json.str(), output);
}

} // namespace nundertale
