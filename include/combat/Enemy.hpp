#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <functional>
#include <raylib.h>

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

struct ActDef {
    std::string id;
    std::string label;
    std::string text;
    std::string response;
    bool spareable = false;
    bool skipAttack = false;
    int mercy = 0;
    std::string requiresItem;
    int requiresFurMin = 0;
    std::string requiresFlag;
    int minUses = 1;
    int atkDelta = 0;
    int defDelta = 0;
    int freezeTurns = 0;
    std::string repeatText;     // shown from the 2nd use on (e.g. Cringe Check), "" = reuse text
    std::string repeatResponse; // enemy line from the 2nd use on, "" = reuse response
    int fur = 0;                // FUR added when the effect applies (ACT-005 Accept Hug)
    bool spareNow = false;      // spares on the spot, without MERCY (ACT-005)
};

// Text and enemy line for the Nth use of an ACT (1-based).
inline const std::string& actTextForUse(const ActDef& act, int useCount) {
    return (useCount >= 2 && !act.repeatText.empty()) ? act.repeatText : act.text;
}
inline const std::string& actResponseForUse(const ActDef& act, int useCount) {
    return (useCount >= 2 && !act.repeatResponse.empty()) ? act.repeatResponse : act.response;
}

inline bool actAvailable(const ActDef& act,
                         const std::vector<std::string>& inventory,
                         int fur,
                         const std::function<bool(const std::string&)>& hasFlag) {
    if (!act.requiresItem.empty() &&
        std::find(inventory.begin(), inventory.end(), act.requiresItem) == inventory.end()) {
        return false;
    }
    if (fur < act.requiresFurMin) {
        return false;
    }
    return act.requiresFlag.empty() || hasFlag(act.requiresFlag);
}

inline bool actEffectApplies(int useCount, int minUses) {
    return minUses <= 1 || useCount >= minUses;
}

inline int clampStat(int value, int delta) {
    return std::max(0, value + delta);
}

class Enemy {
public:
    Enemy(const std::string& name, int maxHp, int atk, int def);

    std::string getName() const { return m_name; }
    int getHp() const { return m_hp; }
    int getMaxHp() const { return m_maxHp; }
    int getAtk() const { return m_atk; }
    int getDef() const { return m_def; }
    int getExp() const { return m_exp; }
    int getGold() const { return m_gold; }
    int getMercy() const { return m_mercy; }
    bool isDead() const { return m_hp <= 0; }
    bool isSpareable() const { return m_spareable; }
    void setSpareable(bool s) { m_spareable = s; }
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

    const std::vector<ActDef>& getActs() const { return m_acts; }
    void setActs(const std::vector<ActDef>& acts) { m_acts = acts; }
    void addAct(const ActDef& act) { m_acts.push_back(act); }

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
    void initDefaultActs();
    void initDefaultFlavors();
    void initDefaultDialogues();

    std::string m_name;
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

    std::vector<ActDef> m_acts;
    std::unordered_map<std::string, int> m_patternFur;
    int m_howlFur = -1;

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
