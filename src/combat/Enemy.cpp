#include "combat/Enemy.hpp"
#include "core/FurTable.hpp"
#include "core/TiledMap.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

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

namespace {

using JsonValue = tiled_detail::JsonValue;

const JsonValue* objectField(const JsonValue& object, const char* key, JsonValue::Kind kind) {
    const JsonValue* value = object.field(key);
    return value != nullptr && value->kind == kind ? value : nullptr;
}

std::string stringValue(const JsonValue& object, const char* key, const std::string& fallback = {}) {
    const JsonValue* value = objectField(object, key, JsonValue::Kind::String);
    return value == nullptr ? fallback : value->string;
}

int intValue(const JsonValue& object, const char* key, int fallback) {
    const JsonValue* value = objectField(object, key, JsonValue::Kind::Number);
    int parsed = fallback;
    return value != nullptr && tiled_detail::toInt(value->number, parsed) ? parsed : fallback;
}

float floatValue(const JsonValue& object, const char* key, float fallback) {
    const JsonValue* value = objectField(object, key, JsonValue::Kind::Number);
    return value == nullptr ? fallback : static_cast<float>(value->number);
}

bool boolValue(const JsonValue& object, const char* key, bool fallback = false) {
    const JsonValue* value = objectField(object, key, JsonValue::Kind::Boolean);
    return value == nullptr ? fallback : value->boolean;
}

std::vector<std::string> stringArray(const JsonValue& object, const char* key) {
    std::vector<std::string> result;
    const JsonValue* values = objectField(object, key, JsonValue::Kind::Array);
    if (values == nullptr) return result;
    for (const JsonValue& value : values->array) {
        if (value.kind == JsonValue::Kind::String) result.push_back(value.string);
    }
    return result;
}

ActDef parseAct(const JsonValue& value) {
    ActDef act;
    act.id = stringValue(value, "id");
    act.label = stringValue(value, "label", act.id);
    act.text = stringValue(value, "text");
    act.response = stringValue(value, "response");
    act.repeatText = stringValue(value, "repeatText");
    act.repeatResponse = stringValue(value, "repeatResponse");
    act.useTexts = stringArray(value, "useTexts");
    act.useResponses = stringArray(value, "useResponses");
    act.requiresItem = stringValue(value, "requiresItem");
    act.requiresFlag = stringValue(value, "requiresFlag");
    act.requiresFurMin = intValue(value, "requiresFurMin", 0);
    act.minUses = intValue(value, "minUses", 1);

    const JsonValue* effects = objectField(value, "effects", JsonValue::Kind::Object);
    if (effects != nullptr) {
        act.spareable = boolValue(*effects, "spareable");
        act.skipAttack = boolValue(*effects, "skipAttack");
        act.stopsBeat = boolValue(*effects, "stopsBeat");
        act.flees = boolValue(*effects, "flees");
        act.spareNow = boolValue(*effects, "spareNow");
        act.mercy = intValue(*effects, "mercy", 0);
        act.fur = intValue(*effects, "fur", 0);
        act.atkDelta = intValue(*effects, "atkDelta", 0);
        act.atkDeltaOnce = boolValue(*effects, "atkDeltaOnce", false);
        act.defDelta = intValue(*effects, "defDelta", 0);
        act.freezeTurns = intValue(*effects, "freezeTurns", 0);
        act.setFlag = stringValue(*effects, "setFlag");
        act.healNextAttack = boolValue(*effects, "healNextAttack", false);
    }
    return act;
}

EnemyPatternDef parsePattern(const JsonValue& value) {
    EnemyPatternDef pattern;
    pattern.id = stringValue(value, "id");
    pattern.kind = enemyPatternKindFromString(stringValue(value, "kind"));
    pattern.weight = intValue(value, "weight", 1);
    const JsonValue* params = objectField(value, "params", JsonValue::Kind::Object);
    if (params != nullptr) {
        pattern.count = intValue(*params, "count", 1);
        pattern.speed = floatValue(*params, "speed", 100.0f);
        pattern.damage = intValue(*params, "damage", 1);
        pattern.furry = intValue(*params, "furry", intValue(*params, "fur", 0));
        pattern.heal = intValue(*params, "heal", 0);
        pattern.text = stringValue(*params, "text");
        pattern.words = stringArray(*params, "words");
        const std::string color = stringValue(*params, "color", "white");
        if (color == "blue") {
            pattern.color = AttackColor::Blue;
        } else if (color == "orange") {
            pattern.color = AttackColor::Orange;
        } else if (color == "green") {
            pattern.color = AttackColor::Green;
        } else {
            pattern.color = AttackColor::White;
        }
        pattern.strike = boolValue(*params, "strike", false);
        pattern.strikeAfter = floatValue(*params, "strikeAfter", 0.0f);
        pattern.beatmap = stringValue(*params, "beatmap");
    }
    return pattern;
}

} // namespace

