#pragma once

#include <string>
#include <vector>
#include <raylib.h>

namespace nundertale {

struct ActDef {
    std::string id;
    std::string label;
    std::string text;
    std::string response;
    bool spareable = false;
    bool skipAttack = false;
    int mercy = 0;
};

class Enemy {
public:
    Enemy(const std::string& name, int maxHp, int atk, int def)
        : m_name(name), m_maxHp(maxHp), m_hp(maxHp), m_atk(atk), m_def(def),
          m_checkFlavor("The self-proclaimed Alpha Wolf of the Ruins.") {
        initDefaultActs();
    }

    std::string getName() const { return m_name; }
    int getHp() const { return m_hp; }
    int getMaxHp() const { return m_maxHp; }
    int getAtk() const { return m_atk; }
    int getDef() const { return m_def; }
    bool isDead() const { return m_hp <= 0; }
    bool isSpareable() const { return m_spareable; }
    void setSpareable(bool s) { m_spareable = s; }

    void takeDamage(int dmg) {
        m_hp -= dmg;
        if (m_hp < 0) m_hp = 0;
    }

    const std::vector<ActDef>& getActs() const { return m_acts; }
    void setActs(const std::vector<ActDef>& acts) { m_acts = acts; }
    void addAct(const ActDef& act) { m_acts.push_back(act); }

    std::vector<std::string> getActOptions() const {
        std::vector<std::string> options;
        for (const auto& a : m_acts) {
            options.push_back(a.label);
        }
        return options;
    }

    void setActOptions(const std::vector<std::string>& opts) {
        // Compatibility helper
        m_acts.clear();
        for (const auto& opt : opts) {
            m_acts.push_back({ opt, opt, "* You checked " + opt, "", false, false, 0 });
        }
    }

    std::string getCheckFlavor() const { return m_checkFlavor; }
    void setCheckFlavor(const std::string& flavor) { m_checkFlavor = flavor; }

    std::string getCheckText() const {
        return "* " + m_name + " - ATK " + std::to_string(m_atk) + " DEF " + std::to_string(m_def) + "\n* " + m_checkFlavor;
    }

    std::string getRandomDialogue() const {
        if (m_dialogues.empty()) return "...";
        int idx = GetRandomValue(0, (int)m_dialogues.size() - 1);
        return m_dialogues[idx];
    }

    void setDialogues(const std::vector<std::string>& lines) {
        m_dialogues = lines;
    }

private:
    void initDefaultActs() {
        m_acts = {
            { "check", "Check", "", "", false, false, 0 },
            { "cringe_check", "Cringe Check",
              "* You pointed out the visible zipper on Barky's fursuit!\n* Barky blushes furiously!",
              "B-baka! Stop looking at the zipper! It's a ventilation slot!", false, false, 50 },
            { "offer_dew", "Offer Mountain Dew",
              "* You offered Barky a can of warm Mountain Dew.\n* His eyes widen with pure gamer ecstasy! Barky can be SPARED!",
              "Is that... Citrus Blast 2012 edition?! You're alright, gamer.", true, false, 100 },
            { "debate_lore", "Debate Anime Lore",
              "* You challenged Barky on obscure anime filler arcs.\n* He gets overwhelmed thinking about it!",
              "Wait, season 4 episode 12 was canon?! That can't be!", false, true, 20 }
        };
    }

    std::string m_name;
    int m_maxHp;
    int m_hp;
    int m_atk;
    int m_def;
    std::string m_checkFlavor;
    bool m_spareable = false;
    std::vector<ActDef> m_acts;
    std::vector<std::string> m_dialogues{ "OwO what's this?!", "You cannot defeat my fursona!", "*Awoo noises*" };
};

} // namespace nundertale
