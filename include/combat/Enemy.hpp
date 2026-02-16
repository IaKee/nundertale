#pragma once

#include <string>
#include <vector>
#include <raylib.h>

namespace nundertale {

class Enemy {
public:
    Enemy(const std::string& name, int maxHp, int atk, int def)
        : m_name(name), m_maxHp(maxHp), m_hp(maxHp), m_atk(atk), m_def(def) {}

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

    std::vector<std::string> getActOptions() const {
        return m_actOptions;
    }

    void setActOptions(const std::vector<std::string>& opts) {
        m_actOptions = opts;
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
    std::string m_name;
    int m_maxHp;
    int m_hp;
    int m_atk;
    int m_def;
    bool m_spareable = false;
    std::vector<std::string> m_actOptions{ "Check", "Cringe Check", "Offer Mountain Dew" };
    std::vector<std::string> m_dialogues{ "OwO what's this?!", "You cannot defeat my fursona!", "*Awoo noises*" };
};

} // namespace nundertale
