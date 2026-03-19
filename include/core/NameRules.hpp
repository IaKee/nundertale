#pragma once

#include <string>
#include <cstddef>

namespace nundertale {

enum class NameEffect { None, Gold, Skyrim, Waifu, Darken, FrogSoul, CrimsonShake, RestartGame };

struct NameReaction {
    std::string tag;       // e.g. "[GOLD]" or "" (shown in yellow under the name on the confirm screen)
    std::string message;   // flavor line, "" = default
    bool allowed = true;   // false => confirm screen only offers "Go back"
    NameEffect effect = NameEffect::None;
};

inline constexpr std::size_t NAME_MAX_LEN = 6;

inline std::string normalizeName(const std::string& raw) {
    std::string out = raw;
    for (char& c : out) {
        if (c >= 'a' && c <= 'z') {
            c = static_cast<char>(c - 'a' + 'A');
        }
    }
    return out;
}

inline NameReaction reactionFor(const std::string& rawName) {
    std::string name = normalizeName(rawName);

    // Table entries
    if (name == "CHAD") {
        return NameReaction{ "[GOLD]", "Gold plated. You are the ultimate gamer.", true, NameEffect::Gold };
    }
    if (name == "PEPE") {
        return NameReaction{ "[THE PROTAGONIST]", "Feels good man. Ready to climb down Mt. Dew.", true, NameEffect::FrogSoul };
    }
    if (name == "FROG") {
        return NameReaction{ "[RIBBIT]", "Feels froggy man.", true, NameEffect::FrogSoul };
    }
    if (name == "SKYRIM") {
        return NameReaction{ "[TAMRIEL]", "Hey you, you're finally awake...", true, NameEffect::Skyrim };
    }
    if (name == "SANS") {
        return NameReaction{ "[COMEDIAN]", "heh, you really think you have the backbone for this?", true, NameEffect::None };
    }
    if (name == "FRISK") {
        return NameReaction{ "[FALLEN CHILD]", "Warning: This mode triggers extreme allergy to fursuits.", true, NameEffect::None };
    }
    if (name == "CHARA") {
        return NameReaction{ "[THE FIRST]", "The true meaning of pure toxic gamer rage.", true, NameEffect::CrimsonShake };
    }
    if (name == "FLOWEY") {
        return NameReaction{ "[WEED]", "Not very original, are you?", true, NameEffect::None };
    }
    if (name == "TORIEL") {
        return NameReaction{ "[MOM]", "Furiell has already trademarked this name.", true, NameEffect::None };
    }
    if (name == "ASGORE") {
        return NameReaction{ "[KING]", "King of questionable herbal tea.", true, NameEffect::None };
    }
    if (name == "RUIVA" || name == "LOIRA" || name == "OTOME" ||
        name == "GOTICA" || name == "MORENA" || name == "YANDER") {
        return NameReaction{ "[WAIFU CONFIRMED]", "A true cultured gamer of fine 2D taste.", true, NameEffect::Waifu };
    }
    if (name == "FURIEL" || name == "FURIELL") {
        return NameReaction{ "", "Furiell is flattered. But no.", false, NameEffect::None };
    }
    if (name == "BARKY") {
        return NameReaction{ "", "AWOO! ...wait. That's MY name.", false, NameEffect::None };
    }
    if (name == "FLOOFY") {
        return NameReaction{ "", "No shame. Respect. ...Not allowed, though.", false, NameEffect::None };
    }
    if (name == "AZZY") {
        return NameReaction{ "", "...", true, NameEffect::Darken };
    }
    if (name == "YIFF") {
        return NameReaction{ "", "The narrator refuses.", false, NameEffect::None };
    }
    if (name == "OWO") {
        return NameReaction{ "", "What's this?", true, NameEffect::None };
    }
    if (name == "GAMER") {
        return NameReaction{ "[REDUNDANT]", "We know.", true, NameEffect::None };
    }
    if (name == "PRO") {
        return NameReaction{ "[PRO]", "Your K/D ratio is showing.", true, NameEffect::None };
    }
    if (name == "GASTER") {
        return NameReaction{ "", "", true, NameEffect::RestartGame };
    }

    // Generic rules after the table
    bool allSame = !name.empty();
    for (std::size_t i = 1; i < name.size(); ++i) {
        if (name[i] != name[0]) {
            allSame = false;
            break;
        }
    }
    if (allSame && name.size() > 1) {
        return NameReaction{ "", "Not very creative... Did your keyboard jam?", true, NameEffect::None };
    }
    if (name.size() == 1) {
        return NameReaction{ "", "You call that a gamer? Saving characters?", true, NameEffect::None };
    }

    return NameReaction{};
}

} // namespace nundertale
