#pragma once

#include <raylib.h>
#include <string>

namespace nundertale {

enum class Area {
    Ruins,
    Snowdinha,
    Wetfall,
    Hotland,
    ConCenter,
    Barrier,
    Surface,
    FurryHud
};

constexpr Color hexColor(unsigned rgb) {
    return Color{
        static_cast<unsigned char>((rgb >> 16) & 0xFF),
        static_cast<unsigned char>((rgb >> 8) & 0xFF),
        static_cast<unsigned char>(rgb & 0xFF),
        255
    };
}

struct AreaPalette {
    const char* id;            // "ruins","snowdinha","wetfall","hotland","con_center","barrier","surface","furry_hud"
    Color base[2];   int baseCount;     // 1..2
    Color accent[3]; int accentCount;   // 0..3 (Snowdinha aurora = 3 stops; Wetfall = 2)
    Color highlight; bool hasHighlight; // false where the table says non-hex ("Multicolor stained glass", "White spinner" => use WHITE + true is NOT allowed; set false), "—"
    Color extra[3];  int extraCount;    // the hexes in the Notes column (Ruins banners/curtain #111111/CRT, Snowdinha wood/LAN, Hotland cables #222222/#444444); 0 if none
};

inline const AreaPalette& paletteFor(Area a) {
    static constexpr AreaPalette kPalettes[] = {
        // Ruins
        // Base: #3B1F5C · #5E3A87
        // Accent: #7CFF3A
        // Highlight: #F4E3FF
        // Notes / extra: Banners #C9A0DC; black 18+ curtain #111 (#111111); CRT #1E3FBF
        AreaPalette{
            "ruins",
            { hexColor(0x3B1F5C), hexColor(0x5E3A87) }, 2,
            { hexColor(0x7CFF3A), Color{0, 0, 0, 0}, Color{0, 0, 0, 0} }, 1,
            hexColor(0xF4E3FF), true,
            { hexColor(0xC9A0DC), hexColor(0x111111), hexColor(0x1E3FBF) }, 3
        },
        // Snowdinha
        // Base: #E8F0FF · #9BB3D9
        // Accent: Aurora #FF4FD8 -> #4FD8FF -> #7CFF3A (3 stops)
        // Highlight: #FFFFFF
        // Notes / extra: Wood #8A5A3C; LAN screen #2EE86B
        AreaPalette{
            "snowdinha",
            { hexColor(0xE8F0FF), hexColor(0x9BB3D9) }, 2,
            { hexColor(0xFF4FD8), hexColor(0x4FD8FF), hexColor(0x7CFF3A) }, 3,
            hexColor(0xFFFFFF), true,
            { hexColor(0x8A5A3C), hexColor(0x2EE86B), Color{0, 0, 0, 0} }, 2
        },
        // Wetfall
        // Base: #0E1A3A · #1F3B73
        // Accent: #3FF2FF · #FF8FC7 (2 stops)
        // Highlight: #B9FBFF
        // Notes: Falling drops, fursuit clotheslines in bright desaturated colors. (no hexes => 0)
        AreaPalette{
            "wetfall",
            { hexColor(0x0E1A3A), hexColor(0x1F3B73) }, 2,
            { hexColor(0x3FF2FF), hexColor(0xFF8FC7), Color{0, 0, 0, 0} }, 2,
            hexColor(0xB9FBFF), true,
            { Color{0, 0, 0, 0}, Color{0, 0, 0, 0}, Color{0, 0, 0, 0} }, 0
        },
        // Hotland
        // Base: #5A1A0A · #C2410C
        // Accent: #39FF14
        // Highlight: #FFB347
        // Notes / extra: Heat distortion; cables #222222/#444444
        AreaPalette{
            "hotland",
            { hexColor(0x5A1A0A), hexColor(0xC2410C) }, 2,
            { hexColor(0x39FF14), Color{0, 0, 0, 0}, Color{0, 0, 0, 0} }, 1,
            hexColor(0xFFB347), true,
            { hexColor(0x222222), hexColor(0x444444), Color{0, 0, 0, 0} }, 2
        },
        // Con Center
        // Base: #CFC4AE · #8F8775
        // Accent: #E6B422
        // Highlight: Multicolor stained glass (non-hex => false)
        // Notes: Deliberately desaturated. (no hexes => 0)
        AreaPalette{
            "con_center",
            { hexColor(0xCFC4AE), hexColor(0x8F8775) }, 2,
            { hexColor(0xE6B422), Color{0, 0, 0, 0}, Color{0, 0, 0, 0} }, 1,
            Color{0, 0, 0, 0}, false,
            { Color{0, 0, 0, 0}, Color{0, 0, 0, 0}, Color{0, 0, 0, 0} }, 0
        },
        // Barrier
        // Base: #1A1A0A (1 stop)
        // Accent: Fog #B8FF4A (1 stop)
        // Highlight: White spinner (non-hex => false)
        // Notes: Animated stink lines. (no hexes => 0)
        AreaPalette{
            "barrier",
            { hexColor(0x1A1A0A), Color{0, 0, 0, 0} }, 1,
            { hexColor(0xB8FF4A), Color{0, 0, 0, 0}, Color{0, 0, 0, 0} }, 1,
            Color{0, 0, 0, 0}, false,
            { Color{0, 0, 0, 0}, Color{0, 0, 0, 0}, Color{0, 0, 0, 0} }, 0
        },
        // Surface
        // Base: #2B2B3A · #FF9E5E
        // Accent: RGB #FF2E63 (1 stop)
        // Highlight: Monitor light #9FD3FF
        // Notes: Pepe's room: bottles, cans, chair. (no hexes => 0)
        AreaPalette{
            "surface",
            { hexColor(0x2B2B3A), hexColor(0xFF9E5E) }, 2,
            { hexColor(0xFF2E63), Color{0, 0, 0, 0}, Color{0, 0, 0, 0} }, 1,
            hexColor(0x9FD3FF), true,
            { Color{0, 0, 0, 0}, Color{0, 0, 0, 0}, Color{0, 0, 0, 0} }, 0
        },
        // FurryHud
        // Base: FLUFF bar #FF77C8 over #3A1030 (2 stops)
        // Accent: Numbers #FFB3E6 (1 stop)
        // Highlight: — (no highlight => false)
        // Notes: Mirrored bar (fills right to left). (no hexes => 0)
        AreaPalette{
            "furry_hud",
            { hexColor(0xFF77C8), hexColor(0x3A1030) }, 2,
            { hexColor(0xFFB3E6), Color{0, 0, 0, 0}, Color{0, 0, 0, 0} }, 1,
            Color{0, 0, 0, 0}, false,
            { Color{0, 0, 0, 0}, Color{0, 0, 0, 0}, Color{0, 0, 0, 0} }, 0
        }
    };

    return kPalettes[static_cast<size_t>(a)];
}

inline bool areaFromId(const std::string& id, Area& out) {
    if (id == "ruins") {
        out = Area::Ruins;
        return true;
    }
    if (id == "snowdinha") {
        out = Area::Snowdinha;
        return true;
    }
    if (id == "wetfall") {
        out = Area::Wetfall;
        return true;
    }
    if (id == "hotland") {
        out = Area::Hotland;
        return true;
    }
    if (id == "con_center") {
        out = Area::ConCenter;
        return true;
    }
    if (id == "barrier") {
        out = Area::Barrier;
        return true;
    }
    if (id == "surface") {
        out = Area::Surface;
        return true;
    }
    if (id == "furry_hud") {
        out = Area::FurryHud;
        return true;
    }
    return false;
}

} // namespace nundertale
