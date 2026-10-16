#include "combat/BattleSystem.hpp"
#include "core/GameContext.hpp"
#include "core/Constants.hpp"
#include <cmath>
#include <algorithm>
#include <sstream>

namespace nundertale {

BattleSystem::BattleSystem(std::unique_ptr<Enemy> enemy)
    : m_enemy(std::move(enemy)) {
    m_narratorText = "* " + m_enemy->getName() + " blocked your path!";
    if (m_enemy) {
        m_enemyVisualHp = static_cast<float>(m_enemy->getHp());
    }
}

void BattleSystem::update(float dt, GameContext& context) {
    if (m_invulnTimer > 0.0f) {
        m_invulnTimer -= dt;
    }
    if (m_enemyShakeTimer > 0.0f) {
        m_enemyShakeTimer = std::max(0.0f, m_enemyShakeTimer - dt);
    }
    if (m_damageDisplayTimer > 0.0f) {
        m_damageDisplayTimer = std::max(0.0f, m_damageDisplayTimer - dt);
    }
    if (m_slashAnimTimer > 0.0f) {
        m_slashAnimTimer = std::max(0.0f, m_slashAnimTimer - dt);
    }

    updateDust(dt);

    if (m_enemy) {
        if (m_enemyVisualHp > static_cast<float>(m_enemy->getHp())) {
            float drainSpeed = std::max(20.0f, (float)m_lastDamage / 0.5f);
            m_enemyVisualHp = std::max(static_cast<float>(m_enemy->getHp()), m_enemyVisualHp - drainSpeed * dt);
        } else {
            m_enemyVisualHp = static_cast<float>(m_enemy->getHp());
        }
    }

    // Box animated resize transition (BHL-001)
    if (m_state == BattleState::EnemyTurn) {
        m_targetBoxRect = { 237.0f, 250.0f, 165.0f, 140.0f };
    } else {
        m_targetBoxRect = { (float)BATTLE_BOX_X, (float)BATTLE_BOX_Y, (float)BATTLE_BOX_W, (float)BATTLE_BOX_H };
    }

    float speed = std::clamp(20.0f * dt, 0.0f, 1.0f);
    m_arenaRect.x += (m_targetBoxRect.x - m_arenaRect.x) * speed;
    m_arenaRect.y += (m_targetBoxRect.y - m_arenaRect.y) * speed;
    m_arenaRect.width += (m_targetBoxRect.width - m_arenaRect.width) * speed;
    m_arenaRect.height += (m_targetBoxRect.height - m_arenaRect.height) * speed;

    switch (m_state) {
        case BattleState::MenuSelect:
            handleMenuInput(context);
            break;
        case BattleState::FightTarget:
            handleFightTargetInput(dt, context);
            break;
        case BattleState::ActSelect:
            handleActInput(context);
            break;
        case BattleState::ItemSelect:
            handleItemInput(context);
            break;
        case BattleState::MercySelect:
            handleMercyInput(context);
            break;
        case BattleState::Fleeing:
            m_fleeTimer += dt;
            m_fleeSoulX -= 260.0f * dt;
            if (static_cast<int>(m_fleeTimer * 30.0f) % 4 == 0) {
                spawnDust({ m_fleeSoulX + 16.0f, m_fleeSoulY + 12.0f }, 1, LIGHTGRAY);
            }
            if (m_fleeSoulX < -30.0f) {
                m_state = BattleState::Fled;
            }
            break;
        case BattleState::Fled:
            break;
        case BattleState::DialogMessage:
            if (context.input.isConfirmPressed()) {
                // BUG-02: Only ends battle if enemy is actually dead or spared via MERCY!
                if (m_enemy->isDead() || m_isSpared) {
                    m_state = BattleState::Victory;
                    if (m_isSpared) {
                        context.player.gold += 10;
                        context.state.setFlag("ruins.barky.spared", 1);
                        m_narratorText = "* YOU WON!\n* You earned 0 EXP and 10 gold.";
                        spawnDust({ (float)(VIRTUAL_WIDTH / 2), (float)(ENEMY_BASELINE_Y - 80) }, 25, COLOR_UNDERTALE_YELLOW);
                    } else {
                        context.player.exp += 20;
                        context.player.gold += 15;
                        context.state.setFlag("ruins.barky.killed", 1);
                        context.state.killsTotal += 1;
                        m_narratorText = "* YOU WON!\n* You earned 20 EXP and 15 gold.";
                        spawnDust({ (float)(VIRTUAL_WIDTH / 2), (float)(ENEMY_BASELINE_Y - 80) }, 30, LIGHTGRAY);

                        // Check level up (PRG-001, BUG-16)
                        static const int expThresholds[] = {
                            0, 10, 30, 70, 120, 200, 300, 500, 800, 1200,
                            1700, 2500, 3500, 5000, 7000, 10000, 15000, 25000, 50000, 99999
                        };
                        int newLevel = context.player.level;
                        for (int lvl = 20; lvl >= 1; --lvl) {
                            if (context.player.exp >= expThresholds[lvl - 1]) {
                                newLevel = lvl;
                                break;
                            }
                        }
                        if (newLevel > context.player.level) {
                            context.player.level = newLevel;
                            context.player.maxHp = (newLevel == 20) ? 99 : (16 + 4 * newLevel);
                            context.player.hp = context.player.maxHp;
                            m_narratorText += "\n* Your VITRIOL increased.";
                            context.audio.playSound("assets/sfx/heal.ogg", 0.9f);
                        }
                    }
                } else {
                    startEnemyTurn();
                }
            }
            break;
        case BattleState::EnemyTurn:
            updateEnemyTurn(dt, context);
            break;
        case BattleState::Victory:
        case BattleState::Defeat:
            break;
    }
}

void BattleSystem::handleMenuInput(GameContext& context) {
    if (context.input.isLeftPressed()) {
        int idx = (static_cast<int>(m_selectedButton) + 3) % 4;
        m_selectedButton = static_cast<MainButton>(idx);
        context.audio.playSound("assets/sfx/selection_sound.wav", 0.5f);
    } else if (context.input.isRightPressed()) {
        int idx = (static_cast<int>(m_selectedButton) + 1) % 4;
        m_selectedButton = static_cast<MainButton>(idx);
        context.audio.playSound("assets/sfx/selection_sound.wav", 0.5f);
    }

    if (context.input.isConfirmPressed()) {
        context.audio.playSound("assets/sfx/selection_sound.wav", 0.8f);
        m_subOptionIndex = 0;
        switch (m_selectedButton) {
            case MainButton::Fight: {
                m_state = BattleState::FightTarget;
                // BUG-17: Random entry side
                bool fromLeft = (GetRandomValue(0, 1) == 0);
                m_targetCursorX = fromLeft ? m_arenaRect.x + 10.0f : m_arenaRect.x + m_arenaRect.width - 10.0f;
                m_targetCursorSpeed = fromLeft ? 400.0f : -400.0f;
                m_targetCursorMoving = true;
                break;
            }
            case MainButton::Act:
                m_state = BattleState::ActSelect;
                break;
            case MainButton::Item:
                m_state = BattleState::ItemSelect;
                break;
            case MainButton::Mercy:
                m_state = BattleState::MercySelect;
                break;
        }
    }
}

void BattleSystem::handleFightTargetInput(float dt, GameContext& context) {
    if (m_targetCursorMoving) {
        // BUG-08: Use dt instead of GetFrameTime()
        m_targetCursorX += m_targetCursorSpeed * dt;

        bool outOfBounds = (m_targetCursorSpeed > 0.0f && m_targetCursorX >= m_arenaRect.x + m_arenaRect.width) ||
                           (m_targetCursorSpeed < 0.0f && m_targetCursorX <= m_arenaRect.x);

        if (outOfBounds) {
            m_targetCursorMoving = false;
            m_narratorText = "* You swung wide and missed!";
            m_lastMiss = true;
            m_lastDamage = 0;
            m_damageDisplayTimer = 1.2f;
            m_enemyShakeTimer = 0.0f;
            m_state = BattleState::DialogMessage;
            context.audio.playSound("assets/sfx/cartoon_slip.ogg", 0.7f);
        } else if (context.input.isConfirmPressed()) {
            m_targetCursorMoving = false;
            float center = m_arenaRect.x + m_arenaRect.width / 2.0f;
            float dist = std::abs(m_targetCursorX - center);
            float maxDist = m_arenaRect.width / 2.0f;

            // Damage formula (FGT-003)
            int baseAtk = context.player.at + context.player.weaponAt;
            int enemyDef = m_enemy->getDef();
            float accuracyMult = (dist <= 12.0f) ? 2.2f : std::max(0.5f, (1.0f - dist / maxDist) * 2.0f);
            int damage = std::max(1, static_cast<int>(std::round((baseAtk - enemyDef + GetRandomValue(1, 3)) * accuracyMult)));

            m_enemyVisualHp = static_cast<float>(m_enemy->getHp());
            m_enemy->takeDamage(damage);

            m_lastMiss = false;
            m_lastDamage = damage;
            m_damageDisplayTimer = 1.5f;
            m_enemyShakeTimer = 0.4f;
            m_slashAnimTimer = 0.25f;

            // FGT-008: Hitmarker on critical hit, slice sound on normal hit
            if (dist <= 12.0f) {
                context.audio.playSound("assets/sfx/u_activate.wav", 0.9f);
                m_narratorText = "* CRITICAL HIT! Dealt " + std::to_string(damage) + " cringe damage!";
            } else {
                context.audio.playSound("assets/sfx/u1.ogg", 0.8f);
                m_narratorText = "* You dealt " + std::to_string(damage) + " cringe damage to " + m_enemy->getName() + "!";
            }

            if (m_enemy->isDead()) {
                m_narratorText += "\n* Barky was vaporized into synthetic fluff!";
                spawnDust({ (float)(VIRTUAL_WIDTH / 2), (float)(ENEMY_BASELINE_Y - 80) }, 25, WHITE);
            }

            m_state = BattleState::DialogMessage;
        }
    }

    if (context.input.isCancelPressed()) {
        m_state = BattleState::MenuSelect;
        m_targetCursorMoving = false;
    }
}

void BattleSystem::handleActInput(GameContext& context) {
    const auto& acts = m_enemy->getActs();
    if (acts.empty()) {
        m_state = BattleState::MenuSelect;
        return;
    }

    int actCount = (int)acts.size();
    if (context.input.isLeftPressed()) {
        if (m_subOptionIndex % 2 == 1) {
            m_subOptionIndex--;
            context.audio.playSound("assets/sfx/selection_sound.wav", 0.4f);
        }
    } else if (context.input.isRightPressed()) {
        if (m_subOptionIndex % 2 == 0 && m_subOptionIndex + 1 < actCount) {
            m_subOptionIndex++;
            context.audio.playSound("assets/sfx/selection_sound.wav", 0.4f);
        }
    } else if (context.input.isUpPressed()) {
        if (m_subOptionIndex >= 2) {
            m_subOptionIndex -= 2;
            context.audio.playSound("assets/sfx/selection_sound.wav", 0.4f);
        }
    } else if (context.input.isDownPressed()) {
        if (m_subOptionIndex + 2 < actCount) {
            m_subOptionIndex += 2;
            context.audio.playSound("assets/sfx/selection_sound.wav", 0.4f);
        }
    }

    if (context.input.isConfirmPressed()) {
        context.audio.playSound("assets/sfx/selection_sound.wav", 0.8f);
        const auto& act = acts[m_subOptionIndex];

        if (act.id == "check") {
            // BUG-06 & ACT-001: Data-driven check text matching enemy instance stats
            m_narratorText = m_enemy->getCheckText();
            m_enemySpeechBubble = "";
        } else {
            m_narratorText = act.text;
            m_enemySpeechBubble = act.response;
            if (act.spareable) {
                m_enemy->setSpareable(true);
            }
        }

        m_state = BattleState::DialogMessage;
    } else if (context.input.isCancelPressed()) {
        m_state = BattleState::MenuSelect;
    }
}

void BattleSystem::handleItemInput(GameContext& context) {
    if (context.player.inventory.empty()) {
        if (context.input.isCancelPressed() || context.input.isConfirmPressed()) {
            m_state = BattleState::MenuSelect;
        }
        return;
    }

    int invCount = (int)context.player.inventory.size();
    if (context.input.isLeftPressed()) {
        if (m_subOptionIndex % 2 == 1) {
            m_subOptionIndex--;
            context.audio.playSound("assets/sfx/selection_sound.wav", 0.4f);
        } else if (m_subOptionIndex >= 4) {
            // Previous page
            m_subOptionIndex -= 4;
            context.audio.playSound("assets/sfx/selection_sound.wav", 0.4f);
        }
    } else if (context.input.isRightPressed()) {
        if (m_subOptionIndex % 2 == 0 && m_subOptionIndex + 1 < invCount) {
            m_subOptionIndex++;
            context.audio.playSound("assets/sfx/selection_sound.wav", 0.4f);
        } else if (m_subOptionIndex < 4 && m_subOptionIndex + 4 < invCount) {
            // Next page
            m_subOptionIndex += 4;
            context.audio.playSound("assets/sfx/selection_sound.wav", 0.4f);
        }
    } else if (context.input.isUpPressed()) {
        if ((m_subOptionIndex % 4) >= 2) {
            m_subOptionIndex -= 2;
            context.audio.playSound("assets/sfx/selection_sound.wav", 0.4f);
        }
    } else if (context.input.isDownPressed()) {
        if ((m_subOptionIndex % 4) < 2 && m_subOptionIndex + 2 < invCount) {
            m_subOptionIndex += 2;
            context.audio.playSound("assets/sfx/selection_sound.wav", 0.4f);
        }
    }

    if (context.input.isConfirmPressed()) {
        std::string item = context.player.inventory[m_subOptionIndex];
        context.player.inventory.erase(context.player.inventory.begin() + m_subOptionIndex);

        // ITM-001 & BUG-05: Handle specific item data behaviors
        if (item.find("Dew") != std::string::npos) {
            context.player.hp = std::min(context.player.maxHp, context.player.hp + 10);
            m_narratorText = "* You chugged the warm Mountain Dew!\n* You recovered 10 HP! Citrus surged!";
        } else if (item.find("Doritos") != std::string::npos) {
            context.player.hp = std::min(context.player.maxHp, context.player.hp + 6);
            m_narratorText = "* You devoured the stale Doritos!\n* Pure nostalgia. You recovered 6 HP!";
        } else if (item.find("Duct Tape") != std::string::npos || item.find("Tape") != std::string::npos) {
            context.player.furryMeter = std::max(0.0f, context.player.furryMeter - 15.0f);
            m_narratorText = "* You aggressively taped down your sprouting fur!\n* Ouch! Furry meter reduced by 15%!";
        } else {
            context.player.hp = std::min(context.player.maxHp, context.player.hp + 8);
            m_narratorText = "* You used " + item + "!\n* Recovered 8 HP.";
        }

        if (context.player.hp >= context.player.maxHp && item.find("Tape") == std::string::npos) {
            m_narratorText += "\n* Your HP was maxed out.";
        }

        context.audio.playSound("assets/sfx/heal.ogg", 0.8f);
        m_state = BattleState::DialogMessage;
    } else if (context.input.isCancelPressed()) {
        m_state = BattleState::MenuSelect;
    }
}

void BattleSystem::handleMercyInput(GameContext& context) {
    if (context.input.isUpPressed() || context.input.isDownPressed()) {
        m_subOptionIndex = 1 - m_subOptionIndex;
        context.audio.playSound("assets/sfx/selection_sound.wav", 0.4f);
    }

    if (context.input.isConfirmPressed()) {
        context.audio.playSound("assets/sfx/selection_sound.wav", 0.8f);
        if (m_subOptionIndex == 0) { // Spare
            if (m_enemy->isSpareable()) {
                m_isSpared = true;
                m_narratorText = "* You spared " + m_enemy->getName() + "!\n* Barky happily wags his tail and steps aside.";
                spawnDust({ (float)(VIRTUAL_WIDTH / 2), (float)(ENEMY_BASELINE_Y - 80) }, 15, COLOR_UNDERTALE_YELLOW);
                context.audio.playSound("assets/sfx/selection_sound.wav", 1.0f);
            } else {
                m_narratorText = "* You spared " + m_enemy->getName() + ".\n* But their name wasn't yellow...";
            }
            m_state = BattleState::DialogMessage;
        } else { // Flee (MRC-002)
            float fleeChance = 0.5f + 0.1f * static_cast<float>(m_turnCount);
            bool success = (GetRandomValue(1, 100) <= static_cast<int>(fleeChance * 100.0f));
            if (success) {
                m_state = BattleState::Fleeing;
                m_fleeSoulX = m_arenaRect.x + 35.0f;
                m_fleeSoulY = m_arenaRect.y + 55.0f + 10.0f;
                m_fleeTimer = 0.0f;
                context.audio.playSound("assets/sfx/cartoon_slip.ogg", 0.7f);
            } else {
                m_narratorText = "* Couldn't run away!\n* Barky blocked your escape with energetic awoos!";
                m_state = BattleState::DialogMessage;
            }
        }
    } else if (context.input.isCancelPressed()) {
        m_state = BattleState::MenuSelect;
    }
}

void BattleSystem::startEnemyTurn() {
    m_turnCount++;
    m_state = BattleState::EnemyTurn;
    m_enemyTurnTimer = 0.0f;
    m_spawnTimer = 0.0f;
    m_bullets.clear();
    // Center soul in arena
    m_soulPosition = { 320.0f, 320.0f };
    m_enemySpeechBubble = m_enemy->getRandomDialogue();
}

void BattleSystem::updateEnemyTurn(float dt, GameContext& context) {
    m_enemyTurnTimer += dt;
    if (m_enemyTurnTimer >= m_enemyTurnDuration) {
        m_bullets.clear();
        m_narratorText = "* Barky is pacing back and forth.";
        m_state = BattleState::MenuSelect;
        return;
    }

    // Soul movement inside dodge box (BHL-006: 4 px/frame at 30 fps = 120 px/s)
    Vector2 moveDir = { 0, 0 };
    if (context.input.isLeft()) moveDir.x -= 1.0f;
    if (context.input.isRight()) moveDir.x += 1.0f;
    if (context.input.isUp()) moveDir.y -= 1.0f;
    if (context.input.isDown()) moveDir.y += 1.0f;

    m_soulPosition.x += moveDir.x * m_soulSpeed * dt;
    m_soulPosition.y += moveDir.y * m_soulSpeed * dt;

    // Clamp inside arena with margin
    float soulPadding = 12.0f;
    m_soulPosition.x = std::clamp(m_soulPosition.x, m_arenaRect.x + soulPadding, m_arenaRect.x + m_arenaRect.width - soulPadding);
    m_soulPosition.y = std::clamp(m_soulPosition.y, m_arenaRect.y + soulPadding, m_arenaRect.y + m_arenaRect.height - soulPadding);

    // Spawn bullets
    m_spawnTimer += dt;
    if (m_spawnTimer >= 0.38f) {
        m_spawnTimer = 0.0f;
        spawnBulletWave();
    }

    // Update & check bullet collisions (BHL-003: 8x8 soul hitbox)
    Rectangle soulHitbox = { m_soulPosition.x - 4, m_soulPosition.y - 4, 8, 8 };

    for (size_t i = 0; i < m_bullets.size(); ) {
        m_bullets[i].update(dt, &m_arenaRect);

        if (m_bullets[i].isOutOfBounds(m_arenaRect, 40.0f)) {
            m_bullets.erase(m_bullets.begin() + i);
            continue;
        }

        if (m_bullets[i].active && m_invulnTimer <= 0.0f && CheckCollisionRecs(soulHitbox, m_bullets[i].getBounds())) {
            // Damage calculation based on enemy ATK and player DF (BHL-005)
            int rawDmg = m_bullets[i].damage + m_enemy->getAtk() - (context.player.df + context.player.armorDf);
            int finalDmg = std::max(1, rawDmg);

            context.player.hp -= finalDmg;
            context.player.furryMeter += m_bullets[i].furry;
            m_invulnTimer = 1.0f; // 30 frames of i-frame
            context.audio.playSound("assets/sfx/pipipipopopo_backwards.ogg", 0.6f);

            if (context.player.hp <= 0) {
                context.player.hp = 0;
                m_state = BattleState::Defeat;
                m_narratorText = "* GAME OVER! You lost all HP and became a furry!";
                context.audio.playSound("assets/sfx/areusure.ogg", 1.0f);
                return;
            }
        }
        ++i;
    }
}

void BattleSystem::spawnBulletWave() {
    int pattern = m_turnCount % 3;

    if (pattern == 0) {
        // Pattern 1: UwU Rain (BHL-014: falling letters with slight zigzag)
        float startX = m_arenaRect.x + (float)GetRandomValue(15, (int)m_arenaRect.width - 25);
        Bullet b;
        b.position = { startX, m_arenaRect.y - 12.0f };
        b.velocity = { (float)GetRandomValue(-20, 20), (float)GetRandomValue(95, 135) };
        b.type = BulletType::UwU;
        b.text = "UWU";
        b.color = (Color){ 255, 120, 210, 255 };
        b.damage = 3;
        b.furry = 6.0f;
        m_bullets.push_back(b);
    } else if (pattern == 1) {
        // Pattern 2: Dew Bounce (BHL-014: Mountain Dew cans bouncing off arena walls & floor)
        float startX = m_arenaRect.x + (float)GetRandomValue(20, (int)m_arenaRect.width - 20);
        Bullet b;
        b.position = { startX, m_arenaRect.y - 10.0f };
        b.velocity = { (float)GetRandomValue(-70, 70), (float)GetRandomValue(40, 75) };
        b.type = BulletType::SodaCan;
        b.color = COLOR_UNDERTALE_GREEN;
        b.gravity = 140.0f;
        b.bounces = true;
        b.damage = 4;
        b.furry = 7.0f;
        m_bullets.push_back(b);
    } else {
        // Pattern 3: Awoo Howl (BHL-014: sound waves expanding downward from Barky)
        float startX = (GetRandomValue(0, 1) == 0) ? (m_arenaRect.x + 22.0f) : (m_arenaRect.x + m_arenaRect.width - 22.0f);
        Bullet b;
        b.position = { startX, m_arenaRect.y - 12.0f };
        float dirX = (startX < m_arenaRect.x + m_arenaRect.width / 2.0f) ? 35.0f : -35.0f;
        b.velocity = { dirX, (float)GetRandomValue(85, 120) };
        b.type = BulletType::Awoo;
        b.arcRadius = 14.0f;
        b.color = (Color){ 100, 200, 255, 255 };
        b.damage = 3;
        b.furry = 5.0f;
        m_bullets.push_back(b);
    }
}

void BattleSystem::drawBackgroundGrid() const {
    // Horizontal 1px lines in GRID_GREEN (BUI-002)
    DrawLine(15, 9, 623, 9, GRID_GREEN);
    DrawLine(15, 126, 623, 126, GRID_GREEN);
    DrawLine(15, 244, 623, 244, GRID_GREEN);

    // Vertical 1px lines in GRID_GREEN (BUI-002)
    int vertX[7] = { 15, 115, 218, 317, 420, 523, 623 };
    for (int vx : vertX) {
        DrawLine(vx, 9, vx, 244, GRID_GREEN);
    }
}

void BattleSystem::drawSoul(float x, float y, Color color) const {
    DrawCircle((int)x - 4, (int)y - 2, 5, color);
    DrawCircle((int)x + 4, (int)y - 2, 5, color);
    DrawTriangle(
        { x - 9.0f, y },
        { x + 9.0f, y },
        { x, y + 9.0f },
        color
    );
}

void BattleSystem::drawHUD(GameContext& context, Font font) const {
    // 1. Name at x = 30
    context.resources.drawText(font, context.player.name, { (float)HUD_NAME_X, (float)HUD_Y }, 20, WHITE, 1.0f);

    // 2. "LV <n>" at x = 132
    std::string lvText = "LV " + std::to_string(context.player.level);
    context.resources.drawText(font, lvText, { (float)HUD_LV_X, (float)HUD_Y }, 20, WHITE, 1.0f);

    // 3. Label "HP" at x = 244
    context.resources.drawText(font, "HP", { (float)HUD_HP_LABEL_X, (float)HUD_Y + 4 }, 14, WHITE, 1.0f);

    // 4. HP Bar at x = 275, y = 400 (width = maxHp * 1.2f, height = 20)
    int barW = std::max(20, (int)(context.player.maxHp * 1.2f));
    DrawRectangle(HUD_HP_BAR_X, HUD_Y, barW, HUD_HP_BAR_H, HP_RED);

    float hpPct = std::clamp((float)context.player.hp / (float)context.player.maxHp, 0.0f, 1.0f);
    int fillW = (int)(barW * hpPct);
    DrawRectangle(HUD_HP_BAR_X, HUD_Y, fillW, HUD_HP_BAR_H, HP_YELLOW);

    // Furry Karma overlay (FUR-003) if furryMeter > 0
    if (context.player.furryMeter > 0.0f) {
        float furryPct = std::clamp(context.player.furryMeter / 100.0f, 0.0f, 1.0f);
        int furW = (int)(barW * furryPct);
        DrawRectangle(HUD_HP_BAR_X + barW - furW, HUD_Y, furW, HUD_HP_BAR_H, KR_PURPLE);
    }

    // 5. HP Numbers after bar
    int numX = HUD_HP_BAR_X + barW + 16;
    std::string hpStr = std::to_string(context.player.hp) + " / " + std::to_string(context.player.maxHp);
    Color numCol = (context.player.furryMeter > 0.0f) ? KR_PURPLE : WHITE;
    context.resources.drawText(font, hpStr, { (float)numX, (float)HUD_Y }, 20, numCol, 1.0f);
}

void BattleSystem::drawButtons(GameContext& context, Font font) const {
    const char* btnLabels[4] = { "FIGHT", "ACT", "ITEM", "MERCY" };
    int btnX[4] = { BATTLE_BUTTON_X_FIGHT, BATTLE_BUTTON_X_ACT, BATTLE_BUTTON_X_ITEM, BATTLE_BUTTON_X_MERCY };

    for (int i = 0; i < 4; ++i) {
        Rectangle bRec = { (float)btnX[i], (float)BATTLE_BUTTON_Y, (float)BATTLE_BUTTON_W, (float)BATTLE_BUTTON_H };
        bool isSelected = (m_selectedButton == static_cast<MainButton>(i));

        Color bColor = isSelected ? UI_YELLOW : UI_ORANGE;
        DrawRectangleLinesEx(bRec, 2.0f, bColor);

        float textW = context.resources.measureTextWidth(font, btnLabels[i], 22, 1.0f);
        Vector2 textPos = { bRec.x + (bRec.width - textW) * 0.5f, bRec.y + 10 };
        context.resources.drawText(font, btnLabels[i], textPos, 22, bColor, 1.0f);

        // Undertale 1:1: Selected button replaces left icon with Soul Heart (BUI-006)
        if (isSelected && m_state == BattleState::MenuSelect) {
            drawSoul(bRec.x + 18, bRec.y + 21, SOUL_RED);
        }
    }
}

void BattleSystem::spawnDust(Vector2 center, int count, Color col) {
    for (int i = 0; i < count; ++i) {
        DustParticle p;
        p.pos = center;
        float angle = (float)GetRandomValue(0, 360) * (3.14159f / 180.0f);
        float spd = (float)GetRandomValue(30, 130);
        p.vel = { std::cos(angle) * spd, std::sin(angle) * spd - 35.0f };
        p.alpha = 1.0f;
        p.size = (float)GetRandomValue(3, 7);
        p.color = col;
        m_dustParticles.push_back(p);
    }
}

void BattleSystem::updateDust(float dt) {
    for (size_t i = 0; i < m_dustParticles.size(); ) {
        m_dustParticles[i].pos.x += m_dustParticles[i].vel.x * dt;
        m_dustParticles[i].pos.y += m_dustParticles[i].vel.y * dt;
        m_dustParticles[i].vel.x *= (1.0f - 1.5f * dt);
        m_dustParticles[i].alpha -= 1.4f * dt;
        if (m_dustParticles[i].alpha <= 0.0f) {
            m_dustParticles.erase(m_dustParticles.begin() + i);
        } else {
            ++i;
        }
    }
}

void BattleSystem::drawDust() const {
    for (const auto& p : m_dustParticles) {
        Color c = p.color;
        c.a = (unsigned char)(std::clamp(p.alpha, 0.0f, 1.0f) * 255.0f);
        DrawRectangle((int)p.pos.x - (int)p.size / 2, (int)p.pos.y - (int)p.size / 2, (int)p.size, (int)p.size, c);
    }
}

void BattleSystem::render(GameContext& context) {
    Font font8Bit = context.resources.getFont8Bit(22);
    Font fontSmall = context.resources.getFont8Bit(16);

    // 1. Official Green Grid Background (BUI-002)
    drawBackgroundGrid();

    // 2. Enemy Drawing (Feet aligned at baseline y = 241, BUI-003)
    int baselineY = ENEMY_BASELINE_Y;
    int enemyCenterX = VIRTUAL_WIDTH / 2;

    // FGT-006: Enemy shake upon taking damage
    if (m_enemyShakeTimer > 0.0f) {
        float shakeProgress = m_enemyShakeTimer / 0.4f;
        enemyCenterX += static_cast<int>(std::sin(m_enemyShakeTimer * 50.0f) * (8.0f * shakeProgress));
    }

    // FGT-007, BND-002, BND-003: Check if enemy should be drawn
    bool shouldDrawEnemy = true;
    if (m_enemy->isDead() && m_state == BattleState::Victory) {
        shouldDrawEnemy = false; // Vaporized into fluff / dust!
    }

    if (shouldDrawEnemy) {
        Color bodyColor = { 50, 50, 70, 255 };
        Color headColor = { 120, 120, 140, 255 };
        Color earColor  = { 90, 90, 110, 255 };

        if (m_isSpared && m_state == BattleState::Victory) {
            // Translucent when spared
            bodyColor.a = 70;
            headColor.a = 70;
            earColor.a  = 70;
        }

        // Body
        DrawRectangle(enemyCenterX - 35, baselineY - 80, 70, 70, bodyColor);
        // Head
        DrawCircle(enemyCenterX, baselineY - 110, 32, headColor);
        // Ears
        DrawTriangle(
            { (float)enemyCenterX - 30, (float)baselineY - 135 },
            { (float)enemyCenterX - 10, (float)baselineY - 140 },
            { (float)enemyCenterX - 20, (float)baselineY - 165 },
            earColor
        );
        DrawTriangle(
            { (float)enemyCenterX + 10, (float)baselineY - 140 },
            { (float)enemyCenterX + 30, (float)baselineY - 135 },
            { (float)enemyCenterX + 20, (float)baselineY - 165 },
            earColor
        );
        // Eyes
        DrawCircle(enemyCenterX - 10, baselineY - 114, 5, WHITE);
        DrawCircle(enemyCenterX + 10, baselineY - 114, 5, WHITE);
        DrawCircle(enemyCenterX - 10, baselineY - 114, 2, BLACK);
        DrawCircle(enemyCenterX + 10, baselineY - 114, 2, BLACK);
    }

    // FGT-004: Slash Animation
    if (m_slashAnimTimer > 0.0f) {
        float slashProg = 1.0f - (m_slashAnimTimer / 0.25f);
        Vector2 slashStart = { (float)enemyCenterX - 45.0f + slashProg * 15.0f, (float)baselineY - 145.0f + slashProg * 25.0f };
        Vector2 slashEnd   = { (float)enemyCenterX + 45.0f - slashProg * 15.0f, (float)baselineY - 65.0f + slashProg * 10.0f };
        DrawLineEx(slashStart, slashEnd, 4.0f, WHITE);
        DrawCircle(enemyCenterX, baselineY - 105, 8.0f * (1.0f - slashProg), UI_YELLOW);
    }

    // FGT-005: Floating Damage Number & Enemy HP Bar
    if (m_damageDisplayTimer > 0.0f) {
        float hpBarY = (float)baselineY - 145.0f;
        if (m_lastMiss) {
            context.resources.drawCenteredText(font8Bit, "MISS", hpBarY - 6.0f, 24, { 190, 190, 190, 255 }, 1.0f);
        } else {
            float bounceOffset = (m_damageDisplayTimer > 1.2f) ? -12.0f * std::sin((1.5f - m_damageDisplayTimer) / 0.3f * 3.14159f) : -4.0f;
            float numY = hpBarY - 26.0f + bounceOffset;
            std::string dmgStr = std::to_string(m_lastDamage);
            context.resources.drawCenteredText(font8Bit, dmgStr, numY, 26, { 235, 30, 30, 255 }, 1.0f);

            float barW = 120.0f;
            float barH = 13.0f;
            float barX = (float)(VIRTUAL_WIDTH / 2) - barW / 2.0f;

            // Background
            DrawRectangle((int)barX, (int)hpBarY, (int)barW, (int)barH, { 180, 30, 30, 255 });

            // Green fill proportional to visual HP
            float maxHp = (float)m_enemy->getMaxHp();
            float fillRatio = (maxHp > 0.0f) ? std::clamp(m_enemyVisualHp / maxHp, 0.0f, 1.0f) : 0.0f;
            float fillW = barW * fillRatio;
            if (fillW > 0.0f) {
                DrawRectangle((int)barX, (int)hpBarY, (int)fillW, (int)barH, { 0, 255, 0, 255 });
            }

            DrawRectangleLinesEx({ barX - 1, hpBarY - 1, barW + 2, barH + 2 }, 2.0f, { 20, 20, 20, 255 });
        }
    }

    // Dust particles
    drawDust();

    // Enemy Speech Bubble during enemy turn
    if (m_state == BattleState::EnemyTurn && !m_enemySpeechBubble.empty()) {
        Rectangle bubbleRect = { (float)enemyCenterX + 45, (float)baselineY - 160, 180, 52 };
        DrawRectangleRec(bubbleRect, WHITE);
        DrawRectangleLinesEx(bubbleRect, 2.0f, BLACK);

        DrawTriangle(
            { bubbleRect.x, bubbleRect.y + 20 },
            { bubbleRect.x, bubbleRect.y + 35 },
            { bubbleRect.x - 12, bubbleRect.y + 28 },
            WHITE
        );
        DrawLineEx({ bubbleRect.x, bubbleRect.y + 20 }, { bubbleRect.x - 12, bubbleRect.y + 28 }, 2.0f, BLACK);
        DrawLineEx({ bubbleRect.x, bubbleRect.y + 35 }, { bubbleRect.x - 12, bubbleRect.y + 28 }, 2.0f, BLACK);

        context.resources.drawText(fontSmall, m_enemySpeechBubble, { bubbleRect.x + 8, bubbleRect.y + 14 }, 16, BLACK, 1.0f);
    }

    // 3. Battle Box (BUI-001: 5px solid white border, black interior)
    DrawRectangleRec(m_arenaRect, BLACK);
    DrawRectangleLinesEx(m_arenaRect, 5.0f, WHITE);

    // 4. Content inside Battle Box
    if (m_state == BattleState::EnemyTurn) {
        // Scissor clipping for bullets (BUG-10, BHL-002)
        int innerX = (int)m_arenaRect.x + 5;
        int innerY = (int)m_arenaRect.y + 5;
        int innerW = std::max(1, (int)m_arenaRect.width - 10);
        int innerH = std::max(1, (int)m_arenaRect.height - 10);

        BeginScissorMode(innerX, innerY, innerW, innerH);
            for (const auto& b : m_bullets) {
                b.render();
            }
        EndScissorMode();

        // Soul Heart inside arena (flashing when invulnerable)
        if (m_invulnTimer <= 0.0f || (int)(GetTime() * 15.0) % 2 == 0) {
            drawSoul(m_soulPosition.x, m_soulPosition.y, SOUL_RED);
        }
    } else if (m_state == BattleState::FightTarget) {
        // Fight Target Bar minigame (FGT-001/002)
        DrawRectangle((int)m_arenaRect.x + 10, (int)m_arenaRect.y + 20, (int)m_arenaRect.width - 20, (int)m_arenaRect.height - 40, { 30, 30, 30, 255 });
        float center = m_arenaRect.x + m_arenaRect.width / 2.0f;
        DrawRectangle((int)center - 8, (int)m_arenaRect.y + 20, 16, (int)m_arenaRect.height - 40, UI_YELLOW);
        DrawRectangle((int)m_targetCursorX - 4, (int)m_arenaRect.y + 15, 8, (int)m_arenaRect.height - 30, WHITE);
    } else if (m_state == BattleState::ActSelect) {
        // ACT menu 2-column layout (BUI-008, ACT-001)
        const auto& acts = m_enemy->getActs();
        for (size_t i = 0; i < acts.size(); ++i) {
            int col = (int)(i % 2);
            int row = (int)(i / 2);
            float optX = (col == 0) ? (m_arenaRect.x + 55.0f) : (m_arenaRect.x + 300.0f);
            float optY = m_arenaRect.y + 25.0f + (float)row * 35.0f;
            if (m_subOptionIndex == (int)i) {
                drawSoul(optX - 20.0f, optY + 10.0f, SOUL_RED);
            }
            context.resources.drawText(font8Bit, "* " + acts[i].label, { optX, optY }, 20, WHITE, 1.0f);
        }
    } else if (m_state == BattleState::ItemSelect) {
        // Item menu 2-column 2-row paginated layout (BUI-008, ITM-002)
        if (context.player.inventory.empty()) {
            context.resources.drawText(font8Bit, "* No items in inventory!", { m_arenaRect.x + 40, m_arenaRect.y + 30 }, 20, WHITE, 1.0f);
        } else {
            int page = m_subOptionIndex / 4;
            int pageStart = page * 4;
            int pageEnd = std::min((int)context.player.inventory.size(), pageStart + 4);

            for (int i = pageStart; i < pageEnd; ++i) {
                int localIdx = i - pageStart;
                int col = localIdx % 2;
                int row = localIdx / 2;
                float optX = (col == 0) ? (m_arenaRect.x + 55.0f) : (m_arenaRect.x + 300.0f);
                float optY = m_arenaRect.y + 25.0f + (float)row * 35.0f;
                if (m_subOptionIndex == i) {
                    drawSoul(optX - 20.0f, optY + 10.0f, SOUL_RED);
                }
                context.resources.drawText(font8Bit, "* " + context.player.inventory[i], { optX, optY }, 20, WHITE, 1.0f);
            }

            if (context.player.inventory.size() > 4) {
                std::string pageStr = "PAGE " + std::to_string(page + 1);
                context.resources.drawText(font8Bit, pageStr, { m_arenaRect.x + 460.0f, m_arenaRect.y + 85.0f }, 18, WHITE, 1.0f);
            }
        }
    } else if (m_state == BattleState::MercySelect) {
        // Mercy menu (BUI-009: Spare in yellow if spareable!)
        Color spareColor = m_enemy->isSpareable() ? UI_YELLOW : WHITE;

        float spareY = m_arenaRect.y + 25;
        float fleeY = m_arenaRect.y + 55;

        if (m_subOptionIndex == 0) drawSoul(m_arenaRect.x + 35, spareY + 10, SOUL_RED);
        if (m_subOptionIndex == 1) drawSoul(m_arenaRect.x + 35, fleeY + 10, SOUL_RED);

        context.resources.drawText(font8Bit, "* Spare", { m_arenaRect.x + 55, spareY }, 20, spareColor, 1.0f);
        context.resources.drawText(font8Bit, "* Flee",  { m_arenaRect.x + 55, fleeY },  20, WHITE, 1.0f);
    } else {
        // Narrator message with word wrapping and hanging indent (BUG-09, TXT-002)
        float startX = m_arenaRect.x + 25.0f;
        float startY = m_arenaRect.y + 24.0f;
        float lineH = 30.0f;
        float indentX = startX + 24.0f;

        std::stringstream ss(m_narratorText);
        std::string paragraph;
        float currentY = startY;

        while (std::getline(ss, paragraph, '\n')) {
            if (paragraph.empty()) {
                currentY += lineH * 0.5f;
                continue;
            }

            bool hasAsterisk = (paragraph.rfind("* ", 0) == 0);
            std::stringstream wordStream(paragraph);
            std::string word;
            std::string currentLine = "";
            bool firstLineOfPara = true;

            while (wordStream >> word) {
                std::string testLine = currentLine.empty() ? word : (currentLine + " " + word);
                float effectiveX = (firstLineOfPara || !hasAsterisk) ? startX : indentX;
                float availableW = m_arenaRect.x + m_arenaRect.width - 25.0f - effectiveX;

                float testW = context.resources.measureTextWidth(font8Bit, testLine, 20, 1.0f);
                if (testW > availableW && !currentLine.empty()) {
                    context.resources.drawText(font8Bit, currentLine, { effectiveX, currentY }, 20, WHITE, 1.0f);
                    currentY += lineH;
                    firstLineOfPara = false;
                    currentLine = word;
                } else {
                    currentLine = testLine;
                }
            }

            if (!currentLine.empty()) {
                float effectiveX = (firstLineOfPara || !hasAsterisk) ? startX : indentX;
                context.resources.drawText(font8Bit, currentLine, { effectiveX, currentY }, 20, WHITE, 1.0f);
                currentY += lineH;
            }
        }
    }

    // 5. Official HUD 1:1 (BUI-005)
    drawHUD(context, font8Bit);

    // 6. Action Buttons 1:1 (BUI-006)
    drawButtons(context, font8Bit);

    // 7. Fleeing Soul Animation (MRC-002)
    if (m_state == BattleState::Fleeing) {
        drawSoul(m_fleeSoulX, m_fleeSoulY, SOUL_RED);
        float legCycle = std::sin(m_fleeTimer * 22.0f);
        DrawLineEx({ m_fleeSoulX - 4.0f, m_fleeSoulY + 7.0f }, { m_fleeSoulX - 4.0f + legCycle * 6.0f, m_fleeSoulY + 16.0f }, 2.0f, WHITE);
        DrawLineEx({ m_fleeSoulX + 4.0f, m_fleeSoulY + 7.0f }, { m_fleeSoulX + 4.0f - legCycle * 6.0f, m_fleeSoulY + 16.0f }, 2.0f, WHITE);
    }
}

} // namespace nundertale
