#include "combat/Enemy.hpp"
#include "combat/EnemyLoader.hpp"
#include "core/FurTable.hpp"
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

// Enemy speech bubble
std::string Enemy::getRandomBubble() const {
    if (!m_bubbles.empty()) {
        int idx = GetRandomValue(0, static_cast<int>(m_bubbles.size()) - 1);
        return m_bubbles[static_cast<size_t>(idx)];
    }
    return getRandomDialogue();
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
    hug.fur = globalFurTable().get("accept_hug", 15);
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

std::unique_ptr<Enemy> Enemy::loadFromJson(const std::string& filepath) {
    std::ifstream testFile(filepath);
    if (!testFile.is_open()) {
        // Fallback to default Barky
        return std::make_unique<Enemy>("Barky", 30, 4, 2);
    }
    testFile.close();

    EnemyData data = loadEnemyDataFromFile(filepath);
    auto enemy = std::make_unique<Enemy>(data.name, data.maxHp, data.atk, data.def);
    enemy->m_id = data.id;
    enemy->m_sprite = data.sprite;
    enemy->m_exp = data.exp;
    enemy->m_gold = data.gold;
    enemy->setMercyLockedAbovePct(data.mercyLockedAbovePct);

    if (!data.check.empty()) {
        enemy->setCheckFlavor(data.check);
    }
    if (!data.acts.empty()) {
        enemy->setActs(data.acts);
    }
    if (!data.bubbles.empty()) {
        enemy->setBubbles(data.bubbles);
    }
    if (!data.patterns.empty()) {
        enemy->setPatterns(data.patterns);
    }
    if (!data.defaultFlavor.empty() || !data.lowHpFlavor.empty() || !data.spareableFlavor.empty()) {
        enemy->setFlavorPools(data.defaultFlavor, data.lowHpFlavor, data.spareableFlavor);
    }
    if (!data.defaultDialogue.empty() || !data.afterHitDialogue.empty()) {
        enemy->setDialogueTriggers(data.defaultDialogue, data.afterHitDialogue, {}, {}, {});
    }

    std::ifstream file(filepath);
    if (file.is_open()) {
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string json = buffer.str();
        const int missingPatternFur = -1;
        const int uwuFur = patternFurFromJson(json, "uwu_rain", missingPatternFur);
        const int dewFur = patternFurFromJson(json, "dew_bounce", missingPatternFur);
        if (uwuFur >= 0) enemy->m_patternFur["uwu_rain"] = uwuFur;
        if (dewFur >= 0) enemy->m_patternFur["dew_bounce"] = dewFur;
        enemy->m_howlFur = howlFurFromJson(json, missingPatternFur);
    }

    return enemy;
}

} // namespace nundertale
