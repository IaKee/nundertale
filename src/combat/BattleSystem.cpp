#include "combat/BattleSystem.hpp"
#include "core/GameContext.hpp"
#include "core/Constants.hpp"
#include <cmath>
#include <algorithm>

namespace nundertale {

BattleSystem::BattleSystem(std::unique_ptr<Enemy> enemy)
    : m_enemy(std::move(enemy)) {
    m_narratorText = "* " + m_enemy->getName() + " blocked your path!";
}

void BattleSystem::update(float dt, GameContext& context) {
    if (m_invulnTimer > 0.0f) {
        m_invulnTimer -= dt;
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
        case BattleState::DialogMessage:
            if (context.input.isConfirmPressed()) {
                // BUG-02: Only ends battle if enemy is actually dead or spared via MERCY!
                if (m_enemy->isDead() || m_isSpared) {
                    m_state = BattleState::Victory;
                    if (m_isSpared) {
                        context.player.gold += 10;
                        context.state.setFlag("ruins.barky.spared", 1);
                        m_narratorText = "* YOU WON!\n* You earned 0 EXP and 10 gold.";
                    } else {
                        context.player.exp += 20;
                        context.player.gold += 15;
                        context.state.setFlag("ruins.barky.killed", 1);
                        context.state.killsTotal += 1;
                        m_narratorText = "* YOU WON!\n* You earned 20 EXP and 15 gold.";

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

            m_enemy->takeDamage(damage);
            context.audio.playSound("assets/sfx/fart_reverb.ogg", 0.8f);

            if (dist <= 12.0f) {
                m_narratorText = "* CRITICAL HIT! Dealt " + std::to_string(damage) + " cringe damage!";
            } else {
                m_narratorText = "* You dealt " + std::to_string(damage) + " cringe damage to " + m_enemy->getName() + "!";
            }

            if (m_enemy->isDead()) {
                m_narratorText += "\n* Barky was vaporized into synthetic fluff!";
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
    const auto& acts = m_enemy->getActOptions();
    if (acts.empty()) {
        m_state = BattleState::MenuSelect;
        return;
    }

    if (context.input.isUpPressed()) {
        m_subOptionIndex = (m_subOptionIndex - 1 + (int)acts.size()) % (int)acts.size();
        context.audio.playSound("assets/sfx/selection_sound.wav", 0.4f);
    } else if (context.input.isDownPressed()) {
        m_subOptionIndex = (m_subOptionIndex + 1) % (int)acts.size();
        context.audio.playSound("assets/sfx/selection_sound.wav", 0.4f);
    }

    if (context.input.isConfirmPressed()) {
        context.audio.playSound("assets/sfx/selection_sound.wav", 0.8f);
        std::string chosenAct = acts[m_subOptionIndex];

        if (chosenAct == "Check") {
            m_narratorText = "* BARKY - ATK 4 DEF 2\n* The self-proclaimed Alpha Wolf of the Ruins.";
        } else if (chosenAct == "Cringe Check") {
            m_narratorText = "* You pointed out the visible zipper on Barky's fursuit!\n* Barky blushes furiously: 'B-baka! It's a ventilation slot!'";
            m_enemy->setSpareable(true);
        } else if (chosenAct == "Offer Mountain Dew") {
            m_narratorText = "* You offered Barky a lukewarm can of Mountain Dew.\n* Barky wagged his tail furiously! He can now be SPARED!";
            m_enemy->setSpareable(true);
        } else {
            m_narratorText = "* You challenged Barky with obscure anime trivia.\n* He gets overwhelmed by canon debates!";
            m_enemy->setSpareable(true);
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
    if (context.input.isUpPressed()) {
        m_subOptionIndex = (m_subOptionIndex - 1 + invCount) % invCount;
        context.audio.playSound("assets/sfx/selection_sound.wav", 0.4f);
    } else if (context.input.isDownPressed()) {
        m_subOptionIndex = (m_subOptionIndex + 1) % invCount;
        context.audio.playSound("assets/sfx/selection_sound.wav", 0.4f);
    }

    if (context.input.isConfirmPressed()) {
        std::string item = context.player.inventory[m_subOptionIndex];
        context.player.inventory.erase(context.player.inventory.begin() + m_subOptionIndex);

        if (item.find("Dew") != std::string::npos) {
            context.player.hp = std::min(context.player.maxHp, context.player.hp + 10);
            m_narratorText = "* You chugged the Mountain Dew!\n* Recovered 10 HP. It was lukewarm.";
        } else if (item.find("Doritos") != std::string::npos) {
            context.player.hp = std::min(context.player.maxHp, context.player.hp + 6);
            m_narratorText = "* You devoured the stale Doritos!\n* Recovered 6 HP. Pure nostalgia.";
        } else {
            context.player.hp = std::min(context.player.maxHp, context.player.hp + 8);
            m_narratorText = "* You used " + item + "!\n* Recovered 8 HP.";
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
                context.audio.playSound("assets/sfx/selection_sound.wav", 1.0f);
            } else {
                m_narratorText = "* You spared " + m_enemy->getName() + ".\n* But their name wasn't yellow...";
            }
            m_state = BattleState::DialogMessage;
        } else { // Flee
            m_narratorText = "* Don't run away! A true gamer faces the cringe head-on!";
            m_state = BattleState::DialogMessage;
        }
    } else if (context.input.isCancelPressed()) {
        m_state = BattleState::MenuSelect;
    }
}

void BattleSystem::startEnemyTurn() {
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
    if (m_spawnTimer >= 0.40f) {
        m_spawnTimer = 0.0f;
        spawnBulletWave();
    }

    // Update & check bullet collisions (BHL-003: 8x8 soul hitbox)
    Rectangle soulHitbox = { m_soulPosition.x - 4, m_soulPosition.y - 4, 8, 8 };

    for (size_t i = 0; i < m_bullets.size(); ) {
        m_bullets[i].update(dt);

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
    float startX = m_arenaRect.x + (float)GetRandomValue(20, (int)m_arenaRect.width - 20);
    Bullet b;
    b.position = { startX, m_arenaRect.y - 10 };
    b.velocity = { (float)GetRandomValue(-25, 25), (float)GetRandomValue(90, 140) };
    b.type = (GetRandomValue(0, 1) == 0) ? BulletType::UwU : BulletType::SodaCan;
    b.text = "UWU";
    b.color = (b.type == BulletType::UwU) ? (Color){ 255, 120, 210, 255 } : COLOR_UNDERTALE_GREEN;
    b.damage = 3;
    b.furry = 6.0f;
    m_bullets.push_back(b);
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

void BattleSystem::render(GameContext& context) {
    Font font8Bit = context.resources.getFont8Bit(22);
    Font fontSmall = context.resources.getFont8Bit(16);

    // 1. Official Green Grid Background (BUI-002)
    drawBackgroundGrid();

    // 2. Enemy Drawing (Feet aligned at baseline y = 241, BUI-003)
    int enemyCenterX = VIRTUAL_WIDTH / 2;
    int baselineY = ENEMY_BASELINE_Y;

    // Body
    DrawRectangle(enemyCenterX - 35, baselineY - 80, 70, 70, { 50, 50, 70, 255 });
    // Head
    DrawCircle(enemyCenterX, baselineY - 110, 32, { 120, 120, 140, 255 });
    // Ears
    DrawTriangle(
        { (float)enemyCenterX - 30, (float)baselineY - 135 },
        { (float)enemyCenterX - 10, (float)baselineY - 140 },
        { (float)enemyCenterX - 20, (float)baselineY - 165 },
        { 90, 90, 110, 255 }
    );
    DrawTriangle(
        { (float)enemyCenterX + 10, (float)baselineY - 140 },
        { (float)enemyCenterX + 30, (float)baselineY - 135 },
        { (float)enemyCenterX + 20, (float)baselineY - 165 },
        { 90, 90, 110, 255 }
    );
    // Eyes
    DrawCircle(enemyCenterX - 10, baselineY - 114, 5, WHITE);
    DrawCircle(enemyCenterX + 10, baselineY - 114, 5, WHITE);
    DrawCircle(enemyCenterX - 10, baselineY - 114, 2, BLACK);
    DrawCircle(enemyCenterX + 10, baselineY - 114, 2, BLACK);

    // BUI-004: Permanent "Barky HP" is removed! Name only appears in target menus or dialogue.

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
        // ACT menu with Soul cursor (BUI-008)
        const auto& opts = m_enemy->getActOptions();
        for (size_t i = 0; i < opts.size(); ++i) {
            float optY = m_arenaRect.y + 25 + (float)i * 30;
            if (m_subOptionIndex == (int)i) {
                drawSoul(m_arenaRect.x + 35, optY + 10, SOUL_RED);
            }
            context.resources.drawText(font8Bit, "* " + opts[i], { m_arenaRect.x + 55, optY }, 20, WHITE, 1.0f);
        }
    } else if (m_state == BattleState::ItemSelect) {
        // Item menu with Soul cursor (BUI-008, ITM-002)
        if (context.player.inventory.empty()) {
            context.resources.drawText(font8Bit, "* No items in inventory!", { m_arenaRect.x + 40, m_arenaRect.y + 30 }, 20, WHITE, 1.0f);
        } else {
            for (size_t i = 0; i < context.player.inventory.size(); ++i) {
                float optY = m_arenaRect.y + 25 + (float)i * 30;
                if (m_subOptionIndex == (int)i) {
                    drawSoul(m_arenaRect.x + 35, optY + 10, SOUL_RED);
                }
                context.resources.drawText(font8Bit, "* " + context.player.inventory[i], { m_arenaRect.x + 55, optY }, 20, WHITE, 1.0f);
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
        // Narrator message
        context.resources.drawText(font8Bit, m_narratorText, { m_arenaRect.x + 30, m_arenaRect.y + 25 }, 20, WHITE, 1.0f);
    }

    // 5. Official HUD 1:1 (BUI-005)
    drawHUD(context, font8Bit);

    // 6. Action Buttons 1:1 (BUI-006)
    drawButtons(context, font8Bit);
}

} // namespace nundertale
