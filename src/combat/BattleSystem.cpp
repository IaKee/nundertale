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

    switch (m_state) {
        case BattleState::MenuSelect:
            handleMenuInput(context);
            break;
        case BattleState::FightTarget:
            handleFightTargetInput(context);
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
                if (m_enemy->isDead() || m_enemy->isSpareable()) {
                    m_state = BattleState::Victory;
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
            if (context.input.isConfirmPressed()) {
                // Done
            }
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
            case MainButton::Fight:
                m_state = BattleState::FightTarget;
                m_targetCursorX = m_arenaRect.x;
                m_targetCursorMoving = true;
                break;
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

void BattleSystem::handleFightTargetInput(GameContext& context) {
    if (m_targetCursorMoving) {
        m_targetCursorX += m_targetCursorSpeed * GetFrameTime();
        if (m_targetCursorX >= m_arenaRect.x + m_arenaRect.width) {
            m_targetCursorMoving = false;
            m_narratorText = "* You swung wide and missed!";
            m_state = BattleState::DialogMessage;
            context.audio.playSound("assets/sfx/cartoon_slip.ogg", 0.7f);
        } else if (context.input.isConfirmPressed()) {
            m_targetCursorMoving = false;
            float center = m_arenaRect.x + m_arenaRect.width / 2.0f;
            float dist = std::abs(m_targetCursorX - center);
            int damage = std::max(2, (int)(22.0f - dist * 0.18f));

            m_enemy->takeDamage(damage);
            context.audio.playSound("assets/sfx/u_activate.wav", 0.9f);

            if (m_enemy->isDead()) {
                m_narratorText = "* " + m_enemy->getName() + " was defeated! You won!";
                context.player.exp += 20;
                context.player.gold += 15;
            } else {
                m_narratorText = "* You dealt " + std::to_string(damage) + " gamer damage to " + m_enemy->getName() + "!";
            }
            m_state = BattleState::DialogMessage;
        }
    }
}

void BattleSystem::handleActInput(GameContext& context) {
    auto opts = m_enemy->getActOptions();
    if (context.input.isUpPressed()) {
        m_subOptionIndex = (m_subOptionIndex + (int)opts.size() - 1) % (int)opts.size();
        context.audio.playSound("assets/sfx/selection_sound.wav", 0.5f);
    } else if (context.input.isDownPressed()) {
        m_subOptionIndex = (m_subOptionIndex + 1) % (int)opts.size();
        context.audio.playSound("assets/sfx/selection_sound.wav", 0.5f);
    }

    if (context.input.isCancelPressed()) {
        m_state = BattleState::MenuSelect;
    } else if (context.input.isConfirmPressed()) {
        context.audio.playSound("assets/sfx/selection_sound.wav", 0.8f);
        std::string chosen = opts[m_subOptionIndex];
        if (chosen == "Check") {
            m_narratorText = "* " + m_enemy->getName() + " - ATK 5 DEF 2. An excitable wolf fursuiter.";
        } else if (chosen == "Cringe Check") {
            m_narratorText = "* You pointed out the zipper on the fursuit. Barky blushes furiously!";
            m_enemy->setSpareable(true);
        } else if (chosen == "Offer Mountain Dew") {
            m_narratorText = "* You shared an ice-cold Mountain Dew. Barky is pacified!";
            m_enemy->setSpareable(true);
        } else {
            m_narratorText = "* You performed " + chosen + ". It was very effective!";
            m_enemy->setSpareable(true);
        }
        m_state = BattleState::DialogMessage;
    }
}

void BattleSystem::handleItemInput(GameContext& context) {
    if (context.player.inventory.empty()) {
        m_narratorText = "* You don't have any items!";
        m_state = BattleState::DialogMessage;
        return;
    }

    if (context.input.isCancelPressed()) {
        m_state = BattleState::MenuSelect;
        return;
    }

    if (context.input.isConfirmPressed()) {
        std::string item = context.player.inventory.back();
        context.player.inventory.pop_back();

        if (item == "Mt. Dew") {
            context.player.hp = std::min(context.player.maxHp, context.player.hp + 10);
            m_narratorText = "* You drank Mountain Dew! Restored 10 HP!";
        } else {
            context.player.hp = std::min(context.player.maxHp, context.player.hp + 6);
            m_narratorText = "* You munched on Doritos! Restored 6 HP!";
        }
        context.audio.playSound("assets/sfx/u_activate.wav", 0.8f);
        m_state = BattleState::DialogMessage;
    }
}

void BattleSystem::handleMercyInput(GameContext& context) {
    if (context.input.isCancelPressed()) {
        m_state = BattleState::MenuSelect;
        return;
    }

    if (context.input.isConfirmPressed()) {
        if (m_enemy->isSpareable()) {
            m_narratorText = "* YOU WON! You spared " + m_enemy->getName() + "!";
            context.audio.playSound("assets/sfx/u_activate.wav", 1.0f);
            context.player.gold += 10;
            m_state = BattleState::Victory;
        } else {
            m_narratorText = "* But their name wasn't yellow yet! They keep attacking!";
            m_state = BattleState::DialogMessage;
        }
    }
}

void BattleSystem::startEnemyTurn() {
    m_state = BattleState::EnemyTurn;
    m_enemyTurnTimer = 0.0f;
    m_enemyTurnDuration = 4.5f;
    m_spawnTimer = 0.0f;
    m_bullets.clear();
    m_soulPosition = { m_arenaRect.x + m_arenaRect.width / 2.0f, m_arenaRect.y + m_arenaRect.height / 2.0f };
    m_enemySpeechBubble = m_enemy->getRandomDialogue();
}

void BattleSystem::updateEnemyTurn(float dt, GameContext& context) {
    m_enemyTurnTimer += dt;
    if (m_enemyTurnTimer >= m_enemyTurnDuration) {
        m_bullets.clear();
        m_narratorText = "* What will you do next?";
        m_state = BattleState::MenuSelect;
        return;
    }

    // Soul movement inside dodge box
    Vector2 moveDir = { 0, 0 };
    if (context.input.isLeft()) moveDir.x -= 1.0f;
    if (context.input.isRight()) moveDir.x += 1.0f;
    if (context.input.isUp()) moveDir.y -= 1.0f;
    if (context.input.isDown()) moveDir.y += 1.0f;

    m_soulPosition.x += moveDir.x * m_soulSpeed * dt;
    m_soulPosition.y += moveDir.y * m_soulSpeed * dt;

    // Clamp inside arena
    float soulRadius = 7.0f;
    m_soulPosition.x = std::clamp(m_soulPosition.x, m_arenaRect.x + soulRadius + 4, m_arenaRect.x + m_arenaRect.width - soulRadius - 4);
    m_soulPosition.y = std::clamp(m_soulPosition.y, m_arenaRect.y + soulRadius + 4, m_arenaRect.y + m_arenaRect.height - soulRadius - 4);

    // Spawn bullets
    m_spawnTimer += dt;
    if (m_spawnTimer >= 0.45f) {
        m_spawnTimer = 0.0f;
        spawnBulletWave();
    }

    // Update & check bullet collisions
    Rectangle soulHitbox = { m_soulPosition.x - 5, m_soulPosition.y - 5, 10, 10 };

    for (auto& b : m_bullets) {
        b.update(dt);
        if (b.active && m_invulnTimer <= 0.0f && CheckCollisionRecs(soulHitbox, b.getBounds())) {
            context.player.hp -= 3;
            context.player.furryMeter += 8.0f;
            m_invulnTimer = 0.8f;
            context.audio.playSound("assets/sfx/pipipipopopo_backwards.ogg", 0.6f);

            if (context.player.hp <= 0) {
                context.player.hp = 0;
                m_state = BattleState::Defeat;
                m_narratorText = "* GAME OVER! You lost all HP and became a furry!";
                context.audio.playSound("assets/sfx/areusure.ogg", 1.0f);
                return;
            }
        }
    }
}

void BattleSystem::spawnBulletWave() {
    float startX = m_arenaRect.x + (float)GetRandomValue(15, (int)m_arenaRect.width - 15);
    Bullet b;
    b.position = { startX, m_arenaRect.y - 10 };
    b.velocity = { (float)GetRandomValue(-20, 20), (float)GetRandomValue(90, 140) };
    b.type = (GetRandomValue(0, 1) == 0) ? BulletType::UwU : BulletType::SodaCan;
    b.text = "UWU";
    b.color = (b.type == BulletType::UwU) ? (Color){ 255, 100, 200, 255 } : COLOR_UNDERTALE_GREEN;
    m_bullets.push_back(b);
}

void BattleSystem::render(GameContext& context) {
    Font font8Bit = context.resources.getFont8Bit(22);
    Font fontSmall = context.resources.getFont8Bit(16);

    // 1. Draw Enemy Placeholder / Sprite at top center
    int enemyCenterX = VIRTUAL_WIDTH / 2;
    int enemyCenterY = 110;

    // Enemy character art (Wolf Fursuiter parody)
    DrawCircle(enemyCenterX, enemyCenterY - 20, 32, { 120, 120, 140, 255 }); // Head
    DrawTriangle( // Left Ear
        { (float)enemyCenterX - 30, (float)enemyCenterY - 45 },
        { (float)enemyCenterX - 10, (float)enemyCenterY - 50 },
        { (float)enemyCenterX - 20, (float)enemyCenterY - 75 },
        { 90, 90, 110, 255 }
    );
    DrawTriangle( // Right Ear
        { (float)enemyCenterX + 10, (float)enemyCenterY - 50 },
        { (float)enemyCenterX + 30, (float)enemyCenterY - 45 },
        { (float)enemyCenterX + 20, (float)enemyCenterY - 75 },
        { 90, 90, 110, 255 }
    );
    DrawCircle(enemyCenterX - 10, enemyCenterY - 24, 5, WHITE); // Left eye
    DrawCircle(enemyCenterX + 10, enemyCenterY - 24, 5, WHITE); // Right eye
    DrawCircle(enemyCenterX - 10, enemyCenterY - 24, 2, BLACK);
    DrawCircle(enemyCenterX + 10, enemyCenterY - 24, 2, BLACK);
    DrawRectangle(enemyCenterX - 35, enemyCenterY + 12, 70, 50, { 50, 50, 70, 255 }); // Torso / Jacket

    // Name & HP text in Undertale font
    Color enemyNameColor = m_enemy->isSpareable() ? COLOR_UNDERTALE_YELLOW : WHITE;
    std::string nameTag = m_enemy->getName() + "  HP: " + std::to_string(m_enemy->getHp()) + " / " + std::to_string(m_enemy->getMaxHp());
    context.resources.drawCenteredText(font8Bit, nameTag, 175, 20, enemyNameColor, 1.0f);

    // Enemy speech bubble during enemy turn
    if (m_state == BattleState::EnemyTurn && !m_enemySpeechBubble.empty()) {
        Rectangle bubbleRect = { (float)enemyCenterX + 50, (float)enemyCenterY - 60, 160, 50 };
        DrawRectangleRec(bubbleRect, WHITE);
        DrawRectangleLinesEx(bubbleRect, 2.0f, BLACK);

        // Speech bubble pointer tail
        DrawTriangle(
            { bubbleRect.x, bubbleRect.y + 20 },
            { bubbleRect.x, bubbleRect.y + 35 },
            { bubbleRect.x - 12, bubbleRect.y + 28 },
            WHITE
        );
        DrawLineEx({ bubbleRect.x, bubbleRect.y + 20 }, { bubbleRect.x - 12, bubbleRect.y + 28 }, 2.0f, BLACK);
        DrawLineEx({ bubbleRect.x, bubbleRect.y + 35 }, { bubbleRect.x - 12, bubbleRect.y + 28 }, 2.0f, BLACK);

        context.resources.drawText(fontSmall, m_enemySpeechBubble, { bubbleRect.x + 10, bubbleRect.y + 16 }, 16, BLACK, 1.0f);
    }

    // 2. Arena box
    DrawRectangleRec(m_arenaRect, BLACK);
    DrawRectangleLinesEx(m_arenaRect, 4.0f, WHITE);

    // 3. Inside arena: depends on state
    if (m_state == BattleState::EnemyTurn) {
        // Draw soul (Frog Green Heart)
        if (m_invulnTimer <= 0.0f || (int)(GetTime() * 15.0) % 2 == 0) {
            Color soulCol = COLOR_UNDERTALE_GREEN;
            DrawCircle((int)m_soulPosition.x - 4, (int)m_soulPosition.y - 2, 5, soulCol);
            DrawCircle((int)m_soulPosition.x + 4, (int)m_soulPosition.y - 2, 5, soulCol);
            DrawTriangle(
                { m_soulPosition.x - 8, m_soulPosition.y },
                { m_soulPosition.x + 8, m_soulPosition.y },
                { m_soulPosition.x, m_soulPosition.y + 8 },
                soulCol
            );
        }

        // Draw bullets
        for (const auto& b : m_bullets) {
            b.render();
        }
    } else if (m_state == BattleState::FightTarget) {
        // Draw fight target timing bar (Undertale style)
        DrawRectangle((int)m_arenaRect.x + 10, (int)m_arenaRect.y + 20, (int)m_arenaRect.width - 20, (int)m_arenaRect.height - 40, { 30, 30, 30, 255 });
        float center = m_arenaRect.x + m_arenaRect.width / 2.0f;
        DrawRectangle((int)center - 8, (int)m_arenaRect.y + 20, 16, (int)m_arenaRect.height - 40, COLOR_UNDERTALE_YELLOW);
        DrawRectangle((int)m_targetCursorX - 4, (int)m_arenaRect.y + 15, 8, (int)m_arenaRect.height - 30, WHITE);
    } else if (m_state == BattleState::ActSelect) {
        auto opts = m_enemy->getActOptions();
        for (size_t i = 0; i < opts.size(); ++i) {
            Color c = (m_subOptionIndex == (int)i) ? COLOR_UNDERTALE_YELLOW : WHITE;
            std::string line = (m_subOptionIndex == (int)i ? "* " : "  ") + opts[i];
            context.resources.drawText(font8Bit, line, { m_arenaRect.x + 20, m_arenaRect.y + 20 + (float)i * 28 }, 20, c, 1.0f);
        }
    } else if (m_state == BattleState::ItemSelect) {
        if (context.player.inventory.empty()) {
            context.resources.drawText(font8Bit, "* No items in inventory!", { m_arenaRect.x + 20, m_arenaRect.y + 30 }, 20, WHITE, 1.0f);
        } else {
            context.resources.drawText(font8Bit, ("* " + context.player.inventory.back()), { m_arenaRect.x + 20, m_arenaRect.y + 30 }, 20, COLOR_UNDERTALE_YELLOW, 1.0f);
        }
    } else {
        // Narrator message
        context.resources.drawText(font8Bit, m_narratorText, { m_arenaRect.x + 15, m_arenaRect.y + 25 }, 20, WHITE, 1.0f);
    }

    // 4. Player Stats HUD with Undertale font
    std::string hud = context.player.name + "   LV " + std::to_string(context.player.level) +
                      "   HP " + std::to_string(context.player.hp) + "/" + std::to_string(context.player.maxHp) +
                      "   FURRY: " + std::to_string((int)context.player.furryMeter) + "%";
    context.resources.drawText(font8Bit, hud, { 60.0f, 385.0f }, 20, WHITE, 1.0f);

    // HP Bar
    DrawRectangle(320, 386, 60, 16, RED);
    float hpPct = (float)context.player.hp / (float)context.player.maxHp;
    DrawRectangle(320, 386, (int)(60.0f * hpPct), 16, COLOR_UNDERTALE_YELLOW);

    // 5. Main Action Buttons: [ FIGHT ] [ ACT ] [ ITEM ] [ MERCY ]
    const char* btnLabels[4] = { "FIGHT", "ACT", "ITEM", "MERCY" };
    int btnWidth = 110;
    int btnHeight = 42;
    int startBtnX = 60;
    int btnY = 418;

    for (int i = 0; i < 4; ++i) {
        Rectangle bRec = { (float)(startBtnX + i * 140), (float)btnY, (float)btnWidth, (float)btnHeight };
        bool isSelected = (m_selectedButton == static_cast<MainButton>(i));

        Color bColor = isSelected ? COLOR_UNDERTALE_YELLOW : COLOR_UNDERTALE_ORANGE;
        DrawRectangleLinesEx(bRec, 3.0f, bColor);

        float textW = context.resources.measureTextWidth(font8Bit, btnLabels[i], 22, 1.0f);
        Vector2 textPos = { bRec.x + (bRec.width - textW) * 0.5f, bRec.y + 10 };
        context.resources.drawText(font8Bit, btnLabels[i], textPos, 22, bColor, 1.0f);

        // Draw green frog soul cursor next to selected button
        if (isSelected && m_state == BattleState::MenuSelect) {
            DrawCircle((int)bRec.x + 12, (int)bRec.y + 21, 5, COLOR_UNDERTALE_GREEN);
        }
    }
}

} // namespace nundertale
