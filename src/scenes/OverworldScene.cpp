#include "scenes/OverworldScene.hpp"
#include "scenes/BattleScene.hpp"
#include "scenes/TitleScene.hpp"
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
                "Awoo! What do we have here? A hairless gamer in my domain?!",
                "I am Barky, the Alpha Wolf of Mt. Dew! Prepare yourself!",
                "Step through the north doorway if you dare face my fursona!"
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
                context.scenes.pushScene(std::make_unique<BattleScene>(), context);
            });
        },
        [](GameContext& context) {
            // Glowing doorway
            DrawRectangleLines(280, 50, 80, 30, COLOR_UNDERTALE_YELLOW);
            DrawText("EXIT", 300, 56, 16, COLOR_UNDERTALE_YELLOW);
        }
    );
    m_interactables.push_back(std::move(door));
}

void OverworldScene::onEnter(GameContext& context) {
    context.audio.playMusic("assets/music/epic_background_loop.ogg", true);
}

void OverworldScene::onResume(GameContext& context) {
    context.audio.playMusic("assets/music/epic_background_loop.ogg", true);
}

void OverworldScene::update(float dt, GameContext& context) {
    // If dialog is active, dialog gets all input
    if (m_dialogBox.isActive()) {
        m_dialogBox.update(dt, context);
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

    // Interaction check
    if (context.input.isConfirmPressed()) {
        Rectangle interactBox = m_player.getInteractionBox();

        // Check SavePoint
        if (m_savePoint && CheckCollisionRecs(interactBox, m_savePoint->getBounds())) {
            m_savePoint->interact(context);
            m_dialogBox.startDialog({
                "* The scent of lukewarm energy drinks and stale Doritos fills you with DETERMINATION.",
                "* HP fully restored! Progress saved."
            }, "");
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
    // Quick Battle Trigger shortcut for testing (ENC-005)
    if (IsKeyPressed(KEY_B)) {
        context.scenes.pushScene(std::make_unique<BattleScene>(), context);
    }
#endif
}

void OverworldScene::render(GameContext& context) {
    ClearBackground(BLACK);

    Font font8Bit = context.resources.getFont8Bit(18);

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

    // 5. Render HUD Overlay with authentic Undertale font
    DrawRectangle(0, 450, 640, 30, { 10, 10, 10, 220 });
    std::string hud = context.player.name + "   LV " + std::to_string(context.player.level) +
                      "   HP " + std::to_string(context.player.hp) + "/" + std::to_string(context.player.maxHp) +
                      "   G " + std::to_string(context.player.gold) +
                      "   FURRY: " + std::to_string((int)context.player.furryMeter) + "%";
    context.resources.drawText(font8Bit, hud, { 15.0f, 455.0f }, 18, WHITE, 1.0f);
    context.resources.drawText(font8Bit, "[Z: Interact | B: Battle | ESC: Menu]", { 370.0f, 455.0f }, 16, COLOR_UNDERTALE_YELLOW, 1.0f);

    // 6. Render DialogBox if open
    m_dialogBox.render(context);
}

void OverworldScene::onExit(GameContext& context) {
}

} // namespace nundertale
