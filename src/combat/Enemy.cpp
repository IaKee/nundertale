#include "combat/Enemy.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace nundertale {

Enemy::Enemy(const std::string& name, int maxHp, int atk, int def)
    : m_name(name), m_maxHp(maxHp), m_hp(maxHp), m_atk(atk), m_def(def),
      m_checkFlavor("Alpha Wolf of the Ruins. The zipper says otherwise.") {
    initDefaultActs();
    initDefaultFlavors();
    initDefaultDialogues();
}

void Enemy::takeDamage(int dmg) {
    m_hp -= dmg;
    if (m_hp < 0) m_hp = 0;
}

void Enemy::modifyAtk(int delta) {
    m_atk = clampStat(m_atk, delta);
}

void Enemy::modifyDef(int delta) {
    m_def = clampStat(m_def, delta);
}

void Enemy::addMercy(int amount) {
    m_mercy = std::min(100, m_mercy + amount);
    if (m_mercy >= m_mercyThreshold) {
        m_spareable = true;
    }
}

std::vector<std::string> Enemy::getActOptions() const {
    std::vector<std::string> options;
    for (const auto& a : m_acts) {
        options.push_back(a.label);
    }
    return options;
}

void Enemy::setActOptions(const std::vector<std::string>& opts) {
    m_acts.clear();
    for (const auto& opt : opts) {
        ActDef a;
        a.id = opt;
        a.label = opt;
        a.text = "* You checked " + opt;
        m_acts.push_back(a);
    }
}

std::string Enemy::getCheckText() const {
    std::string upper = m_name;
    for (char& ch : upper) {
        if (ch >= 'a' && ch <= 'z') ch = static_cast<char>(ch - 'a' + 'A');
    }
    return "* " + upper + " - ATK " + std::to_string(m_atk) + " DEF " + std::to_string(m_def) + "\n* " + m_checkFlavor;
}

// BFL-002: Turn flavor text based on status & player actions
std::string Enemy::getFlavorText(const std::string& lastActionFlag) {
    // 1. Check conditionals first
    if (lastActionFlag == "cringe_check") {
        return "* Barky is still furiously blushing over his exposed zipper.";
    }
    if (lastActionFlag == "debate_lore") {
        return "* Barky is still mentally trying to reconcile filler arc lore.";
    }

    // 2. Spareable pool
    if (m_spareable && !m_spareableFlavor.empty()) {
        int idx = GetRandomValue(0, (int)m_spareableFlavor.size() - 1);
        if (m_spareableFlavor.size() > 1 && idx == m_lastFlavorIndex) {
            idx = (idx + 1) % static_cast<int>(m_spareableFlavor.size());
        }
        m_lastFlavorIndex = idx;
        return m_spareableFlavor[idx];
    }

    // 3. Low HP pool (HP <= 35%)
    if (m_hp <= static_cast<int>(static_cast<float>(m_maxHp) * 0.35f) && !m_lowHpFlavor.empty()) {
        int idx = GetRandomValue(0, (int)m_lowHpFlavor.size() - 1);
        if (m_lowHpFlavor.size() > 1 && idx == m_lastFlavorIndex) {
            idx = (idx + 1) % static_cast<int>(m_lowHpFlavor.size());
        }
        m_lastFlavorIndex = idx;
        return m_lowHpFlavor[idx];
    }

    // 4. Default pool
    if (!m_defaultFlavor.empty()) {
        int idx = GetRandomValue(0, (int)m_defaultFlavor.size() - 1);
        if (m_defaultFlavor.size() > 1 && idx == m_lastFlavorIndex) {
            idx = (idx + 1) % static_cast<int>(m_defaultFlavor.size());
        }
        m_lastFlavorIndex = idx;
        return m_defaultFlavor[idx];
    }

    return "* " + m_name + " is standing ground.";
}

void Enemy::setFlavorPools(const std::vector<std::string>& def,
                           const std::vector<std::string>& lowHp,
                           const std::vector<std::string>& spareable) {
    m_defaultFlavor = def;
    m_lowHpFlavor = lowHp;
    m_spareableFlavor = spareable;
}

// BFL-003: Contextual dialogue based on triggers
std::string Enemy::getRandomDialogue() const {
    if (!m_dialoguesDefault.empty()) {
        int idx = GetRandomValue(0, (int)m_dialoguesDefault.size() - 1);
        return m_dialoguesDefault[idx];
    }
    return "...";
}

