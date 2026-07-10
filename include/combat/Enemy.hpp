#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <functional>
#include <raylib.h>
#include "ActGate.hpp"
#include "EnemyPatterns.hpp"
#include "FloofyScript.hpp"
#include "GladosPatterns.hpp"
#include "core/PeacePath.hpp"

namespace nundertale {

inline bool jsonIntForKey(const std::string& json, const std::string& key, int& value) {
    const std::string needle = "\"" + key + "\"";
    std::size_t pos = json.find(needle);
    if (pos == std::string::npos) return false;
    pos = json.find(':', pos + needle.size());
    if (pos == std::string::npos) return false;
    ++pos;
    while (pos < json.size() && std::isspace(static_cast<unsigned char>(json[pos])) != 0) ++pos;
    bool negative = false;
    if (pos < json.size() && json[pos] == '-') {
        negative = true;
        ++pos;
    }
    if (pos >= json.size() || std::isdigit(static_cast<unsigned char>(json[pos])) == 0) return false;
    int parsed = 0;
    while (pos < json.size() && std::isdigit(static_cast<unsigned char>(json[pos])) != 0) {
        parsed = parsed * 10 + (json[pos] - '0');
        ++pos;
    }
    value = negative ? -parsed : parsed;
    return true;
}

inline std::string jsonObjectAt(const std::string& json, std::size_t openPos) {
    if (openPos >= json.size() || json[openPos] != '{') return {};
    int depth = 0;
    bool inString = false;
    bool escaped = false;
    for (std::size_t pos = openPos; pos < json.size(); ++pos) {
        const char ch = json[pos];
        if (inString) {
            if (escaped) escaped = false;
            else if (ch == '\\') escaped = true;
            else if (ch == '"') inString = false;
            continue;
        }
        if (ch == '"') inString = true;
        else if (ch == '{') ++depth;
        else if (ch == '}' && --depth == 0) return json.substr(openPos, pos - openPos + 1);
    }
    return {};
}

inline int patternFurFromJson(const std::string& json, const std::string& patternId, int fallback) {
    const std::size_t patternsPos = json.find("\"patterns\"");
    if (patternsPos == std::string::npos) return fallback;
    std::size_t searchPos = patternsPos;
    while ((searchPos = json.find("\"id\"", searchPos)) != std::string::npos) {
        const std::size_t colon = json.find(':', searchPos + 4);
        const std::size_t quote = colon == std::string::npos ? std::string::npos : json.find('"', colon + 1);
        const std::size_t endQuote = quote == std::string::npos ? std::string::npos : json.find('"', quote + 1);
        if (endQuote == std::string::npos) return fallback;
        if (json.substr(quote + 1, endQuote - quote - 1) == patternId) {
            const std::size_t objectStart = json.rfind('{', searchPos);
            const std::string patternObject = jsonObjectAt(json, objectStart);
            int value = 0;
            if (jsonIntForKey(patternObject, "furry", value)) return value;
            if (jsonIntForKey(patternObject, "fur", value)) return value;
            return fallback;
        }
        searchPos = endQuote + 1;
    }
    return fallback;
}

inline int howlFurFromJson(const std::string& json, int fallback) {
    int value = 0;
    return jsonIntForKey(json, "howlFur", value) ? value : fallback;
}

class Enemy {
public:
    Enemy(const std::string& name, int maxHp, int atk, int def);

