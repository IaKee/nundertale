#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <optional>
#include <raylib.h>
#include <sstream>
#include <string>
#include "core/SpriteSheet.hpp"

namespace nundertale {

class SpriteSheetCache {
public:
    const SpriteSheetDef* load(const std::string& imagePath, const std::string& jsonPath) {
        const std::string key = imagePath + "\n" + jsonPath;
        const auto existing = m_defs.find(key);
        if (existing != m_defs.end()) {
            return existing->second ? &*existing->second : nullptr;
        }

        std::error_code imageError;
        std::error_code jsonError;
        const bool imageExists = std::filesystem::exists(imagePath, imageError);
        const bool jsonExists = std::filesystem::exists(jsonPath, jsonError);
        if (!imageExists || !jsonExists || imageError || jsonError) {
            std::cerr << "[SpriteSheet] Falling back for " << jsonPath
                      << ": PNG or JSON file is missing.\n";
            m_defs.emplace(key, std::nullopt);
            return nullptr;
        }

        std::ifstream file(jsonPath);
        std::ostringstream contents;
        contents << file.rdbuf();
        SpriteSheetDef parsed;
        if (!file || !parseSpriteSheetJson(contents.str(), parsed)) {
            std::cerr << "[SpriteSheet] Falling back for " << jsonPath
                      << ": JSON is invalid.\n";
            m_defs.emplace(key, std::nullopt);
            return nullptr;
        }

        const auto inserted = m_defs.emplace(key, std::move(parsed));
        return &*inserted.first->second;
    }

private:
    std::map<std::string, std::optional<SpriteSheetDef>> m_defs;
};

inline SpriteSheetCache& spriteSheetCache() {
    static SpriteSheetCache cache;
    return cache;
}

inline void drawSprite(Texture2D texture, const SpriteSheetDef& sheet,
                       const std::string& animation, float time, Vector2 position,
                       float scale = 2.0f, Color tint = WHITE) {
    const FrameRect frame = frameRect(sheet, animation, time);
    const float drawScale = scale > 0.0f ? scale : 1.0f;
    const Rectangle source = {
        static_cast<float>(frame.x),
        static_cast<float>(frame.y),
        static_cast<float>(frame.w),
        static_cast<float>(frame.h)
    };
    const Rectangle destination = {
        position.x - static_cast<float>(sheet.originX) * drawScale,
        position.y - static_cast<float>(sheet.originY) * drawScale,
        static_cast<float>(frame.w) * drawScale,
        static_cast<float>(frame.h) * drawScale
    };
    DrawTexturePro(texture, source, destination, { 0.0f, 0.0f }, 0.0f, tint);
}

} // namespace nundertale