std::string Enemy::getDialogueForTrigger(const std::string& trigger, const std::string& actResponse) {
    if (!actResponse.empty()) {
        return actResponse;
    }
    if (trigger == "crit" && !m_dialoguesCrit.empty()) {
        return m_dialoguesCrit[GetRandomValue(0, (int)m_dialoguesCrit.size() - 1)];
    }
    if (trigger == "hit" && !m_dialoguesAfterHit.empty()) {
        return m_dialoguesAfterHit[GetRandomValue(0, (int)m_dialoguesAfterHit.size() - 1)];
    }
    if (trigger == "miss" && !m_dialoguesMiss.empty()) {
        return m_dialoguesMiss[GetRandomValue(0, (int)m_dialoguesMiss.size() - 1)];
    }
    if (trigger == "item" && !m_dialoguesItem.empty()) {
        return m_dialoguesItem[GetRandomValue(0, (int)m_dialoguesItem.size() - 1)];
    }
    return getRandomDialogue();
}

void Enemy::setDialogues(const std::vector<std::string>& lines) {
    m_dialoguesDefault = lines;
}

void Enemy::setDialogueTriggers(const std::vector<std::string>& def,
                                const std::vector<std::string>& afterHit,
                                const std::vector<std::string>& crit,
                                const std::vector<std::string>& miss,
                                const std::vector<std::string>& item) {
    m_dialoguesDefault = def;
    m_dialoguesAfterHit = afterHit;
    m_dialoguesCrit = crit;
    m_dialoguesMiss = miss;
    m_dialoguesItem = item;
}

void Enemy::initDefaultActs() {
    // Barky's ACTs, verbatim from LORE §19.1 (ACT-001, ACT-005, ACT-006)
    auto act = [](const char* id, const char* label, const char* text, const char* response) {
        ActDef a;
        a.id = id;
        a.label = label;
        a.text = text;
        a.response = response;
        return a;
    };

    ActDef check = act("check", "Check", "", "");

    ActDef cringe = act("cringe_check", "Cringe Check",
        "* You stare directly at the zipper on Barky's back.",
        "H-hey! It's an artisanal design feature, b-baka!");
    cringe.mercy = 50;
    cringe.repeatText = "* You stare again. Barky is out of blush.";
    cringe.repeatResponse = "...I KNOW, okay? I KNOW.";

    ActDef dew = act("offer_dew", "Offer Dew",
        "* You crack open an ice-cold can.",
        "Citrus Blast 2012?! ...wait, how do I know that flavor?");
    dew.spareable = true;

    ActDef lore = act("debate_lore", "Debate Lore",
        "* You recite incoherent mecha lore. Barky is bewildered.",
        "Season 4 episode 12 was CANON?!");
    lore.skipAttack = true;

    ActDef laser = act("laser", "Laser",
        "* You shine a red dot on the floor.",
        "NO-- I'M NOT-- WHERE DID IT GO--");
    laser.requiresItem = "Toy Laser Pointer";
    laser.spareable = true;
    laser.skipAttack = true;

    ActDef hug = act("accept_hug", "Accept Hug",
        "* Barky hugs you. He smells like a gym bag. You hug back anyway.",
        "...this is the best day of my life. Don't tell anyone.");
    hug.fur = 15;
    hug.spareNow = true;

    m_acts = { check, cringe, dew, lore, laser, hug };
}

void Enemy::initDefaultFlavors() {
    m_defaultFlavor = {
        "* Barky growls while adjusting his plastic wolf tail.",
        "* The scent of cheap synthetic fur fills the air.",
        "* Barky is practicing his menacing anime smirk.",
        "* Smells like cheap nylon and citrus soda."
    };
    m_lowHpFlavor = {
        "* Barky's ears are drooping. He looks tired.",
        "* Barky is panting and re-evaluating his life choices.",
        "* Barky's fursuit is visibly fraying at the seams."
    };
    m_spareableFlavor = {
        "* Barky is wagging his tail happily. He doesn't want to fight.",
        "* Barky seems open to joining your Discord server.",
        "* Barky looks ready to trade anime recommendation lists."
    };
}

void Enemy::initDefaultDialogues() {
    m_dialoguesDefault = {
        "Awoooo! Fear the shadow fang pack!",
        "You dare challenge an alpha gamer?",
        "Rawr x3 means 'delete your save file' in wolf!",
        "OwO what's this?!"
    };
    m_dialoguesAfterHit = {
        "Ouch! Watch the fursuit seams, man!",
        "Hey! That cost 800 dollars on Etsy!",
        "My synthetic fur is dry-clean only!"
    };
    m_dialoguesCrit = {
        "OOF! Critical hit to my gamer pride!",
        "B-baka! That actually hurt!"
    };
    m_dialoguesMiss = {
        "Haha! Too slow, casual!",
        "Can't touch this fursona!"
    };
    m_dialoguesItem = {
        "Wait, did you bring snacks?!",
        "The sacred citrus nectar of Mt. Dew?!"
    };
}

// Helper functions for lightweight JSON parsing
static std::string unescapeJson(const std::string& raw) {
    std::string out;
    out.reserve(raw.size());
    for (size_t i = 0; i < raw.size(); ++i) {
        if (raw[i] == '\\' && i + 1 < raw.size()) {
            char n = raw[++i];
            out += (n == 'n') ? '\n' : n;
        } else {
            out += raw[i];
        }
    }
    return out;
}

