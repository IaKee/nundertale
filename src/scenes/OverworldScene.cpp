#include "scenes/OverworldScene.hpp"
#include "scenes/BattleScene.hpp"
#include "scenes/TitleScene.hpp"
#include "scenes/EncounterTransitionScene.hpp"
#include "core/GameContext.hpp"
#include "core/Constants.hpp"
#include "core/SceneManager.hpp"
#include <raylib.h>

namespace nundertale {

OverworldScene::OverworldScene()
    : m_player(Vector2{ 300.0f, 320.0f }) {
    initObstacles();
    initInteractables();
}

void OverworldScene::initObstacles() {
    m_obstacles.clear();
    // Room borders based on 640x480 room
    m_obstacles.push_back({ 0, 0, 640, 60 });       // Top wall
    m_obstacles.push_back({ 0, 440, 640, 40 });     // Bottom wall
    m_obstacles.push_back({ 0, 0, 40, 480 });       // Left wall
    m_obstacles.push_back({ 600, 0, 40, 480 });     // Right wall

    // Room pillars / furniture
    m_obstacles.push_back({ 140, 160, 50, 80 });    // Left pillar
    m_obstacles.push_back({ 450, 160, 50, 80 });    // Right pillar
}

void OverworldScene::initInteractables() {
    m_interactables.clear();

    // 1. Save Point Star
    m_savePoint = std::make_unique<SavePoint>(Vector2{ 320.0f, 260.0f });

    // 2. Signpost on the wall
    Rectangle signBounds = { 100, 70, 40, 30 };
    auto sign = std::make_unique<GenericInteractable>(
        signBounds,
        [this](GameContext& context) {
            context.audio.playSound("assets/sfx/selection_sound.wav", 0.6f);
            m_dialogBox.startDialog({
                "NOTICE FROM THE RUINS ADMINISTRATION:\n* Please refrain from shedding faux-fur in the air ducts.",
                "* Trespassing gamers will be subjected to mandatory waifu appreciation seminars."
            }, "SIGN");
        },
        [](GameContext& context) {
            DrawRectangle(105, 80, 30, 20, { 140, 90, 40, 255 });
            DrawRectangleLines(105, 80, 30, 20, BLACK);
            DrawText("?", 115, 82, 16, WHITE);
        }
    );
    m_interactables.push_back(std::move(sign));

    // 3. NPC Barky the Alpha Furry standing near the door
    Rectangle npcBounds = { 490, 80, 40, 50 };
    auto npc = std::make_unique<GenericInteractable>(
        npcBounds,
        [this](GameContext& context) {
            context.audio.playSound("assets/sfx/pipipipopopo_backwards.ogg", 0.6f);
            m_dialogBox.startDialog({
                "* Awoo! What do we have here? A hairless gamer in my domain?!",
                "* I am Barky, the Alpha Wolf of Mt. Dew! Prepare yourself!",
                "* Step through the north doorway if you dare face my fursona!"
            }, "BARKY");
        },
        [](GameContext& context) {
            // NPC placeholder visual
            DrawCircle(510, 95, 16, { 120, 120, 140, 255 });
            DrawTriangle({ 498, 85 }, { 506, 85 }, { 502, 70 }, { 90, 90, 110, 255 });
            DrawTriangle({ 514, 85 }, { 522, 85 }, { 518, 70 }, { 90, 90, 110, 255 });
            DrawRectangle(496, 110, 28, 25, { 50, 50, 70, 255 });
        }
    );
    m_interactables.push_back(std::move(npc));

    // 4. North Doorway Trigger to Battle Scene
    Rectangle doorBounds = { 280, 50, 80, 30 };
    auto door = std::make_unique<GenericInteractable>(
        doorBounds,
        [this](GameContext& context) {
            context.audio.playSound("assets/sfx/pipipipopopo_backwards.ogg", 0.8f);
            m_dialogBox.startDialog({
                "* An intense aura of cringe and determination blocks your path!",
                "* Barky the Alpha Furry attacks!"
            }, "", [this, &context]() {
                Vector2 pPos = m_player.getPosition();
                context.scenes.pushScene(std::make_unique<EncounterTransitionScene>(pPos, std::make_unique<BattleScene>()), context);
            });
        },
        [](GameContext& context) {
            // Glowing doorway
            DrawRectangleLines(280, 50, 80, 30, COLOR_UNDERTALE_YELLOW);
            DrawText("EXIT", 300, 56, 16, COLOR_UNDERTALE_YELLOW);
        }
    );
    m_interactables.push_back(std::move(door));

    // 5. Snack Bowl (ITM-004: Monster Candy parody with 8-slot limit check)
    Rectangle bowlBounds = { 200, 160, 40, 30 };
    auto snackBowl = std::make_unique<GenericInteractable>(
        bowlBounds,
        [this](GameContext& context) {
            context.audio.playSound("assets/sfx/selection_sound.wav", 0.6f);
            if (!context.player.canAddItem()) {
                m_dialogBox.startDialog({
                    "* (You tried to take a snack...)\n* (You're carrying too much.)"
                }, "");
            } else {
                context.player.addItem("Mt. Dew");
                context.audio.playSound("assets/sfx/heal.ogg", 0.7f);
                m_dialogBox.startDialog({
                    "* (You took a can of warm Mountain Dew.)\n* (Your inventory feels slightly heavier.)"
                }, "");
            }
        },
        [](GameContext& context) {
            DrawRectangle(205, 165, 30, 20, { 50, 120, 50, 255 });
            DrawRectangleLines(205, 165, 30, 20, WHITE);
            DrawText("DEW", 208, 168, 12, COLOR_UNDERTALE_YELLOW);
        }
    );
    m_interactables.push_back(std::move(snackBowl));
}

void OverworldScene::onEnter(GameContext& context) {
    context.audio.playMusic("assets/music/epic_background_loop.ogg", true);
    if (context.state.playerPos.x != 0.0f && context.state.playerPos.y != 0.0f) {
        m_player.setPosition(context.state.playerPos);
    }
}

void OverworldScene::onResume(GameContext& context) {
    context.audio.playMusic("assets/music/epic_background_loop.ogg", true);
}

void OverworldScene::drawSoul(float x, float y) const {
    // 16x16 pixel-perfect Undertale red soul (BUI-007)
    DrawRectangle((int)x + 2, (int)y, 4, 3, SOUL_RED);
    DrawRectangle((int)x + 8, (int)y, 4, 3, SOUL_RED);
    DrawRectangle((int)x + 1, (int)y + 3, 12, 4, SOUL_RED);
    DrawRectangle((int)x + 2, (int)y + 7, 10, 3, SOUL_RED);
    DrawRectangle((int)x + 4, (int)y + 10, 6, 2, SOUL_RED);
    DrawRectangle((int)x + 6, (int)y + 12, 2, 2, SOUL_RED);
}

void OverworldScene::handleMenuInput(GameContext& context) {
    if (m_menuState == OverworldMenuState::Main) {
        if (context.input.isUpPressed()) {
            m_menuIndex = (m_menuIndex - 1 + 3) % 3;
            context.audio.playSound("assets/sfx/selection_sound.wav", 0.4f);
        } else if (context.input.isDownPressed()) {
            m_menuIndex = (m_menuIndex + 1) % 3;
            context.audio.playSound("assets/sfx/selection_sound.wav", 0.4f);
        }

        if (context.input.isCancelPressed() || IsKeyPressed(KEY_C) || IsKeyPressed(KEY_LEFT_CONTROL)) {
            m_menuState = OverworldMenuState::Closed;
            context.audio.playSound("assets/sfx/selection_sound.wav", 0.3f);
            return;
        }

        if (context.input.isConfirmPressed()) {
            context.audio.playSound("assets/sfx/selection_sound.wav", 0.7f);
            if (m_menuIndex == 0) {
                m_menuState = OverworldMenuState::Item;
                m_itemSubIndex = 0;
            } else if (m_menuIndex == 1) {
                m_menuState = OverworldMenuState::Stat;
            } else if (m_menuIndex == 2) {
                m_menuState = OverworldMenuState::Cell;
            }
        }
    } else if (m_menuState == OverworldMenuState::Stat) {
        if (context.input.isCancelPressed() || context.input.isConfirmPressed() || IsKeyPressed(KEY_C)) {
            m_menuState = OverworldMenuState::Main;
            context.audio.playSound("assets/sfx/selection_sound.wav", 0.4f);
        }
    } else if (m_menuState == OverworldMenuState::Item) {
        int invSize = (int)context.player.inventory.size();
        if (invSize > 0) {
            if (context.input.isUpPressed()) {
                m_itemSubIndex = (m_itemSubIndex - 1 + invSize) % invSize;
                context.audio.playSound("assets/sfx/selection_sound.wav", 0.4f);
            } else if (context.input.isDownPressed()) {
                m_itemSubIndex = (m_itemSubIndex + 1) % invSize;
                context.audio.playSound("assets/sfx/selection_sound.wav", 0.4f);
            }
            if (context.input.isConfirmPressed()) {
                std::string item = context.player.inventory[m_itemSubIndex];
                context.player.inventory.erase(context.player.inventory.begin() + m_itemSubIndex);

                if (item.find("Mouse") != std::string::npos || item.find("Keyboard") != std::string::npos || item.find("Weapon") != std::string::npos) {
                    // PRG-002: Equip weapon
                    std::string oldWeapon = context.player.weapon;
                    context.player.weapon = item;
                    context.player.weaponAt = (item.find("Keyboard") != std::string::npos) ? 4 : 2;
                    context.player.inventory.push_back(oldWeapon);
                    context.audio.playSound("assets/sfx/heal.ogg", 0.8f);
                    m_menuState = OverworldMenuState::Closed;
                    m_dialogBox.startDialog({ "* You equipped the " + item + ".\n* Attack power updated!" }, "");
                    return;
                } else if (item.find("Headset") != std::string::npos || item.find("Hoodie") != std::string::npos || item.find("Armor") != std::string::npos) {
                    // PRG-002: Equip armor
                    std::string oldArmor = context.player.armor;
                    context.player.armor = item;
                    context.player.armorDf = (item.find("Hoodie") != std::string::npos) ? 3 : 1;
                    context.player.inventory.push_back(oldArmor);
                    context.audio.playSound("assets/sfx/heal.ogg", 0.8f);
                    m_menuState = OverworldMenuState::Closed;
                    m_dialogBox.startDialog({ "* You equipped the " + item + ".\n* Defense updated!" }, "");
                    return;
                } else if (item.find("Dew") != std::string::npos) {
                    context.player.hp = std::min(context.player.maxHp, context.player.hp + 10);
                    context.audio.playSound("assets/sfx/heal.ogg", 0.8f);
                    m_menuState = OverworldMenuState::Closed;
                    m_dialogBox.startDialog({ "* You drank the warm Mountain Dew.\n* You recovered 10 HP!" }, "");
                    return;
                } else if (item.find("Doritos") != std::string::npos) {
                    context.player.hp = std::min(context.player.maxHp, context.player.hp + 6);
                    context.audio.playSound("assets/sfx/heal.ogg", 0.8f);
                    m_menuState = OverworldMenuState::Closed;
                    m_dialogBox.startDialog({ "* You ate the stale Doritos.\n* You recovered 6 HP!" }, "");
                    return;
                } else {
                    context.player.hp = std::min(context.player.maxHp, context.player.hp + 8);
                    context.audio.playSound("assets/sfx/heal.ogg", 0.8f);
                    m_menuState = OverworldMenuState::Closed;
                    m_dialogBox.startDialog({ "* You used " + item + ".\n* Recovered 8 HP." }, "");
                    return;
                }
            }
        }
        if (context.input.isCancelPressed() || IsKeyPressed(KEY_C)) {
            m_menuState = OverworldMenuState::Main;
            context.audio.playSound("assets/sfx/selection_sound.wav", 0.4f);
        }
    } else if (m_menuState == OverworldMenuState::Cell) {
        if (context.input.isCancelPressed() || context.input.isConfirmPressed() || IsKeyPressed(KEY_C)) {
            m_menuState = OverworldMenuState::Main;
            context.audio.playSound("assets/sfx/selection_sound.wav", 0.4f);
        }
    }
}

void OverworldScene::renderMenu(GameContext& context, Font font) {
    if (m_menuState == OverworldMenuState::Closed) return;

    // Box 1: Player Summary Card (Top-Left)
    Rectangle summaryBox = { 32, 52, 140, 110 };
    DrawRectangleRec(summaryBox, BLACK);
    DrawRectangleLinesEx(summaryBox, 5.0f, WHITE);

    context.resources.drawText(font, context.player.name, { 45, 62 }, 20, WHITE, 1.0f);
    context.resources.drawText(font, "LV  " + std::to_string(context.player.level), { 45, 88 }, 18, WHITE, 1.0f);
    context.resources.drawText(font, "HP  " + std::to_string(context.player.hp) + "/" + std::to_string(context.player.maxHp), { 45, 112 }, 18, WHITE, 1.0f);
    context.resources.drawText(font, "G   " + std::to_string(context.player.gold), { 45, 136 }, 18, WHITE, 1.0f);

    // Box 2: Menu Options Card
    Rectangle optionsBox = { 32, 175, 140, 120 };
    DrawRectangleRec(optionsBox, BLACK);
    DrawRectangleLinesEx(optionsBox, 5.0f, WHITE);

    const char* menuLabels[3] = { "ITEM", "STAT", "CELL" };
    for (int i = 0; i < 3; ++i) {
        float optY = 195.0f + (float)i * 28.0f;
        if (m_menuState == OverworldMenuState::Main && m_menuIndex == i) {
            drawSoul(50.0f, optY + 9.0f);
        }
        context.resources.drawText(font, menuLabels[i], { 68.0f, optY }, 20, WHITE, 1.0f);
    }

    // Submenu Panel (Right side)
    if (m_menuState == OverworldMenuState::Stat) {
        Rectangle statBox = { 186, 52, 400, 340 };
        DrawRectangleRec(statBox, BLACK);
        DrawRectangleLinesEx(statBox, 5.0f, WHITE);

        context.resources.drawText(font, "\"" + context.player.name + "\"", { 210, 70 }, 22, WHITE, 1.0f);
        context.resources.drawText(font, "LV  " + std::to_string(context.player.level), { 210, 102 }, 20, WHITE, 1.0f);
        context.resources.drawText(font, "HP  " + std::to_string(context.player.hp) + " / " + std::to_string(context.player.maxHp), { 210, 126 }, 20, WHITE, 1.0f);

        std::string atStr = "AT  " + std::to_string(context.player.at) + " (" + std::to_string(context.player.weaponAt) + ")";
        std::string dfStr = "DF  " + std::to_string(context.player.df) + " (" + std::to_string(context.player.armorDf) + ")";
        context.resources.drawText(font, atStr, { 210, 160 }, 18, WHITE, 1.0f);
        context.resources.drawText(font, dfStr, { 210, 184 }, 18, WHITE, 1.0f);

        context.resources.drawText(font, "EXP: " + std::to_string(context.player.exp), { 370, 160 }, 18, WHITE, 1.0f);
        context.resources.drawText(font, "NEXT: 10", { 370, 184 }, 18, WHITE, 1.0f);

        context.resources.drawText(font, "WEAPON: " + context.player.weapon, { 210, 220 }, 18, WHITE, 1.0f);
        context.resources.drawText(font, "ARMOR:  " + context.player.armor, { 210, 246 }, 18, WHITE, 1.0f);
        context.resources.drawText(font, "GOLD:   " + std::to_string(context.player.gold), { 210, 272 }, 18, WHITE, 1.0f);

        // Furry meter status line (MNU-003 / FUR-006)
        Color furColor = (context.player.furryMeter > 0.0f) ? KR_PURPLE : WHITE;
        context.resources.drawText(font, "FURRY:  " + std::to_string((int)context.player.furryMeter) + "%", { 210, 305 }, 18, furColor, 1.0f);
    } else if (m_menuState == OverworldMenuState::Item) {
        Rectangle itemBox = { 186, 52, 400, 260 };
        DrawRectangleRec(itemBox, BLACK);
        DrawRectangleLinesEx(itemBox, 5.0f, WHITE);

        if (context.player.inventory.empty()) {
            context.resources.drawText(font, "* (Inventory is empty)", { 210, 75 }, 20, WHITE, 1.0f);
        } else {
            for (size_t i = 0; i < context.player.inventory.size(); ++i) {
                float optY = 75.0f + (float)i * 28.0f;
                if (m_itemSubIndex == (int)i) {
                    drawSoul(210.0f, optY + 9.0f);
                }
                context.resources.drawText(font, "* " + context.player.inventory[i], { 228.0f, optY }, 20, WHITE, 1.0f);
            }
        }
    } else if (m_menuState == OverworldMenuState::Cell) {
        Rectangle cellBox = { 186, 52, 400, 180 };
        DrawRectangleRec(cellBox, BLACK);
        DrawRectangleLinesEx(cellBox, 5.0f, WHITE);

        context.resources.drawText(font, "* Furiell (No signal in the Ruins)", { 210, 80 }, 20, GRAY, 1.0f);
        context.resources.drawText(font, "* Discord (Wi-Fi disconnected)", { 210, 115 }, 20, GRAY, 1.0f);
    }
}

void OverworldScene::handleSaveBoxInput(GameContext& context) {
    if (m_saveBoxState == SaveBoxState::Active) {
        if (context.input.isLeftPressed() || context.input.isRightPressed()) {
            m_saveBoxChoice = (m_saveBoxChoice == 0) ? 1 : 0;
            context.audio.playSound("assets/sfx/selection_sound.wav", 0.5f);
        }

        if (context.input.isConfirmPressed()) {
            if (m_saveBoxChoice == 0) { // Save
                context.state.playerPos = m_player.getPosition();
                context.save.saveGame(context.state);
                context.audio.playSound("assets/sfx/u_activate.wav", 0.9f);
                m_saveBoxState = SaveBoxState::Saved;
            } else { // Return
                m_saveBoxState = SaveBoxState::Closed;
                context.audio.playSound("assets/sfx/selection_sound.wav", 0.4f);
            }
        } else if (context.input.isCancelPressed()) {
            m_saveBoxState = SaveBoxState::Closed;
            context.audio.playSound("assets/sfx/selection_sound.wav", 0.4f);
        }
    } else if (m_saveBoxState == SaveBoxState::Saved) {
        if (context.input.isConfirmPressed() || context.input.isCancelPressed()) {
            m_saveBoxState = SaveBoxState::Closed;
        }
    }
}

void OverworldScene::renderSaveBox(GameContext& context, Font font) {
    if (m_saveBoxState == SaveBoxState::Closed) return;

    Rectangle box = { 130.0f, 155.0f, 380.0f, 170.0f };
    DrawRectangleRec(box, BLACK);

    Color borderColor = (m_saveBoxState == SaveBoxState::Saved) ? UI_YELLOW : WHITE;
    DrawRectangleLinesEx(box, 5.0f, borderColor);

    // Format playtime MM:SS
    int totalSec = (int)context.state.playTime;
    int mins = totalSec / 60;
    int secs = totalSec % 60;
    char timeBuf[16];
    std::snprintf(timeBuf, sizeof(timeBuf), "%d:%02d", mins, secs);

    // Line 1: Name   LV X   MM:SS
    std::string line1 = context.player.name;
    context.resources.drawText(font, line1, { 160.0f, 175.0f }, 22, borderColor, 1.0f);

    std::string lvStr = "LV " + std::to_string(context.player.level);
    context.resources.drawText(font, lvStr, { 290.0f, 175.0f }, 22, borderColor, 1.0f);

    context.resources.drawText(font, timeBuf, { 420.0f, 175.0f }, 22, borderColor, 1.0f);

    // Line 2: Room Display Name
    std::string roomName = context.save.getRoomDisplayName(context.state.currentRoom);
    context.resources.drawText(font, roomName, { 160.0f, 215.0f }, 22, borderColor, 1.0f);

    // Line 3: Save / Return or "File saved."
    if (m_saveBoxState == SaveBoxState::Active) {
        if (m_saveBoxChoice == 0) {
            drawSoul(180.0f, 268.0f);
        } else {
            drawSoul(330.0f, 268.0f);
        }
        context.resources.drawText(font, "Save", { 205.0f, 260.0f }, 22, WHITE, 1.0f);
        context.resources.drawText(font, "Return", { 355.0f, 260.0f }, 22, WHITE, 1.0f);
    } else if (m_saveBoxState == SaveBoxState::Saved) {
        context.resources.drawText(font, "File saved.", { 160.0f, 260.0f }, 22, UI_YELLOW, 1.0f);
    }
}

void OverworldScene::update(float dt, GameContext& context) {
    // If dialog is active, dialog gets all input
    if (m_dialogBox.isActive()) {
        m_dialogBox.update(dt, context);
        return;
    }

    // Save Point Box update (SAV-002)
    if (m_saveBoxState != SaveBoxState::Closed) {
        handleSaveBoxInput(context);
        return;
    }

    // Overworld C Menu update (MNU-001)
    if (m_menuState != OverworldMenuState::Closed) {
        handleMenuInput(context);
        return;
    }

    if (IsKeyPressed(KEY_C) || IsKeyPressed(KEY_LEFT_CONTROL)) {
        m_menuState = OverworldMenuState::Main;
        m_menuIndex = 0;
        context.audio.playSound("assets/sfx/selection_sound.wav", 0.5f);
        return;
    }

    if (m_savePoint) {
        m_savePoint->update(dt);
    }

    // Player movement
    Vector2 inputDir = { 0, 0 };
    if (context.input.isLeft()) inputDir.x -= 1.0f;
    if (context.input.isRight()) inputDir.x += 1.0f;
    if (context.input.isUp()) inputDir.y -= 1.0f;
    if (context.input.isDown()) inputDir.y += 1.0f;

    m_player.update(dt, inputDir, m_obstacles);

    // Keep GameState synchronized (CORE-004)
    context.state.playerPos = m_player.getPosition();
    context.state.playTime += dt;

    // Interaction check
    if (context.input.isConfirmPressed()) {
        Rectangle interactBox = m_player.getInteractionBox();

        // Check SavePoint (SAV-002)
        if (m_savePoint && CheckCollisionRecs(interactBox, m_savePoint->getBounds())) {
            m_savePoint->interact(context);
            m_dialogBox.startDialog({
                "* The scent of lukewarm energy drinks and stale Doritos fills you with DETERMINATION.",
                "* HP fully restored."
            }, "", [this]() {
                m_saveBoxState = SaveBoxState::Active;
                m_saveBoxChoice = 0;
            });
            return;
        }

        // Check other interactables
        for (auto& item : m_interactables) {
            if (CheckCollisionRecs(interactBox, item->getBounds())) {
                item->interact(context);
                return;
            }
        }
    }

#ifdef NUNDERTALE_DEBUG
    // Quick Battle Trigger shortcut for testing (ENC-005, ENC-002)
    if (IsKeyPressed(KEY_B)) {
        Vector2 pPos = m_player.getPosition();
        context.scenes.pushScene(std::make_unique<EncounterTransitionScene>(pPos, std::make_unique<BattleScene>()), context);
    }
#endif
}

void OverworldScene::render(GameContext& context) {
    ClearBackground(BLACK);

    Font font8Bit = context.resources.getFont8Bit(20);

    // 1. Draw Background Room Texture (assets/img/test.bmp)
    Texture2D bgTex = context.resources.getTexture("assets/img/test.bmp");
    DrawTexture(bgTex, 0, 0, WHITE);

    // 2. Draw Pillars / Room Decor
    DrawRectangle(140, 160, 50, 80, { 70, 50, 90, 255 });
    DrawRectangleLines(140, 160, 50, 80, BLACK);
    DrawRectangle(450, 160, 50, 80, { 70, 50, 90, 255 });
    DrawRectangleLines(450, 160, 50, 80, BLACK);

    // 3. Render Interactables & SavePoint
    for (auto& item : m_interactables) {
        item->render(context);
    }
    if (m_savePoint) {
        m_savePoint->render(context);
    }

    // 4. Render Player (Pepe the frog using masked texture)
    Texture2D pepeTex = context.resources.getMaskedTexture("assets/img/pepe_char.bmp", { 255, 0, 255, 255 });
    m_player.render(pepeTex);

    // OVW-001: Bottom HUD bar removed! Exploration is clean 1:1 like Undertale.
    // Overworld C Menu (MNU-001)
    renderMenu(context, font8Bit);

    // Save Point Box (SAV-002)
    renderSaveBox(context, font8Bit);

    // 5. Render DialogBox if open
    m_dialogBox.render(context);

    // 6. Hitbox Debug Overlay (QA-002)
    if (context.debugHitboxes) {
        for (const auto& obs : m_obstacles) {
            DrawRectangleLinesEx(obs, 1.0f, RED);
        }
        for (const auto& item : m_interactables) {
            DrawRectangleLinesEx(item->getBounds(), 1.0f, COLOR_UNDERTALE_YELLOW);
        }
        if (m_savePoint) {
            DrawRectangleLinesEx(m_savePoint->getBounds(), 1.0f, UI_YELLOW);
        }
        DrawRectangleLinesEx(m_player.getCollisionBox(), 1.0f, BLUE);
        DrawRectangleLinesEx(m_player.getInteractionBox(), 1.0f, COLOR_UNDERTALE_GREEN);
    }
}

void OverworldScene::onExit(GameContext& context) {
}

} // namespace nundertale

