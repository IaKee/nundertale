#pragma once

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <optional>
#include <sstream>
#include <string>

#include "core/SpriteSheet.hpp"

namespace nundertale {

struct PortraitDef {
    std::string id;
    std::string imagePath;
    std::string jsonPath;
    SpriteSheetDef sheet;
};

inline bool portraitIdValid(const std::string& id) {
    return !id.empty() && std::all_of(id.begin(), id.end(), [](unsigned char ch) {
        return std::isalnum(ch) != 0 || ch == '_' || ch == '-';
    });
}

inline float portraitDrawScale(int density) {
    return 2.0f / static_cast<float>(std::max(1, density));
}

// Portrait ids are file stems below assets/portraits. A sidecar is optional; without one the
// image is a single density-1 frame. Invalid sidecars fail closed so a bad asset cannot be drawn
// at the wrong size.
inline std::optional<PortraitDef> resolvePortrait(
    const std::string& id, const std::filesystem::path& repositoryRoot = ".") {
    if (!portraitIdValid(id)) return std::nullopt;

    const std::filesystem::path directory = repositoryRoot / "assets" / "portraits";
    const std::filesystem::path imagePath = directory / (id + ".png");
    std::error_code error;
    if (!std::filesystem::is_regular_file(imagePath, error) || error) return std::nullopt;

    PortraitDef portrait;
    portrait.id = id;
    portrait.imagePath = imagePath.lexically_normal().generic_string();
    portrait.sheet.image = imagePath.filename().generic_string();
    portrait.sheet.density = 1;
    portrait.sheet.anims.emplace("idle", SpriteAnim{});

    const std::filesystem::path jsonPath = directory / (id + ".json");
    error.clear();
    if (!std::filesystem::exists(jsonPath, error)) {
        if (error) return std::nullopt;
        return portrait;
    }
    if (error || !std::filesystem::is_regular_file(jsonPath, error) || error) return std::nullopt;

    std::ifstream input(jsonPath);
    std::ostringstream contents;
    contents << input.rdbuf();
    SpriteSheetDef sheet;
    if (!input || !parseSpriteSheetJson(contents.str(), sheet)) return std::nullopt;
    if (sheet.image != imagePath.filename().generic_string()) return std::nullopt;

    portrait.jsonPath = jsonPath.lexically_normal().generic_string();
    portrait.sheet = std::move(sheet);
    return portrait;
}

} // namespace nundertale
