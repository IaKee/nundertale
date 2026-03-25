#pragma once

#include <string>
#include <vector>

namespace nundertale {

inline std::string savePlaceLine(const std::string& roomId) {
    if (roomId == "ruins_entrance" || roomId.empty()) {
        return "* The scent of lukewarm energy drinks and stale Doritos fills you with DETERMINATION.";
    }
    if (roomId == "ruins_hall") {
        return "* The distant hum of dial-up internet fills you with DETERMINATION.";
    }
    if (roomId == "ruins_exit") {
        return "* The cold air and the faint smell of ozone fill you with DETERMINATION.";
    }
    if (roomId == "snowdin_entrance") {
        return "* The crunch of styrofoam snow fills you with DETERMINATION.";
    }
    if (roomId == "snowdin_town") {
        return "* The sight of 47 people sharing one Wi-Fi password on a fridge fills you with DETERMINATION.";
    }
    if (roomId == "snowdin_fog") {
        return "* The hum of the town's single shared router fills you with DETERMINATION.";
    }
    if (roomId == "waterfall_entrance") {
        return "* The sound of dripping and a distant \"rawr\" fill you with DETERMINATION.";
    }
    if (roomId == "waterfall_arena") {
        return "* The humidity of a thousand fursuits fills you with DETERMINATION.";
    }
    if (roomId == "hotland_quiz") {
        return "* The sound of 400 server fans fills you with DETERMINATION.";
    }
    if (roomId == "newhome_throne") {
        return "* The smell of herbal tea and old carpet fills you with DETERMINATION.";
    }
    if (roomId == "con_center_hall") {
        return "* The terrified hugging of a hundred furries fills you with DETERMINATION.";
    }
    return "* The scent of lukewarm energy drinks and stale Doritos fills you with DETERMINATION.";
}

inline std::vector<std::string> saveLines(const std::string& roomId, int furValue, bool toxicRoute) {
    if (toxicRoute) {
        return { "* Determination." };
    }
    std::vector<std::string> lines;
    lines.push_back(savePlaceLine(roomId));
    if (furValue > 0) {
        lines.push_back("* DETERMINATION can't fix fluff.");
    }
    return lines;
}

} // namespace nundertale