    std::string getName() const { return m_name; }
    const std::string& getId() const { return m_id; }
    const std::string& getSprite() const { return m_sprite; }
    int getHp() const { return m_hp; }
    int getMaxHp() const { return m_maxHp; }
    int getAtk() const { return m_atk; }
    int getDef() const { return m_def; }
    int getExp() const { return m_exp; }
    int getGold() const { return m_gold; }
    const std::string& getSpareText() const { return m_spareText; }
    const std::string& getDeathText() const { return m_deathText; }
    // C1-022: repeated-spare lines (Furiell), spared narrator outro, death lines and outcome flags
    const std::vector<std::string>& getSpareLines() const { return m_spareLines; }
    std::size_t getSpareLinesShown() const { return m_spareLinesShown; }
    void setSpareLinesShown(std::size_t shown) { m_spareLinesShown = shown; }
    const std::string& getSpareOutro() const { return m_spareOutro; }
    const std::vector<std::string>& getDeathLines() const { return m_deathLines; }
    const std::string& getSpareFlag() const { return m_spareFlag; }
    const std::string& getKillFlag() const { return m_killFlag; }
    // BHL-020: bullets can't hurt a soul at HP <= 4 and steer away from it
    bool hasMercifulBullets() const { return m_mercifulBullets; }
    int getMercy() const { return m_mercy; }
    bool isDead() const { return m_hp <= 0; }
    bool isSpareable() const { return m_spareable; }
    bool canBeSpared() const { return m_canBeSpared; }
    bool isInvulnerable() const { return m_invulnerable; }
    bool isUnkillable() const { return m_unkillable; }
    bool countsAsKill() const { return m_countsAsKill; }
    void setSpareable(bool s) { m_spareable = s && m_canBeSpared; }
    static bool mercyLockedAt(int hp, int maxHp, int lockPct) {
        if (maxHp <= 0 || lockPct <= 0) return false;
        return static_cast<long long>(hp) * 100 > static_cast<long long>(maxHp) * lockPct;
    }
    bool isMercyLocked() const { return mercyLockedAt(m_hp, m_maxHp, m_mercyLockedAbovePct); }
    void setMercyLockedAbovePct(int pct) { m_mercyLockedAbovePct = pct; }

    void addMercy(int amount);
    void takeDamage(int dmg);
    void modifyAtk(int delta);
    void modifyDef(int delta);

    int getPatternFur(const std::string& patternId, int fallback) const {
        const auto it = m_patternFur.find(patternId);
        return it != m_patternFur.end() ? it->second : fallback;
    }
    int getHowlFur(int fallback) const { return m_howlFur >= 0 ? m_howlFur : fallback; }
    const std::vector<EnemyPatternDef>& getPatterns() const { return m_patterns; }
    int getSurviveTurns() const { return m_surviveTurns; }
    const std::string& getSurvivalEndText() const { return m_survivalEndText; }
    const std::vector<GladosPhaseDef>& getSurvivalPhases() const { return m_survivalPhases; }
    const ScriptedBattleEnd& getScriptedEnd() const { return m_scriptedEnd; }
    int getScriptedEndAfterTurns() const { return m_scriptedEnd.afterEnemyTurns; }
    const std::string& getTurnOffsetFlag() const { return m_turnOffsetFlag; }
    const std::string& getScriptedEndText() const { return m_scriptedEndText; }
    const std::string& getInvulnerableHitText() const { return m_invulnerableHitText; }
    const std::vector<std::string>& getInvulnerableTexts() const { return m_invulnerableTexts; }
    const std::string& getInvulnerableBubble() const { return m_invulnerableBubble; }
    const std::string& getFleeBlockedText() const { return m_fleeBlockedText; }
    int getFurCap() const { return m_furCap; }
    bool suppressesFurMirror() const { return m_suppressFurMirror; }
    const std::string& getPatternSpeech(const std::string& patternId) const {
        static const std::string empty;
        const auto it = m_patternSpeech.find(patternId);
        return it != m_patternSpeech.end() ? it->second : empty;
    }
    const std::string& getPatternSprite(const std::string& patternId) const {
        static const std::string empty;
        const auto it = m_patternSprite.find(patternId);
        return it != m_patternSprite.end() ? it->second : empty;
    }
    int getActHeal(const std::string& actId) const {
        const auto it = m_actHeal.find(actId);
        return it != m_actHeal.end() ? it->second : 0;
    }

    const std::vector<ActDef>& getActs() const { return m_acts; }
    void setActs(const std::vector<ActDef>& acts) { m_acts = acts; rebuildMenuActs(); }
    void addAct(const ActDef& act) { m_acts.push_back(act); rebuildMenuActs(); }
    // ACT menu order: getActs() plus the peace path's extra ACTs (Barky's Comment on Tail) after Check.
    const std::vector<ActDef>& getMenuActs() const { return m_menuActs; }

    // ACT-008: data-driven peace path (enemy JSON "peacePath"); no block = old mercy/spareable behaviour.
    bool hasPeacePath() const { return m_hasPeacePath; }
    const PeacePathDef& getPeacePath() const { return m_peacePath; }
    const PeacePathState& getPeaceState() const { return m_peaceState; }
    void setPeacePath(const PeacePathDef& def, const std::vector<ActDef>& extraActs = {});
    // Applies the ACT (by its menu label) to the path; the enemy turns spareable on the step that reaches the goal.
    PeaceStepResult applyPeaceAct(const std::string& actLabel, bool toxicTrack);
    // Peace-path ACTs with requiresProgress stay hidden until the required number of steps is complete.
    bool peaceActAvailable(const std::string& actLabel) const;
    bool peaceMercyAllowed(bool toxicTrack) const;
    int peaceStepsLeft(bool toxicTrack) const;

