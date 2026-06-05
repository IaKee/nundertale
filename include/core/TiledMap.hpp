#pragma once

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <cstdint>
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
    std::vector<TiledProperty> properties;

    const TiledProperty* findProperty(const std::string& propertyName) const {
        for (const TiledProperty& property : properties) {
            if (property.name == propertyName) return &property;
        }
        return nullptr;
    }

    const TiledObject* findObject(const std::string& objectName) const {
        for (const TiledObject& object : objects) {
            if (object.name == objectName) return &object;
        }
        return nullptr;
    }
};

struct TiledTileset {
    std::uint32_t firstGid = 1;
    std::string name;
    std::string image;
    int tileWidth = 0;
    int tileHeight = 0;
    int columns = 0;
    int tileCount = 0;
    int imageWidth = 0;
    int imageHeight = 0;
};

struct TiledMap {
    std::string orientation;
    int tileWidth = 0;
    int tileHeight = 0;
    int width = 0;
    int height = 0;
    std::vector<TiledLayer> layers;
    std::vector<TiledTileset> tilesets;
    std::vector<TiledProperty> properties;

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

inline bool intField(const JsonValue& object, const char* key, int& output) {
    double value = 0.0;
    return numberField(object, key, value) && toInt(value, output);
}

inline bool optionalIntField(const JsonValue& object, const char* key, int& output) {
    const JsonValue* value = object.field(key);
    if (value == nullptr) return true;
    return value->kind == JsonValue::Kind::Number && toInt(value->number, output);
}

inline bool uint32Field(const JsonValue& object, const char* key, std::uint32_t& output) {
    double value = 0.0;
    if (!numberField(object, key, value) || std::floor(value) != value ||
        value < 0.0 || value > static_cast<double>(std::numeric_limits<std::uint32_t>::max())) {
        return false;
    }
    output = static_cast<std::uint32_t>(value);
    return true;
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

    if (const JsonValue* properties = value.field("properties")) {
        if (properties->kind != JsonValue::Kind::Array) return false;
        for (const JsonValue& rawProperty : properties->array) {
            TiledProperty property;
            if (!parseProperty(rawProperty, property)) return false;
            object.properties.push_back(std::move(property));
        }
    }
    return true;
}

inline bool parseProperties(const JsonValue& owner, std::vector<TiledProperty>& output) {
    const JsonValue* properties = owner.field("properties");
    if (properties == nullptr) return true;
    if (properties->kind != JsonValue::Kind::Array) return false;
    for (const JsonValue& rawProperty : properties->array) {
        TiledProperty property;
        if (!parseProperty(rawProperty, property)) return false;
        output.push_back(std::move(property));
    }
    return true;
}

inline bool parseTileset(const JsonValue& value, TiledTileset& tileset) {
    if (value.kind != JsonValue::Kind::Object ||
        !uint32Field(value, "firstgid", tileset.firstGid) ||
        !optionalStringField(value, "name", tileset.name) ||
        !optionalStringField(value, "image", tileset.image) ||
        !optionalIntField(value, "tilewidth", tileset.tileWidth) ||
        !optionalIntField(value, "tileheight", tileset.tileHeight) ||
        !optionalIntField(value, "columns", tileset.columns) ||
        !optionalIntField(value, "tilecount", tileset.tileCount) ||
        !optionalIntField(value, "imagewidth", tileset.imageWidth) ||
        !optionalIntField(value, "imageheight", tileset.imageHeight)) {
        return false;
    }
    return tileset.firstGid > 0;
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
            if (entry.kind != JsonValue::Kind::Number || std::floor(entry.number) != entry.number ||
                entry.number < 0.0 ||
                entry.number > static_cast<double>(std::numeric_limits<std::uint32_t>::max())) {
                return false;
            }
            layer.data.push_back(static_cast<std::uint32_t>(entry.number));
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
    return parseProperties(value, layer.properties);
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
