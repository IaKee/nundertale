#pragma once

#include "combat/BeatMap.hpp"
#include "combat/Enemy.hpp"
#include "combat/FloofyScript.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace ru36_detail {

inline std::string readFile(const std::string& path) {
    std::ifstream file(path);
    assert(file.is_open());
    std::ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

} // namespace ru36_detail

// C1-028: The Bass Shake moves to Floofy (floofy_bass enemy/encounter, beat map, thorn petals)
inline void suite_ru36() {
    using namespace nundertale;

    // 1. The enemy follows the petal fight's rules.
    const auto floofy = Enemy::loadFromJson("data/enemies/floofy_bass.json");
    assert(floofy != nullptr);
    assert(floofy->getId() == "floofy_bass" && floofy->getName() == "Floofy");
    assert(floofy->isInvulnerable() && !floofy->canBeSpared());
    assert(floofy->getScriptedEndAfterTurns() == 6);
    assert(floofy->getScriptedEnd().interruptOnTurnStart);
    assert(floofy->getScriptedEnd().endCutscene == "r2_floofy_end");
    assert(floofy->getTurnOffsetFlag() == "ruins.floofy.deaths");
    assert(floofy->getFleeBlockedText() == "* The petals block the exit.");
    assert(floofy->getFurCap() == 99 && floofy->suppressesFurMirror());
    assert(joinBattleText(floofy->getInvulnerableTexts()) ==
           "* You hit him. Your fist sinks into the fursuit padding.\n* It's too soft to hurt.");
    assert(floofy->getInvulnerableBubble() == "Aww~ was that a hug?");
    assert(battleMercyOptions(floofy->canBeSpared(), false, !floofy->getFleeBlockedText().empty()).size() == 1U);

    // the dummy-only ACTs are gone: Check, Plead, Compliment the Crown
    std::vector<std::string> actIds;
    for (const ActDef& act : floofy->getActs()) actIds.push_back(act.id);
    assert(std::find(actIds.begin(), actIds.end(), "check") != actIds.end());
    assert(std::find(actIds.begin(), actIds.end(), "plead") != actIds.end());
    assert(std::find(actIds.begin(), actIds.end(), "compliment_crown") != actIds.end());
    assert(std::find(actIds.begin(), actIds.end(), "shake") == actIds.end());
    assert(std::find(actIds.begin(), actIds.end(), "turn_down") == actIds.end());
    for (const ActDef& act : floofy->getActs()) {
        assert(!act.stopsBeat);
        assert(!act.spareable && !act.spareNow);
    }

    // the only pattern is the beat pattern on the Floofy beat map
    assert(floofy->getPatterns().size() == 1U);
    const EnemyPatternDef& pattern = floofy->getPatterns().front();
    assert(pattern.kind == EnemyPatternKind::Beat && pattern.beatmap == "floofy_bass");
    assert(pattern.furry > 0 && pattern.damage == 1);

    // 2. The encounter: opener, black background, the bass track, no flee flag.
    const std::string encounter = ru36_detail::readFile("data/encounters/floofy_bass.json");
    assert(encounter.find("\"enemies\": [\"floofy_bass\"]") != std::string::npos);
    assert(encounter.find("...DANCE.") != std::string::npos);
    assert(encounter.find("\"background\": \"black\"") != std::string::npos);
    assert(encounter.find("\"music\": \"battle.bass_shake\"") != std::string::npos);
    assert(encounter.find("\"canFlee\": false") != std::string::npos);

    // 3. The beat map: the meme loop x3, one burst per accelerated part.
    BeatMap map;
    assert(loadBeatMap("data/beatmaps/floofy_bass.json", map));
    assert(map.track == "battle.bass_shake");
    assert(map.notes.size() == 10U && map.shots.size() == 2U && map.passesPerTurn == 3);
    assert(map.expressions.size() == 4U);
    const double quarter = 60.0 / 130.0;
    assert(std::fabs(map.loopLength - 8.0 * quarter) < 1e-12);
    assert(std::fabs(map.turnEndsAt - 3.0 * quarter) < 1e-12);
    assert(std::fabs(beatTurnDuration(map) - (2.0 * 8.0 * quarter + 3.0 * quarter)) < 1e-9);

    // 30 petals per turn, none in the wide-shot gaps
    const auto all = beatEventsBetween(map, initialBeatEventCursor(map), beatTurnDuration(map) + 1.0);
    assert(all.size() == 30U);
    for (const BeatEvent& event : all) {
        assert(beatShotAt(map, event.turnTime) == "closeup");
    }
    for (int pass = 0; pass < map.passesPerTurn; ++pass) {
        const double start = static_cast<double>(pass) * map.loopLength;
        const auto burst = beatEventsBetween(map, start - 1e-6, start + 1.384);
        assert(burst.size() == 10U);
        const auto gap = beatEventsBetween(map, start + 1.385, start + map.loopLength - 1e-6);
        assert(gap.empty());
    }
    assert(beatShotAt(map, 0.0) == "closeup");
    assert(beatShotAt(map, 3.0 * quarter - 1e-6) == "closeup");
    assert(beatShotAt(map, 3.0 * quarter) == "wide");
    assert(beatShotAt(map, map.loopLength - 1e-6) == "wide");
    assert(beatShotAt(map, map.loopLength) == "closeup");
    // no menu-turn bullets: the menu loop is the calm bar, entirely wide
    assert(beatShotAt(map, map.menuLoopStart) == "wide" && beatShotAt(map, map.menuLoopEnd - 1e-6) == "wide");

    // 4. Thorn petals instead of fluff tufts, harder every turn.
    assert(map.bulletSprite == "assets/sprites/bullets/thorn_petal.png");
    assert(map.bulletSpriteBig == "assets/sprites/bullets/thorn_petal.png");
    // (the PNG itself lands with ART-183; until then the engine draws a circle)
    const int soulDefense = 0 + 1; // Player::df + the starting armor's armorDf
    const int small = beatBulletDamage(map, pattern.damage, false) + floofy->getAtk() - soulDefense;
    const int big = beatBulletDamage(map, pattern.damage, true) + floofy->getAtk() - soulDefense;
    assert(small == 3 && big == 4); // "3-4 damage per hit against 20 HP"
    assert(beatBulletSpeed(map, 110.0f, 1) == 110.0f);
    float previous = 0.0f;
    for (int turn = 1; turn <= 6; ++turn) {
        const float speed = beatBulletSpeed(map, 110.0f, turn);
        assert(speed > previous);
        previous = speed;
    }
    assert(beatBulletSpeed(map, 110.0f, 6) < 110.0f * 1.5f);

    // 5. The close-up stand-in exists, and the wide shot steps once per beat.
    assert(map.closeupSprite == "fursuit_dummy_close");
    assert(std::filesystem::exists("assets/sprites/battle/" + map.closeupSprite + ".png"));
    assert(std::filesystem::exists("assets/sprites/battle/" + map.closeupSprite + ".json"));
    assert(map.wideStepSixteenths == 4);
    assert(beatVisualStateAt(map, 0, 4.0 * quarter, 2).wideFrame == 0);
    assert(beatVisualStateAt(map, 0, 5.0 * quarter, 2).wideFrame == 1);
    assert(beatVisualStateAt(map, 0, 6.0 * quarter, 2).wideFrame == 0);
    assert(beatVisualStateAt(map, 0, 5.0 * quarter + 0.2, 2).wideFrame == 1);

    // 6. The finale: Furiell's spray lands on a drop and the record scratch stops the track.
    assert(map.finale.enabled && map.finale.pass == 2 && map.finale.time == 0.0);
    assert(std::fabs(beatFinaleTime(map) - 2.0 * map.loopLength) < 1e-9);
    assert(beatFinaleTime(map) < beatTurnDuration(map));
    assert(beatShotAt(map, beatFinaleTime(map)) == "closeup"); // a drop = the cut into the close-up
    assert(!beatFinaleReached(map, beatFinaleTime(map) - 0.01));
    assert(beatFinaleReached(map, beatFinaleTime(map)));
    assert(beatFinaleReached(map, beatFinaleTime(map) + 0.5));
    assert(map.finale.sfx.size() == 2U);
    assert(map.finale.sfx[0] == "assets/sfx/spray.ogg");
    assert(map.finale.sfx[1] == "assets/sfx/record_scratch.ogg");
    // before the drop the final turn still fires two full bursts
    assert(beatEventsBetween(map, initialBeatEventCursor(map), beatFinaleTime(map) - 1e-6).size() == 20U);
    // the interruption turn comes from the scripted end (turn 6, earlier after 3+ deaths)
    assert(scriptedBattleEndsOnTurnStart(6, effectiveScriptedEndTurn(6, 0), true));
    assert(!scriptedBattleEndsOnTurnStart(5, effectiveScriptedEndTurn(6, 0), true));
    assert(scriptedBattleEndsOnTurnStart(5, effectiveScriptedEndTurn(6, 3), true));

    // 7. Map-level validation of the new fields.
    BeatMap bad;
    const std::string base = ru36_detail::readFile("data/beatmaps/floofy_bass.json");
    assert(parseBeatMap(base, bad));
    std::string badFinale = base;
    badFinale.replace(badFinale.find("\"pass\": 2"), 9U, "\"pass\": 3");
    assert(!parseBeatMap(badFinale, bad)); // pass 3 does not exist in a 3-pass turn
    std::string badStep = base;
    badStep.replace(badStep.find("\"stepSixteenths\": 4"), 19U, "\"stepSixteenths\": 0");
    assert(!parseBeatMap(badStep, bad));

    // 8. The dummy's map still reads as before (BSK suites): fluff tufts, no finale.
    BeatMap dummy;
    assert(loadBeatMap("data/beatmaps/bass_shake.json", dummy));
    assert(dummy.notes == map.notes && dummy.kicks == map.kicks && dummy.bigNotes == map.bigNotes);
    assert(dummy.loopLength == map.loopLength && dummy.turnEndsAt == map.turnEndsAt);
    assert(dummy.shots.size() == map.shots.size());
    assert(dummy.bulletSprite.empty() && !dummy.finale.enabled && dummy.closeupSprite.empty());
    assert(dummy.wideStepSixteenths == 1 && dummy.bigDamageBonus == 0 && dummy.speedPerTurn == 0.0);
    assert(beatBulletDamage(dummy, 1, true) == 1);
    assert(beatBulletSpeed(dummy, 110.0f, 6) == 110.0f);
    assert(beatFinaleTime(dummy) < 0.0 && !beatFinaleReached(dummy, 100.0));

    // 9. The old spec path is gone; the R2 spec carries the measurements.
    assert(!std::filesystem::exists("docs/fights/R5_fursuit_dummy_bass_shake.md"));
    const std::string spec = ru36_detail::readFile("docs/fights/R2_floofy_bass_shake.md");
    assert(spec.find("3.692") != std::string::npos && spec.find("130 BPM") != std::string::npos);
    assert(spec.find("0.081 · 0.197 · 0.313 · 0.424") != std::string::npos);

    std::cout << "[PASS] C1-028: floofy_bass is invulnerable with no Spare, six turns ending on the finale drop, "
                 "thorn-petal bursts on the beat map notes, harder per turn, dummy map unchanged."
              << std::endl;
}