    std::vector<std::string> getActOptions() const;
    void setActOptions(const std::vector<std::string>& opts);

    std::string getCheckFlavor() const { return m_checkFlavor; }
    void setCheckFlavor(const std::string& flavor) { m_checkFlavor = flavor; }
    std::string getCheckText() const;

    // BFL-002: Turn flavor text based on status & player actions
    std::string getFlavorText(const std::string& lastActionFlag = "");
    void setFlavorPools(const std::vector<std::string>& def,
                        const std::vector<std::string>& lowHp,
                        const std::vector<std::string>& spareable);

    // BFL-003: Contextual dialogue based on triggers
    std::string getRandomDialogue() const;
    std::string getDialogueForTrigger(const std::string& trigger, const std::string& actResponse = "");
    void setDialogues(const std::vector<std::string>& lines);
    void setDialogueTriggers(const std::vector<std::string>& def,
                             const std::vector<std::string>& afterHit,
                             const std::vector<std::string>& crit,
                             const std::vector<std::string>& miss,
                             const std::vector<std::string>& item);

    // BFL-005 & BFL-006: Data-driven enemy loader
    static std::unique_ptr<Enemy> loadFromJson(const std::string& filepath);

private:
    void rebuildMenuActs();
    void initDefaultActs();
    void initDefaultFlavors();
    void initDefaultDialogues();

    std::string m_name;
    std::string m_id = "barky";
    std::string m_sprite = "barky";
    int m_maxHp;
    int m_hp;
    int m_atk;
    int m_def;
    int m_exp = 20;
    int m_gold = 10;
    int m_mercy = 0;
    int m_mercyThreshold = 100;
    int m_mercyLockedAbovePct = 0;
    std::string m_checkFlavor;
    bool m_spareable = false;
    bool m_canBeSpared = true;
    bool m_invulnerable = false;
    bool m_unkillable = false;
    bool m_countsAsKill = true;
    int m_surviveTurns = 0;
    std::string m_survivalEndText;
    std::vector<GladosPhaseDef> m_survivalPhases;
    ScriptedBattleEnd m_scriptedEnd;
    std::string m_turnOffsetFlag;
    std::string m_scriptedEndText;
    std::string m_invulnerableHitText;
    std::vector<std::string> m_invulnerableTexts;
    std::string m_invulnerableBubble;
    std::string m_fleeBlockedText;
    int m_furCap = -1;
    bool m_suppressFurMirror = false;

    std::vector<ActDef> m_acts;
    std::vector<ActDef> m_peaceExtraActs;
    std::vector<ActDef> m_menuActs;
    PeacePathDef m_peacePath;
    PeacePathState m_peaceState;
    bool m_hasPeacePath = false;
    bool m_peaceToxic = false;
    std::unordered_map<std::string, int> m_patternFur;
    std::unordered_map<std::string, std::string> m_patternSpeech;
    std::unordered_map<std::string, std::string> m_patternSprite;
    std::unordered_map<std::string, int> m_actHeal;
    int m_howlFur = -1;
    std::vector<EnemyPatternDef> m_patterns;
    std::string m_spareText = "You can tell no one about the zipper. Pack secret.";
    std::string m_deathText = "* Only a pile of synthetic wolf fur remains.";
    std::vector<std::string> m_spareLines;
    std::size_t m_spareLinesShown = 0;
    std::string m_spareOutro;
    std::vector<std::string> m_deathLines;
    std::string m_spareFlag;
    std::string m_killFlag;
    bool m_mercifulBullets = false;

    // Flavor pools (BFL-002)
    std::vector<std::string> m_defaultFlavor;
    std::vector<std::string> m_lowHpFlavor;
    std::vector<std::string> m_spareableFlavor;
    int m_lastFlavorIndex = -1;

    // Dialogue pools (BFL-003)
    std::vector<std::string> m_dialoguesDefault;
    std::vector<std::string> m_dialoguesAfterHit;
    std::vector<std::string> m_dialoguesCrit;
    std::vector<std::string> m_dialoguesMiss;
    std::vector<std::string> m_dialoguesItem;
};

} // namespace nundertale
