#pragma once

#include <functional>
#include <string>

namespace nundertale {

inline const char* DEFAULT_BATTLE_MUSIC = "assets/music/epic_background_loop.ogg";

inline std::string musicPathForId(const std::string& id) {
    if (id.empty() || id == "battle_default") {
        return DEFAULT_BATTLE_MUSIC;
    }
    return "assets/music/" + id + ".ogg";
}

inline std::string resolveMusicPath(
    const std::string& id,
    const std::function<bool(const std::string&)>& exists) {
    const std::string candidate = musicPathForId(id);
    if (!exists(candidate)) {
        return DEFAULT_BATTLE_MUSIC;
    }
    return candidate;
}

} // namespace nundertale
