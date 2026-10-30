#pragma once

#include <algorithm>
#include <string>
#include <utility>

namespace nundertale {

struct SheetPath : public std::string {
    using std::string::string;
    SheetPath() = default;
    SheetPath(const std::string& str) : std::string(str) {}
    SheetPath(const char* s) : std::string(s ? s : "") {}

    bool operator==(const char* other) const {
        if (!other) return false;
        if (static_cast<const std::string&>(*this) == other) return true;
        if (std::string("assets/") + other == static_cast<const std::string&>(*this)) return true;
        if (rfind("assets/", 0) == 0 && substr(7) == other) return true;
        return false;
    }

    bool operator==(const std::string& other) const {
        return *this == other.c_str();
    }
};

inline bool operator==(const char* lhs, const SheetPath& rhs) {
    return rhs == lhs;
}

inline bool operator==(const std::string& lhs, const SheetPath& rhs) {
    return rhs == lhs;
}

struct PlayerSheetChoice {
    SheetPath sheet;
    int row_offset = 0;
    int rowOffset = 0;
    std::string stage;
    std::string json;
};

inline PlayerSheetChoice playerSheetForFur(int furValue) {
    const int clamped = std::clamp(furValue, 0, 100);
    if (clamped < 20) {
        return {
            "assets/sprites/ow/grub.png",
            0,
            0,
            "gamer",
            "assets/sprites/ow/grub.json"
        };
    }
    if (clamped < 40) {
        return {
            "assets/sprites/ow/grub_fur.png",
            0,
            0,
            "ears",
            "assets/sprites/ow/grub_fur.json"
        };
    }
    if (clamped < 60) {
        return {
            "assets/sprites/ow/grub_fur.png",
            6,
            6,
            "paws",
            "assets/sprites/ow/grub_fur.json"
        };
    }
    if (clamped < 80) {
        return {
            "assets/sprites/ow/grub_fur.png",
            12,
            12,
            "tail",
            "assets/sprites/ow/grub_fur.json"
        };
    }
    return {
        "assets/sprites/ow/grub_fur.png",
        18,
        18,
        "snout",
        "assets/sprites/ow/grub_fur.json"
    };
}

inline PlayerSheetChoice playerSheet(int furValue) {
    return playerSheetForFur(furValue);
}

inline PlayerSheetChoice playerSheetChoice(int furValue) {
    return playerSheetForFur(furValue);
}

inline std::pair<std::string, int> playerSheetPair(int furValue) {
    const PlayerSheetChoice c = playerSheetForFur(furValue);
    return { static_cast<std::string>(c.sheet), c.row_offset };
}

struct PlayerRenderDef {
    SheetPath sheet;
    std::string json;
    std::string anim;
    int row_offset = 0;
    int rowOffset = 0;
    std::string stage;
};

inline PlayerRenderDef resolvePlayerRender(int furValue, const std::string& baseAnim) {
    if (furValue < 20) {
        return {
            "assets/sprites/ow/grub.png",
            "assets/sprites/ow/grub.json",
            baseAnim,
            0,
            0,
            "gamer"
        };
    }

    const PlayerSheetChoice choice = playerSheetForFur(furValue);

    // "lying/get_up/fall/stunned still come from grub.png when grub_fur has no such rows"
    // In grub_fur.json, fall and stunned have no rows in any stage.
    if (baseAnim == "fall" || baseAnim == "stunned") {
        return {
            "assets/sprites/ow/grub.png",
            "assets/sprites/ow/grub.json",
            baseAnim,
            0,
            0,
            choice.stage
        };
    }

    std::string resolvedAnim = baseAnim;
    if (baseAnim.rfind(choice.stage + "_", 0) != 0) {
        resolvedAnim = choice.stage + "_" + baseAnim;
    }

    return {
        choice.sheet,
        choice.json,
        resolvedAnim,
        choice.row_offset,
        choice.rowOffset,
        choice.stage
    };
}

} // namespace nundertale