static std::string extractJsonString(const std::string& json, const std::string& key) {
    std::string needle = "\"" + key + "\":";
    size_t pos = json.find(needle);
    if (pos == std::string::npos) return "";
    pos += needle.size();
    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t' || json[pos] == '\n' || json[pos] == '\r')) pos++;
    if (pos >= json.size() || json[pos] != '\"') return "";
    pos++;
    size_t end = pos;
    while (end < json.size() && !(json[end] == '\"' && json[end - 1] != '\\')) end++;
    if (end >= json.size()) return "";
    return unescapeJson(json.substr(pos, end - pos));
}

static int extractJsonInt(const std::string& json, const std::string& key, int defaultVal = 0) {
    std::string needle = "\"" + key + "\":";
    size_t pos = json.find(needle);
    if (pos == std::string::npos) return defaultVal;
    pos += needle.size();
    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t' || json[pos] == '\n' || json[pos] == '\r')) pos++;
    size_t end = pos;
    while (end < json.size() && (std::isdigit(json[end]) || json[end] == '-')) end++;
    if (end > pos) {
        try {
            return std::stoi(json.substr(pos, end - pos));
        } catch (...) {}
    }
    return defaultVal;
}

static std::vector<std::string> extractStringArray(const std::string& json, const std::string& key) {
    std::vector<std::string> result;
    std::string needle = "\"" + key + "\":";
    size_t pos = json.find(needle);
    if (pos == std::string::npos) return result;
    size_t openBracket = json.find('[', pos);
    if (openBracket == std::string::npos) return result;
    size_t closeBracket = json.find(']', openBracket);
    if (closeBracket == std::string::npos) return result;

    std::string slice = json.substr(openBracket + 1, closeBracket - openBracket - 1);
    size_t cur = 0;
    while (cur < slice.size()) {
        size_t q1 = slice.find('\"', cur);
        if (q1 == std::string::npos) break;
        size_t q2 = slice.find('\"', q1 + 1);
        if (q2 == std::string::npos) break;
        result.push_back(unescapeJson(slice.substr(q1 + 1, q2 - q1 - 1)));
        cur = q2 + 1;
    }
    return result;
}

std::unique_ptr<Enemy> Enemy::loadFromJson(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        // Fallback to default Barky
        return std::make_unique<Enemy>("Barky", 30, 4, 2);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string json = buffer.str();

    std::string name = extractJsonString(json, "name");
    if (name.empty()) name = "Barky";
    int hp = extractJsonInt(json, "hp", 30);
    int maxHp = extractJsonInt(json, "maxHp", hp);
    int atk = extractJsonInt(json, "atk", 4);
    int def = extractJsonInt(json, "def", 2);

    auto enemy = std::make_unique<Enemy>(name, maxHp, atk, def);
    enemy->m_exp = extractJsonInt(json, "exp", 20);
    enemy->m_gold = extractJsonInt(json, "gold", 10);
    enemy->setMercyLockedAbovePct(extractJsonInt(json, "mercyLockedAbovePct", 0));

    std::string checkStr = extractJsonString(json, "check");
    if (!checkStr.empty()) {
        enemy->setCheckFlavor(checkStr);
    }

    // Extract flavor pools (BFL-002)
    size_t flavorPos = json.find("\"flavor\":");
    if (flavorPos != std::string::npos) {
        size_t flavorEnd = json.find('}', json.find('{', flavorPos));
        if (flavorEnd != std::string::npos) {
            std::string flavorSlice = json.substr(flavorPos, flavorEnd - flavorPos + 1);
            std::vector<std::string> defFlavors = extractStringArray(flavorSlice, "default");
            std::vector<std::string> lowHpFlavors = extractStringArray(flavorSlice, "lowHp");
            std::vector<std::string> spareFlavors = extractStringArray(flavorSlice, "spareable");
            if (!defFlavors.empty()) enemy->m_defaultFlavor = defFlavors;
            if (!lowHpFlavors.empty()) enemy->m_lowHpFlavor = lowHpFlavors;
            if (!spareFlavors.empty()) enemy->m_spareableFlavor = spareFlavors;
        }
    }

    // Extract dialogue pools (BFL-003)
    size_t diagPos = json.find("\"dialogue\":");
    if (diagPos != std::string::npos) {
        size_t diagEnd = json.find('}', json.find('{', diagPos));
        if (diagEnd != std::string::npos) {
            std::string diagSlice = json.substr(diagPos, diagEnd - diagPos + 1);
            std::vector<std::string> defDiags = extractStringArray(diagSlice, "default");
            std::vector<std::string> hitDiags = extractStringArray(diagSlice, "afterHit");
            if (!defDiags.empty()) enemy->m_dialoguesDefault = defDiags;
            if (!hitDiags.empty()) enemy->m_dialoguesAfterHit = hitDiags;
        }
    }

    return enemy;
}

} // namespace nundertale
