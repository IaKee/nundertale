#include "combat/BattleSystem.hpp"
#include "core/GameContext.hpp"
#include "core/Constants.hpp"
#include "core/FurReactions.hpp"
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
    updateSoulTrails(dt);
    m_backgroundAnimTimer += dt;

    // BHL-017: KR-style residual damage drain
    if (!isFinished()) {
        int residualDmg = m_residual.update(dt);
        if (residualDmg > 0 && context.player.hp > 1) {
            context.player.hp = std::max(1, context.player.hp - residualDmg);
        }
    }

    // FUR-003: First-appearance FUR label tremble logic
    if (!m_furStartChecked) {
        m_furStartChecked = true;
        m_furVisibleAtStart = furHudVisible(context.state.furry.value);
        if (m_furVisibleAtStart && context.state.getFlag("fur_hud_seen", 0) == 0) {
            m_furTrembleTimer = 0.0f;
            m_furTrembleTriggered = true;
        }
    } else if (!m_furVisibleAtStart && !m_furTrembleTriggered && furHudVisible(context.state.furry.value)) {
        m_furTrembleTimer = 0.0f;
        m_furTrembleTriggered = true;
    }

    if (m_furTrembleTimer >= 0.0f && m_furTrembleTimer < 1.5f) {
        m_furTrembleTimer += dt;
        if (m_furTrembleTimer >= 1.5f) {
            context.state.setFlag("fur_hud_seen", 1);
        }
    }

    if (m_enemy && !m_enemy->isDead() && m_enemyShakeTimer <= 0.0f && !m_isSpared) {
        m_enemyIdleTimer += dt;
    }

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
                m_residual.clear();
            }
            break;
        case BattleState::Fled:
            break;
        case BattleState::DialogMessage:
            if (context.input.isConfirmPressed()) {
                if (m_dialogReturnsToMenu) {
                    m_dialogReturnsToMenu = false;
                    m_state = BattleState::MenuSelect;
                } else if (m_enemy->isDead() || m_isSpared) { // BUG-02
                    m_state = BattleState::Victory;
                    m_residual.clear();
                    if (m_isSpared) {
                        context.player.gold += 10;
                        context.state.setFlag("ruins.barky.spared", 1);
                        context.state.setFlag("ruins.barky.state", 1);
                        context.state.sparesTotal += 1;
                        m_narratorText = "* YOU WON!\n* You earned 0 EXP and 10 gold.";
                        spawnDust({ (float)(VIRTUAL_WIDTH / 2), (float)(ENEMY_BASELINE_Y - 80) }, 25, COLOR_UNDERTALE_YELLOW);
                    } else {
                        context.player.exp += 20;
                        context.player.gold += 15;
                        context.state.setFlag("ruins.barky.killed", 1);
                        context.state.setFlag("ruins.barky.state", 2);
                        context.state.killsTotal += 1;
                        context.state.killsByArea["ruins"] += 1;
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
                            m_narratorText += "\n* Your LEVEL OF VITRIOL increased."; // LORE §8.2 (BUG-22)
                            context.audio.playSound("assets/sfx/heal.ogg", 0.9f);
                        }
                    }
                } else {
                    startEnemyTurn(context);
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

    // FUR-003: If FUR was gained during this frame and hadn't triggered yet
    if (m_furStartChecked && !m_furVisibleAtStart && !m_furTrembleTriggered && furHudVisible(context.state.furry.value)) {
        m_furTrembleTimer = 0.0f;
        m_furTrembleTriggered = true;
    }
    if (isFinished() && m_furTrembleTriggered && context.state.getFlag("fur_hud_seen", 0) == 0) {
        context.state.setFlag("fur_hud_seen", 1);
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
                m_fightPhase = FightPhase::Moving;
                m_fightPhaseTimer = 0.0f;
                // BUG-17: Random entry side
                bool fromLeft = (GetRandomValue(0, 1) == 0);
                float innerLeft = m_arenaRect.x + 5.0f;
                float innerRight = m_arenaRect.x + m_arenaRect.width - 5.0f;
                float innerW = innerRight - innerLeft;
                float speed = attackBarSpeed(innerW);
                m_targetCursorX = fromLeft ? innerLeft : innerRight;
                m_targetCursorSpeed = fromLeft ? speed : -speed;
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
                if (m_enemy->isMercyLocked()) {
                    m_narratorText = "* This feature is not available in your region.";
                    m_dialogReturnsToMenu = true;
                    m_state = BattleState::DialogMessage;
                } else {
                    m_state = BattleState::MercySelect;
                }
                break;
        }
    }
}