std::unique_ptr<Enemy> Enemy::loadFromJson(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::clog << "Could not load enemy JSON '" << filepath << "'; using Barky fallback.\n";
        return std::make_unique<Enemy>("Barky", 30, 4, 2);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    JsonValue root;
    if (!tiled_detail::JsonParser(buffer.str()).parse(root) || root.kind != JsonValue::Kind::Object) {
        std::clog << "Invalid enemy JSON '" << filepath << "'; using Barky fallback.\n";
        return std::make_unique<Enemy>("Barky", 30, 4, 2);
    }

    const std::string name = stringValue(root, "name", "Barky");
    const int hp = intValue(root, "hp", 30);
    const int maxHp = intValue(root, "maxHp", hp);
    const int atk = intValue(root, "atk", 4);
    const int def = intValue(root, "def", 2);

    auto enemy = std::make_unique<Enemy>(name, maxHp, atk, def);
    enemy->m_id = stringValue(root, "id", "barky");
    enemy->m_sprite = stringValue(root, "sprite", enemy->m_id);
    enemy->m_exp = intValue(root, "exp", 20);
    enemy->m_gold = intValue(root, "gold", 10);
    enemy->m_spareText = stringValue(root, "spareText", enemy->m_spareText);
    enemy->m_deathText = stringValue(root, "deathText", enemy->m_deathText);
    enemy->setMercyLockedAbovePct(intValue(root, "mercyLockedAbovePct", 0));
    enemy->m_howlFur = intValue(root, "howlFur", -1);
    enemy->m_spareLines = stringArray(root, "spareLines");
    enemy->m_spareOutro = stringValue(root, "spareOutro");
    enemy->m_deathLines = stringArray(root, "deathLines");
    enemy->m_spareFlag = stringValue(root, "spareFlag");
    enemy->m_killFlag = stringValue(root, "killFlag");
    enemy->m_mercifulBullets = boolValue(root, "mercifulBullets", false);

    const std::string check = stringValue(root, "check");
    if (!check.empty()) enemy->m_checkFlavor = check;

    if (const JsonValue* acts = objectField(root, "acts", JsonValue::Kind::Array)) {
        enemy->m_acts.clear();
        for (const JsonValue& rawAct : acts->array) {
            if (rawAct.kind != JsonValue::Kind::Object) continue;
            ActDef act = parseAct(rawAct);
            if (!act.id.empty()) enemy->m_acts.push_back(std::move(act));
        }
    }

    if (const JsonValue* patterns = objectField(root, "patterns", JsonValue::Kind::Array)) {
        enemy->m_patterns.clear();
        enemy->m_patternFur.clear();
        for (const JsonValue& rawPattern : patterns->array) {
            if (rawPattern.kind != JsonValue::Kind::Object) continue;
            EnemyPatternDef pattern = parsePattern(rawPattern);
            if (pattern.id.empty()) continue;
            enemy->m_patternFur[pattern.id] = pattern.furry;
            enemy->m_patterns.push_back(std::move(pattern));
        }
    }

    if (const JsonValue* flavor = objectField(root, "flavor", JsonValue::Kind::Object)) {
        const std::vector<std::string> defaults = stringArray(*flavor, "default");
        const std::vector<std::string> lowHp = stringArray(*flavor, "lowHp");
        const std::vector<std::string> spareable = stringArray(*flavor, "spareable");
        if (!defaults.empty()) enemy->m_defaultFlavor = defaults;
        // a data enemy never inherits Barky's built-in pools: missing pools fall back to `default`
        enemy->m_lowHpFlavor = lowHp;
        enemy->m_spareableFlavor = spareable;
    }

    if (const JsonValue* dialogue = objectField(root, "dialogue", JsonValue::Kind::Object)) {
        const std::vector<std::string> defaults = stringArray(*dialogue, "default");
        const std::vector<std::string> afterHit = stringArray(*dialogue, "afterHit");
        const std::vector<std::string> crit = stringArray(*dialogue, "crit");
        const std::vector<std::string> miss = stringArray(*dialogue, "miss");
        const std::vector<std::string> item = stringArray(*dialogue, "item");
        if (!defaults.empty()) enemy->m_dialoguesDefault = defaults;
        if (!afterHit.empty()) enemy->m_dialoguesAfterHit = afterHit;
        if (!crit.empty()) enemy->m_dialoguesCrit = crit;
        if (!miss.empty()) enemy->m_dialoguesMiss = miss;
        if (!item.empty()) enemy->m_dialoguesItem = item;
    }

    return enemy;
}

} // namespace nundertale