void BattleSystem::handleFightTargetInput(float dt, GameContext& context) {
    if (m_fightPhase == FightPhase::Moving) {
        if (context.input.isCancelPressed()) {
            m_state = BattleState::MenuSelect;
            m_targetCursorMoving = false;
            m_fightPhase = FightPhase::Moving;
            m_fightPhaseTimer = 0.0f;
            return;
        }

        if (m_targetCursorMoving) {
            // BUG-08: Use dt instead of GetFrameTime()
            m_targetCursorX += m_targetCursorSpeed * dt;

            float innerLeft = m_arenaRect.x + 5.0f;
            float innerRight = m_arenaRect.x + m_arenaRect.width - 5.0f;

            if (attackBarMissed(m_targetCursorX, m_targetCursorSpeed, innerLeft, innerRight)) {
                m_targetCursorMoving = false;
                m_lastPlayerAction = PlayerTurnAction::FightMiss;
                m_narratorText = "* You swung wide and missed!";
                m_lastMiss = true;
                m_lastDamage = 0;
                m_damageDisplayTimer = 1.2f;
                m_enemyShakeTimer = 0.0f;
                context.audio.playSound("assets/sfx/cartoon_slip.ogg", 0.7f);
                m_fightPhase = FightPhase::Fade;
                m_fightPhaseTimer = 0.0f;
            } else if (context.input.isConfirmPressed()) {
                m_targetCursorMoving = false;
                float center = m_arenaRect.x + m_arenaRect.width / 2.0f;
                float dist = std::abs(m_targetCursorX - center);
                float maxDist = m_arenaRect.width / 2.0f;

                // Damage formula (FGT-003)
                int baseAtk = context.player.at + context.player.weaponAt;
                int enemyDef = m_enemy->getDef();
                float accuracyMult = (dist <= ATTACK_CRIT_RADIUS) ? 2.2f : std::max(0.5f, (1.0f - dist / maxDist) * 2.0f);
                int damage = std::max(1, static_cast<int>(std::round(static_cast<float>(baseAtk - enemyDef + GetRandomValue(1, 3)) * accuracyMult)));

                m_enemyVisualHp = static_cast<float>(m_enemy->getHp());
                m_enemy->takeDamage(damage);

                m_lastMiss = false;
                m_lastDamage = damage;
                m_damageDisplayTimer = 1.5f;
                m_enemyShakeTimer = 0.4f;
                m_slashAnimTimer = 0.25f;

                // FGT-008: Hitmarker on critical hit, slice sound on normal hit
                if (dist <= ATTACK_CRIT_RADIUS) {
                    m_lastPlayerAction = PlayerTurnAction::FightCrit;
                    context.audio.playSound("assets/sfx/u_activate.wav", 0.9f);
                    context.shake(5.0f, 0.22f); // GFX-006: Critical screen shake
                    context.flash(WHITE, 0.12f); // GFX-007: Impact flash
                    m_narratorText = "* CRITICAL HIT! Dealt " + std::to_string(damage) + " cringe damage!";
                } else {
                    m_lastPlayerAction = PlayerTurnAction::FightHit;
                    context.audio.playSound("assets/sfx/u1.ogg", 0.8f);
                    context.shake(2.0f, 0.1f);
                    m_narratorText = "* You dealt " + std::to_string(damage) + " cringe damage to " + m_enemy->getName() + "!";
                }

                if (m_enemy->isDead()) {
                    m_narratorText += "\n* Only a pile of synthetic wolf fur remains."; // LORE §19.1
                    spawnDust({ (float)(VIRTUAL_WIDTH / 2), (float)(ENEMY_BASELINE_Y - 80) }, 25, WHITE);
                }

                m_fightPhase = FightPhase::HitHold;
                m_fightPhaseTimer = 0.0f;
            }
        }
    } else if (m_fightPhase == FightPhase::HitHold) {
        m_fightPhaseTimer += dt;
        if (m_fightPhaseTimer >= ATTACK_HIT_HOLD_SECONDS) {
            m_fightPhase = FightPhase::Fade;
            m_fightPhaseTimer = 0.0f;
        }
    } else if (m_fightPhase == FightPhase::Fade) {
        m_fightPhaseTimer += dt;
        if (m_fightPhaseTimer >= ATTACK_TARGET_FADE_SECONDS) {
            m_state = BattleState::DialogMessage;
            m_fightPhase = FightPhase::Moving;
            m_fightPhaseTimer = 0.0f;
        }
    }
}

std::vector<int> BattleSystem::getAvailableActIndices(const GameContext& context) const {
    std::vector<int> indices;
    const auto& acts = m_enemy->getActs();
    for (size_t i = 0; i < acts.size(); ++i) {
        if (actAvailable(acts[i], context.player.inventory, context.state.furry.value,
                         [&context](const std::string& flag) { return context.state.hasFlag(flag); })) {
            indices.push_back(static_cast<int>(i));
        }
    }
    return indices;
}

void BattleSystem::handleActInput(GameContext& context) {
    const auto& acts = m_enemy->getActs();
    const std::vector<int> availableActs = getAvailableActIndices(context);
    if (availableActs.empty()) {
        m_state = BattleState::MenuSelect;
        return;
    }

    int actCount = static_cast<int>(availableActs.size());
    m_subOptionIndex = std::clamp(m_subOptionIndex, 0, actCount - 1);
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
        const auto& act = acts[static_cast<size_t>(availableActs[static_cast<size_t>(m_subOptionIndex)])];
        int useCount = ++m_actUseCounts[act.id];
        bool applies = actEffectApplies(useCount, act.minUses);

        m_lastPlayerAction = PlayerTurnAction::Act;
        m_lastActionDetail = act.id;
        m_lastActResponse = actResponseForUse(act, useCount);

        if (act.id == "check") {
            // BUG-06 & ACT-001: Data-driven check text matching enemy instance stats
            m_narratorText = m_enemy->getCheckText();
            m_enemySpeechBubble = "";
        } else {
            m_narratorText = actTextForUse(act, useCount);
            m_enemySpeechBubble = actResponseForUse(act, useCount);
            bool wasSpareable = m_enemy->isSpareable();
            if (applies && act.fur != 0) {
                // ACT-005: Accept Hug adds FUR (FUR-002/FUR-005 feedback)
                const FurChange furChange = applyFur(context.state.furry.value, context.state.furry.stageMax, act.fur);
                if (furChange.stageUp) {
                    const Color fluffPink{ 255, 170, 220, 255 };
                    spawnDust(m_soulPosition, 18, fluffPink);
                    context.flash(fluffPink, 0.12f);
                    m_narratorText += "\n* You feel... fluffier.";
                }
                if (furChange.reachedMirror) {
                    context.state.setFlag("fur_mirror_pending", 1);
                }
            }
            if (applies && act.spareNow) {
                // ACT-005: spares on the spot; the dialog then ends the battle as a spare
                m_isSpared = true;
                spawnDust({ (float)(VIRTUAL_WIDTH / 2), (float)(ENEMY_BASELINE_Y - 80) }, 15, COLOR_UNDERTALE_YELLOW);
            }
            if (applies) {
                m_enemy->addMercy(act.mercy);
                if (act.spareable) {
                    m_enemy->setSpareable(true);
                }
                m_enemy->modifyAtk(act.atkDelta);
                m_enemy->modifyDef(act.defDelta);
                m_frozenTurns += std::max(0, act.freezeTurns);
            }
            // BUI-012: optional ding the moment the name turns yellow (placeholder SFX until AUD-011)
            if (!wasSpareable && m_enemy->isSpareable() && context.save.getConfig().spareDing) {
                context.audio.playSound("assets/sfx/u_activate.wav", 0.5f);
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
        const ItemDef* itemDef = context.itemDatabase.findItemByName(item);
        bool gainedFurStage = false;

        m_lastPlayerAction = PlayerTurnAction::Item;
        m_lastActionDetail = item;

        if (item == "Bottle") {
            m_narratorText = "* You consider drinking from the bottle.\n* ...No. Absolutely not.";
            context.audio.playSound("assets/sfx/selection_sound.wav", 0.8f);
            m_state = BattleState::DialogMessage;
            return;
        }

        context.player.inventory.erase(context.player.inventory.begin() + m_subOptionIndex);

        // PRG-002: Equip weapon/armor or consume item
        if ((itemDef != nullptr && itemDef->type == "weapon") ||
            (itemDef == nullptr && (item.find("Mouse") != std::string::npos || item.find("Keyboard") != std::string::npos || item.find("Weapon") != std::string::npos))) {
            std::string oldWeapon = context.player.weapon;
            context.player.weapon = item;
            context.player.weaponAt = (itemDef != nullptr) ? itemDef->at : ((item.find("Keyboard") != std::string::npos) ? 4 : 2);
            context.player.inventory.push_back(oldWeapon);
            m_narratorText = "* You equipped the " + item + "!\n* Attack power updated!";
        } else if ((itemDef != nullptr && itemDef->type == "armor") ||
                   (itemDef == nullptr && (item.find("Headset") != std::string::npos || item.find("Hoodie") != std::string::npos || item.find("Armor") != std::string::npos))) {
            std::string oldArmor = context.player.armor;
            context.player.armor = item;
            context.player.armorDf = (itemDef != nullptr) ? itemDef->df : ((item.find("Hoodie") != std::string::npos) ? 3 : 1);
            context.player.inventory.push_back(oldArmor);
            m_narratorText = "* You equipped the " + item + "!\n* Defense updated!";
            if (itemDef != nullptr && itemDef->furry > 0 && !itemDef->id.empty()) {
                const std::string flag = "fur_on_equip." + itemDef->id;
                if (!context.state.hasFlag(flag)) {
                    const FurChange furChange = applyFur(context.state.furry.value, context.state.furry.stageMax, itemDef->furry);
                    context.state.setFlag(flag, 1);
                    gainedFurStage = furChange.stageUp;
                    if (furChange.reachedMirror) {
                        context.state.setFlag("fur_mirror_pending", 1);
                    }
                }
            }
        } else if (item.find("Dew") != std::string::npos) {
            context.player.hp = std::min(context.player.maxHp, context.player.hp + 10);
            m_soulSpeedBuffTurns = 2; // ITM-005: +35% speed buff for 2 enemy turns
            m_narratorText = "* You chugged the warm Mountain Dew!\n* You recovered 10 HP! Citrus speed surge!";
        } else if (item.find("Doritos") != std::string::npos) {
            context.player.hp = std::min(context.player.maxHp, context.player.hp + 6);
            m_narratorText = "* You devoured the stale Doritos!\n* Pure nostalgia. You recovered 6 HP!";
        } else if (item.find("Duct Tape") != std::string::npos || item.find("Tape") != std::string::npos) {
            applyFur(context.state.furry.value, context.state.furry.stageMax, -15);
            m_narratorText = "* You aggressively taped down your sprouting fur!\n* Ouch! Furry meter reduced by 15%!";
        } else {
            context.player.hp = std::min(context.player.maxHp, context.player.hp + 8);
            m_narratorText = "* You used " + item + "!\n* Recovered 8 HP.";
        }

        if (context.player.hp >= context.player.maxHp && item.find("Tape") == std::string::npos && item.find("Mouse") == std::string::npos && item.find("Headset") == std::string::npos) {
            m_narratorText += "\n* Your HP was maxed out.";
        }
        if (gainedFurStage) {
            m_narratorText += "\n* You feel... fluffier.";
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
                m_narratorText = "* You spared " + m_enemy->getName() + ".";
                m_enemySpeechBubble = "You can tell no one about the zipper. Pack secret."; // LORE §19.1
                spawnDust({ (float)(VIRTUAL_WIDTH / 2), (float)(ENEMY_BASELINE_Y - 80) }, 15, COLOR_UNDERTALE_YELLOW);
                context.audio.playSound("assets/sfx/selection_sound.wav", 1.0f);
            } else {
                m_lastPlayerAction = PlayerTurnAction::MercySpareFail;
                m_narratorText = "* You spared " + m_enemy->getName() + ".\n* But their name wasn't yellow...";
            }
            m_state = BattleState::DialogMessage;
        } else { // Flee (MRC-002)
            float fleeChance = 0.5f + 0.1f * static_cast<float>(m_turnCount);
            bool success = (GetRandomValue(1, 100) <= static_cast<int>(fleeChance * 100.0f));
            if (success) {
                m_state = BattleState::Fleeing;
                context.state.fleesTotal += 1;
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

void BattleSystem::startEnemyTurn(GameContext& context) {
    m_turnCount++;

    if (m_frozenTurns > 0) {
        m_frozenTurns--;
        m_bullets.clear();
        m_zones.clear();
        m_soulTrails.clear();
        if (m_soulSpeedBuffTurns > 0) {
            m_soulSpeedBuffTurns--;
        }
        m_narratorText = m_enemy->getFlavorText(m_lastActionDetail);
        m_enemySpeechBubble = "";
        m_lastPlayerAction = PlayerTurnAction::None;
        m_lastActResponse = "";
        m_state = BattleState::MenuSelect;
        return;
    }

    m_state = BattleState::EnemyTurn;
    m_enemyTurnTimer = 0.0f;
    m_spawnTimer = 0.0f;
    m_bullets.clear();
    m_zones.clear();
    m_ribbitState = RibbitState{};
    const auto cringeUses = m_actUseCounts.find("cringe_check");
    const bool howlUnlocked = cringeUses != m_actUseCounts.end() && cringeUses->second > 0;
    const bool howlJustUnlocked = howlUnlocked && !m_howlUnlocked;
    m_barkyPattern = barkyPatternForTurn(m_turnCount - 1, howlUnlocked, howlJustUnlocked);
    m_howlUnlocked = howlUnlocked;
    m_howlWallTimer = 1000.0f; // first wall of a turn spawns immediately (BHL-014 tuning)
    // Center soul in arena
    m_soulPosition = { 320.0f, 320.0f };

    // BFL-003: Contextual dialogue based on player's action
    std::string trigger = "";
    if (m_lastPlayerAction == PlayerTurnAction::FightCrit) {
        trigger = "crit";
    } else if (m_lastPlayerAction == PlayerTurnAction::FightHit) {
        trigger = "hit";
    } else if (m_lastPlayerAction == PlayerTurnAction::FightMiss) {
        trigger = "miss";
    } else if (m_lastPlayerAction == PlayerTurnAction::Item) {
        trigger = "item";
    }

    const bool hasQueuedActResponse = !m_lastActResponse.empty();
    const std::string furReaction = furReactionLine(context.state.furry.value, "barky");
    if (!m_furReactionSaid && !hasQueuedActResponse && !furReaction.empty()) {
        m_enemySpeechBubble = furReaction;
        m_furReactionSaid = true;
    } else {
        m_enemySpeechBubble = m_enemy->getDialogueForTrigger(trigger, m_lastActResponse);
    }

    // Reset action state for the turn
    m_lastPlayerAction = PlayerTurnAction::None;
    m_lastActResponse = "";
}

bool BattleSystem::applySoulDamage(GameContext& context, int rawDamage, int fur) {
    int reducedDamage = rawDamage + m_enemy->getAtk() - (context.player.df + context.player.armorDf);
    int finalDamage = std::max(1, reducedDamage);

    context.player.hp -= finalDamage;
    float armorMult = 1.0f;
    const ItemDef* armorDef = context.itemDatabase.findItemByName(context.player.armor);
    if (armorDef != nullptr) {
        armorMult = armorDef->furMult;
    }
    const FurChange furChange = applyFur(
        context.state.furry.value,
        context.state.furry.stageMax,
        fur,
        armorMult);
    context.audio.playSound("assets/sfx/pipipipopopo_backwards.ogg", 0.6f);
    context.shake(6.0f, 0.25f);
    context.flash({ 255, 0, 0, 255 }, 0.15f);

    if (furChange.stageUp) {
        const Color fluffPink{ 255, 170, 220, 255 };
        spawnDust(m_soulPosition, 18, fluffPink);
        context.flash(fluffPink, 0.12f);
        m_furStageUpPending = true;
    }
    if (furChange.reachedMirror) {
        context.state.setFlag("fur_mirror_pending", 1);
    }

    if (context.player.hp <= 0) {
        context.player.hp = 0;
        m_state = BattleState::Defeat;
        m_residual.clear();
        m_furStageUpPending = false;
        m_narratorText.clear(); // BUG-21: no narrator line, straight to the FURRY OVER sequence
        context.audio.playSound("assets/sfx/areusure.ogg", 1.0f);
        return true;
    }

    return false;
}

void BattleSystem::updateEnemyTurn(float dt, GameContext& context) {
    m_enemyTurnTimer += dt;
    m_howlWallTimer += dt;
    if (m_enemyTurnTimer >= m_enemyTurnDuration) {
        m_bullets.clear();
        m_zones.clear();
        m_soulTrails.clear();
        if (m_soulSpeedBuffTurns > 0) {
            m_soulSpeedBuffTurns--;
        }

        if (m_furStageUpPending) {
            m_narratorText = "* You feel... fluffier.";
            m_furStageUpPending = false;
        } else {
            // BFL-002: Query turn flavor text from enemy pools without repeating
            m_narratorText = m_enemy->getFlavorText(m_lastActionDetail);
        }
        m_state = BattleState::MenuSelect;
        return;
    }

    // Soul movement inside dodge box (BHL-006: 4 px/frame at 30 fps = 120 px/s)
    // ITM-005: Mountain Dew speed surge (+35%)
    float currentSpeed = (m_soulSpeedBuffTurns > 0) ? (m_soulSpeed * 1.35f) : m_soulSpeed;

    Vector2 prevSoulPos = m_soulPosition;

    Vector2 moveDir = { 0, 0 };
    if (context.input.isLeft()) moveDir.x -= 1.0f;
    if (context.input.isRight()) moveDir.x += 1.0f;
    if (context.input.isUp()) moveDir.y -= 1.0f;
    if (context.input.isDown()) moveDir.y += 1.0f;

    if (moveDir.x != 0.0f || moveDir.y != 0.0f) {
        m_soulPosition.x += moveDir.x * currentSpeed * dt;
        m_soulPosition.y += moveDir.y * currentSpeed * dt;

        if (m_soulSpeedBuffTurns > 0) {
            m_soulTrails.push_back({ m_soulPosition, 0.55f });
        }
    }

    // Clamp inside arena with margin
    float soulPadding = 12.0f;
    m_soulPosition.x = std::clamp(m_soulPosition.x, m_arenaRect.x + soulPadding, m_arenaRect.x + m_arenaRect.width - soulPadding);
    m_soulPosition.y = std::clamp(m_soulPosition.y, m_arenaRect.y + soulPadding, m_arenaRect.y + m_arenaRect.height - soulPadding);

    bool soulMoving = (m_soulPosition.x != prevSoulPos.x || m_soulPosition.y != prevSoulPos.y);

    // BHL-015: "thinking" ribbit easter-egg bubble over soul when still for 3.0s
    if (updateRibbit(m_ribbitState, dt, soulMoving)) {
        context.audio.playSound("assets/sfx/selection_sound.wav", 0.3f);
    }

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

        if (m_bullets[i].active && CheckCollisionRecs(soulHitbox, m_bullets[i].getBounds())) {
            if (m_bullets[i].healsSoul()) {
                context.player.hp = std::min(context.player.maxHp, context.player.hp + m_bullets[i].heal);
                context.audio.playSound("assets/sfx/heal.ogg", 0.8f);
                m_bullets.erase(m_bullets.begin() + i);
                continue;
            } else if (m_bullets[i].hurtsSoul(soulMoving) && m_invulnTimer <= 0.0f) {
                m_invulnTimer = 1.0f; // 30 frames of i-frame
                if (applySoulDamage(context, m_bullets[i].damage, m_bullets[i].fur)) {
                    return;
                }
            }
        }
        ++i;
    }

    // BHL-016: persistent zones tick independently of bullet invulnerability.
    for (size_t i = 0; i < m_zones.size(); ) {
        bool soulInside = CheckCollisionRecs(soulHitbox, m_zones[i].area);
        int damage = m_zones[i].update(dt, soulInside);
        int fur = static_cast<int>(std::lround(m_zones[i].furPerTick));
        bool expired = m_zones[i].expired();

        if (damage > 0 && applySoulDamage(context, damage, fur)) {
            return;
        }

        if (expired) {
            m_zones.erase(m_zones.begin() + i);
            continue;
        }

        ++i;
    }
}

void BattleSystem::spawnBulletWave() {
    if (m_barkyPattern == BarkyPattern::UwuRain) {
        // Pattern 1: UwU Rain (BHL-014: falling letters with slight zigzag)
        float startX = m_arenaRect.x + (float)GetRandomValue(15, (int)m_arenaRect.width - 25);
        Bullet b;
        b.position = { startX, m_arenaRect.y - 12.0f };
        b.velocity = { (float)GetRandomValue(-20, 20), (float)GetRandomValue(95, 135) };
        b.type = BulletType::UwU;
        b.text = "UWU";
        b.color = Color{ 255, 120, 210, 255 };
        b.damage = 3;
        b.fur = 6;
        m_bullets.push_back(b);
    } else if (m_barkyPattern == BarkyPattern::DewBounce) {
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
        b.fur = 4;
        m_bullets.push_back(b);
    } else {
        // Pattern 3: Howl Wave (BHL-014 tuning). Next wall ready when previous has travelled >= 60% box width and >= 1.4s.
        constexpr float wallSpeed = 130.0f;
        const float travelled = m_howlWallTimer * wallSpeed;
        if (!howlNextWallReady(travelled, m_arenaRect.width, m_howlWallTimer)) {
            return;
        }

        constexpr float arcRadius = 10.0f;
        constexpr float gapHeight = 48.0f;
        constexpr float rowStep = 18.0f;
        const float innerTop = m_arenaRect.y + 10.0f;
        const float innerBottom = m_arenaRect.y + m_arenaRect.height - 10.0f;
        const int gapMin = static_cast<int>(innerTop);
        const int gapMax = static_cast<int>(innerBottom - gapHeight);
        const float gapTop = static_cast<float>(GetRandomValue(gapMin, gapMax));
        const float startX = m_howlFromLeft
            ? m_arenaRect.x - arcRadius
            : m_arenaRect.x + m_arenaRect.width + arcRadius;
        const float velocityX = m_howlFromLeft ? wallSpeed : -wallSpeed;

        for (float y = innerTop; y <= innerBottom; y += rowStep) {
            // Include the arc hitbox margin so the full 48 px gap remains reachable.
            if (howlGapContains(gapTop - arcRadius, gapHeight + arcRadius * 2.0f, y)) {
                continue;
            }

            Bullet b;
            b.position = { startX, y };
            b.velocity = { velocityX, 0.0f };
            b.type = BulletType::Awoo;
            b.arcRadius = arcRadius;
            b.color = Color{ 100, 200, 255, 255 };
            b.damage = 3;
            b.fur = 5;
            m_bullets.push_back(b);
        }

        m_howlFromLeft = !m_howlFromLeft;
        m_howlWallTimer = 0.0f;
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
    int barW = std::max(20, (int)(static_cast<float>(context.player.maxHp) * 1.2f));
    DrawRectangle(HUD_HP_BAR_X, HUD_Y, barW, HUD_HP_BAR_H, HP_RED);

    float hpPct = std::clamp((float)context.player.hp / (float)context.player.maxHp, 0.0f, 1.0f);
    int fillW = (int)(static_cast<float>(barW) * hpPct);
    DrawRectangle(HUD_HP_BAR_X, HUD_Y, fillW, HUD_HP_BAR_H, HP_YELLOW);

    // BHL-017: Magenta residual overlay on the right end of the yellow fill (the HP that is about to drain, like KR)
    if (m_residual.pending > 0.0f && context.player.maxHp > 0) {
        float pendingPct = m_residual.pending / static_cast<float>(context.player.maxHp);
        int resW = static_cast<int>(static_cast<float>(barW) * pendingPct);
        int maxAllowedW = fillW;
        int clampedW = std::clamp(resW, 0, maxAllowedW);
        if (clampedW > 0) {
            DrawRectangle(HUD_HP_BAR_X + fillW - clampedW, HUD_Y, clampedW, HUD_HP_BAR_H, Color{ 255, 0, 255, 255 });
        }
    }

    int jitter = furHudJitter(m_furTrembleTimer);

    // Furry Karma overlay (FUR-003) if FUR > 0
    if (furHudVisible(context.state.furry.value)) {
        float furryPct = std::clamp(static_cast<float>(context.state.furry.value) / static_cast<float>(FUR_MAX), 0.0f, 1.0f);
        int furW = (int)(static_cast<float>(barW) * furryPct);
        DrawRectangle(HUD_HP_BAR_X + barW - furW + jitter, HUD_Y + jitter, furW, HUD_HP_BAR_H, KR_PURPLE);
    }

    // 5. HP Numbers after bar (with FUR label if FUR > 0)
    int barEnd = HUD_HP_BAR_X + barW;
    int numX = barEnd + 16;

    if (furHudVisible(context.state.furry.value)) {
        float furLabelX = static_cast<float>(barEnd + 10);
        float furLabelY = static_cast<float>(HUD_Y + 4);
        context.resources.drawText(font, "FUR", { furLabelX + static_cast<float>(jitter), furLabelY + static_cast<float>(jitter) }, 14, KR_PURPLE, 1.0f);

        float furW = context.resources.measureTextWidth(font, "FUR", 14, 1.0f);
        numX = static_cast<int>(furLabelX + furW + 10.0f);
    }

    std::string hpStr = std::to_string(context.player.hp) + " / " + std::to_string(context.player.maxHp);
    Color numCol = furHudVisible(context.state.furry.value) ? KR_PURPLE : WHITE;
    context.resources.drawText(font, hpStr, { static_cast<float>(numX), static_cast<float>(HUD_Y) }, 20, numCol, 1.0f);
}

void BattleSystem::drawButtons(GameContext& context, Font font) const {
    const char* btnLabels[4] = { "FIGHT", "ACT", "ITEM", "MERCY" };
    int btnX[4] = { BATTLE_BUTTON_X_FIGHT, BATTLE_BUTTON_X_ACT, BATTLE_BUTTON_X_ITEM, BATTLE_BUTTON_X_MERCY };

    for (int i = 0; i < 4; ++i) {
        Rectangle bRec = { (float)btnX[i], (float)BATTLE_BUTTON_Y, (float)BATTLE_BUTTON_W, (float)BATTLE_BUTTON_H };
        bool isSelected = (m_selectedButton == static_cast<MainButton>(i));

        bool mercyLocked = (i == static_cast<int>(MainButton::Mercy) && m_enemy->isMercyLocked());
        Color bColor = mercyLocked ? Color{ 128, 128, 128, 255 } : (isSelected ? UI_YELLOW : UI_ORANGE);
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

void BattleSystem::updateSoulTrails(float dt) {
    for (size_t i = 0; i < m_soulTrails.size(); ) {
        m_soulTrails[i].alpha -= 3.5f * dt;
        if (m_soulTrails[i].alpha <= 0.0f) {
            m_soulTrails.erase(m_soulTrails.begin() + i);
        } else {
            ++i;
        }
    }
}

void BattleSystem::drawSoulTrails() const {
    for (const auto& t : m_soulTrails) {
        Color c = Fade(COLOR_UNDERTALE_GREEN, std::clamp(t.alpha, 0.0f, 1.0f) * 0.5f);
        drawSoul(t.pos.x, t.pos.y, c);
    }
}

void BattleSystem::drawAttackTarget() const {
    int innerX = static_cast<int>(m_arenaRect.x) + 5;
    int innerY = static_cast<int>(m_arenaRect.y) + 5;
    int innerW = std::max(1, static_cast<int>(m_arenaRect.width) - 10);
    int innerH = std::max(1, static_cast<int>(m_arenaRect.height) - 10);

    float squish = 1.0f;
    if (m_fightPhase == FightPhase::Fade) {
        squish = attackTargetSquish(m_fightPhaseTimer);
    }

    BeginScissorMode(innerX, innerY, innerW, innerH);

    // Black background of the target
    DrawRectangle(innerX, innerY, innerW, innerH, BLACK);

    float centerX = static_cast<float>(innerX) + static_cast<float>(innerW) * 0.5f;
    float centerY = static_cast<float>(innerY) + static_cast<float>(innerH) * 0.5f;

    float radiusX = (static_cast<float>(innerW) * 0.5f - 1.0f) * squish;
    float radiusY = static_cast<float>(innerH) * 0.5f - 1.0f;

    if (squish > 0.005f && radiusX >= 1.0f) {
        // Vertical lines (bands): 8 symmetric pairs narrowing toward center
        for (int i = 1; i <= 8; ++i) {
            float t = static_cast<float>(i) / 9.0f;
            float ratio = std::pow(t, 1.4f);
            float baseDx = (static_cast<float>(innerW) * 0.5f - 1.0f) * ratio;
            float dx = baseDx * squish;

            if (dx < radiusX) {
                float halfH = radiusY * std::sqrt(1.0f - (dx * dx) / (radiusX * radiusX));
                Color lineColor = { 0, 160, 0, 255 };

                // Right symmetric line
                DrawLineEx({ centerX + dx, centerY - halfH }, { centerX + dx, centerY + halfH }, 1.0f, lineColor);
                // Left symmetric line
                DrawLineEx({ centerX - dx, centerY - halfH }, { centerX - dx, centerY + halfH }, 1.0f, lineColor);
            }
        }

        // Center band: slightly brighter light green translucent strip
        float bandW = 14.0f * squish;
        if (bandW > 1.0f) {
            DrawRectangle(static_cast<int>(centerX - bandW * 0.5f), innerY, static_cast<int>(bandW), innerH, Color{ 0, 180, 0, 45 });
        }

        // White center line
        DrawLineEx({ centerX, centerY - radiusY }, { centerX, centerY + radiusY }, 2.0f, WHITE);

        // White ellipse outline (2 px thick)
        DrawEllipseLines(static_cast<int>(std::round(centerX)), static_cast<int>(std::round(centerY)), radiusX, radiusY, WHITE);
        if (radiusX > 1.5f && radiusY > 1.5f) {
            DrawEllipseLines(static_cast<int>(std::round(centerX)), static_cast<int>(std::round(centerY)), radiusX - 1.0f, radiusY - 1.0f, WHITE);
        }
    }

    // Bar drawing:
    // Bar is drawn during Moving and HitHold phases (in HitHold, blinks white/black).
    if (m_fightPhase == FightPhase::Moving || m_fightPhase == FightPhase::HitHold) {
        bool isWhite = (m_fightPhase == FightPhase::Moving) ? true : attackBarBlinkWhite(m_fightPhaseTimer);

        Color barFill = isWhite ? WHITE : BLACK;
        Color barOutline = isWhite ? BLACK : Color{ 30, 30, 30, 255 };

        Rectangle barRect = { m_targetCursorX - 7.0f, static_cast<float>(innerY), 14.0f, static_cast<float>(innerH) };
        DrawRectangleRec(barRect, barFill);
        DrawRectangleLinesEx(barRect, 2.0f, barOutline);
    }

    EndScissorMode();
}

void BattleSystem::render(GameContext& context) {
    Font font8Bit = context.resources.getFont8Bit(22);
    Font fontSmall = context.resources.getFont8Bit(16);

    // 1. Battle Background (BUI-002, BUI-013)
    drawBattleBackground(m_background, m_backgroundAnimTimer);

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
        Color tailColor = { 70, 70, 90, 255 };

        // BUI-011: Spared enemy stays translucent (~50% opacity) and pauses idle
        if (m_isSpared) {
            bodyColor.a = 120;
            headColor.a = 120;
            earColor.a  = 120;
            tailColor.a = 120;
        }

        // BUI-010: Idle animation (tail sway, ear twitch, breathing bob)
        float breathBob = (m_enemyShakeTimer <= 0.0f && !m_isSpared) ? std::sin(m_enemyIdleTimer * 2.8f) * 2.0f : 0.0f;
        float earSway   = (m_enemyShakeTimer <= 0.0f && !m_isSpared) ? std::sin(m_enemyIdleTimer * 3.5f) * 2.5f : 0.0f;
        float tailSway  = (m_enemyShakeTimer <= 0.0f && !m_isSpared) ? std::sin(m_enemyIdleTimer * 4.2f) * 12.0f : 0.0f;

        // Tail behind body
        DrawTriangle(
            { (float)enemyCenterX - 28.0f, (float)baselineY - 50.0f + breathBob },
            { (float)enemyCenterX - 36.0f, (float)baselineY - 26.0f + breathBob },
            { (float)enemyCenterX - 56.0f + tailSway, (float)baselineY - 58.0f + breathBob },
            tailColor
        );

        // Body with breathing bob
        DrawRectangle(enemyCenterX - 35, baselineY - 80 + (int)breathBob, 70, 70 - (int)breathBob, bodyColor);

        // Head with breathing bob
        int headCenterY = baselineY - 110 + (int)breathBob;
        DrawCircle(enemyCenterX, headCenterY, 32, headColor);

        // Ears with subtle twitch/sway
        DrawTriangle(
            { (float)enemyCenterX - 30, (float)headCenterY - 25 },
            { (float)enemyCenterX - 10, (float)headCenterY - 30 },
            { (float)enemyCenterX - 20 + earSway, (float)headCenterY - 55 },
            earColor
        );
        DrawTriangle(
            { (float)enemyCenterX + 10, (float)headCenterY - 30 },
            { (float)enemyCenterX + 30, (float)headCenterY - 25 },
            { (float)enemyCenterX + 20 - earSway, (float)headCenterY - 55 },
            earColor
        );

        // Eyes
        DrawCircle(enemyCenterX - 10, headCenterY - 4, 5, m_isSpared ? Fade(WHITE, 0.5f) : WHITE);
        DrawCircle(enemyCenterX + 10, headCenterY - 4, 5, m_isSpared ? Fade(WHITE, 0.5f) : WHITE);
        DrawCircle(enemyCenterX - 10, headCenterY - 4, 2, m_isSpared ? Fade(BLACK, 0.5f) : BLACK);
        DrawCircle(enemyCenterX + 10, headCenterY - 4, 2, m_isSpared ? Fade(BLACK, 0.5f) : BLACK);
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

    // TXT-008: Enemy Speech Bubble with auto-sizing and tail pointer
    // The bubble also shows on the spare dialog (Barky's spare / Accept Hug lines, LORE §19.1)
    const bool showBubble = m_state == BattleState::EnemyTurn || (m_state == BattleState::DialogMessage && m_isSpared);
    if (showBubble && !m_enemySpeechBubble.empty()) {
        float textW = context.resources.measureTextWidth(fontSmall, m_enemySpeechBubble, 16, 1.0f);
        float bubbleW = std::clamp(textW + 24.0f, 100.0f, 240.0f);
        float bubbleH = 48.0f;
        if (textW > 220.0f) {
            bubbleH = 64.0f;
        }
        Rectangle bubbleRect = { (float)enemyCenterX + 45.0f, (float)baselineY - 165.0f, bubbleW, bubbleH };

        DrawRectangleRec(bubbleRect, WHITE);
        DrawRectangleLinesEx(bubbleRect, 2.0f, BLACK);

        // Tail pointer pointing toward Barky's head
        Vector2 p1 = { bubbleRect.x, bubbleRect.y + 16.0f };
        Vector2 p2 = { bubbleRect.x, bubbleRect.y + 32.0f };
        Vector2 p3 = { bubbleRect.x - 14.0f, bubbleRect.y + 24.0f };
        DrawTriangle(p1, p2, p3, WHITE);
        DrawLineEx(p1, p3, 2.0f, BLACK);
        DrawLineEx(p2, p3, 2.0f, BLACK);

        context.resources.drawText(fontSmall, m_enemySpeechBubble, { bubbleRect.x + 10.0f, bubbleRect.y + 14.0f }, 16, BLACK, 1.0f);
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
            for (const auto& zone : m_zones) {
                DrawRectangleRec(zone.area, zone.color);
            }
            for (const auto& b : m_bullets) {
                b.render();
            }
        EndScissorMode();

        // ITM-005: Draw speed buff soul trails
        drawSoulTrails();

        // Soul Heart inside arena (flashing when invulnerable)
        if (m_invulnTimer <= 0.0f || (int)(GetTime() * 15.0) % 2 == 0) {
            drawSoul(m_soulPosition.x, m_soulPosition.y, SOUL_RED);
        }

        // BHL-015: "thinking" ribbit text over soul when still
        if (ribbitVisible(m_ribbitState)) {
            Font fontTiny = context.resources.getFont8Bit(12);
            float bubbleW = 44.0f;
            float bubbleH = 16.0f;
            Rectangle bubbleRect = { m_soulPosition.x + 8.0f, m_soulPosition.y - 20.0f, bubbleW, bubbleH };

            DrawRectangleRec(bubbleRect, WHITE);
            DrawRectangleLinesEx(bubbleRect, 1.0f, BLACK);

            // Tail pointing down-left toward soul
            Vector2 p1 = { bubbleRect.x + 2.0f, bubbleRect.y + bubbleRect.height };
            Vector2 p2 = { bubbleRect.x + 7.0f, bubbleRect.y + bubbleRect.height };
            Vector2 p3 = { bubbleRect.x - 3.0f, bubbleRect.y + bubbleRect.height + 5.0f };
            DrawTriangle(p1, p2, p3, WHITE);
            DrawLineEx(p1, p3, 1.0f, BLACK);
            DrawLineEx(p2, p3, 1.0f, BLACK);

            context.resources.drawText(fontTiny, "ribbit", { bubbleRect.x + 5.0f, bubbleRect.y + 2.0f }, 12.0f, BLACK, 1.0f);
        }
    } else if (m_state == BattleState::FightTarget) {
        drawAttackTarget();
    } else if (m_state == BattleState::ActSelect) {
        // ACT menu 2-column layout (BUI-008, ACT-001)
        const auto& acts = m_enemy->getActs();
        const std::vector<int> availableActs = getAvailableActIndices(context);
        for (size_t i = 0; i < availableActs.size(); ++i) {
            int col = (int)(i % 2);
            int row = (int)(i / 2);
            float optX = (col == 0) ? (m_arenaRect.x + 55.0f) : (m_arenaRect.x + 300.0f);
            float optY = m_arenaRect.y + 25.0f + (float)row * 35.0f;
            if (m_subOptionIndex == (int)i) {
                drawSoul(optX - 20.0f, optY + 10.0f, SOUL_RED);
            }
            const ActDef& act = acts[static_cast<size_t>(availableActs[i])];
            context.resources.drawText(font8Bit, "* " + act.label, { optX, optY }, 20, WHITE, 1.0f);
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

    // 8. Hitbox Debug Overlay (QA-002)
    if (context.debugHitboxes) {
        if (m_state == BattleState::EnemyTurn) {
            Rectangle soulHb = { m_soulPosition.x - 4.0f, m_soulPosition.y - 4.0f, 8.0f, 8.0f };
            DrawRectangleLinesEx(soulHb, 1.0f, RED);
            for (const auto& b : m_bullets) {
                DrawRectangleLinesEx(b.getBounds(), 1.0f, COLOR_UNDERTALE_GREEN);
            }
        }
    }
}

} // namespace nundertale
