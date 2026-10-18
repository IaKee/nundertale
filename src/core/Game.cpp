#include "core/Game.hpp"
#include "scenes/DisclaimerScene.hpp"
#include "scenes/OverworldScene.hpp"
#include "scenes/BattleScene.hpp"
#include "services/MusicTracks.hpp"
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>

namespace nundertale {

Game::Game() {
    initWindow();

    m_audioService = std::make_unique<AudioService>();
    m_resourceService = std::make_unique<ResourceService>();
    m_inputService = std::make_unique<InputService>();
    m_saveService = std::make_unique<SaveService>();
    m_sceneManager = std::make_unique<SceneManager>();

    // Wire up context dependencies (Dependency Injection)
    m_context = std::make_unique<GameContext>(*m_audioService, *m_resourceService, *m_inputService, *m_sceneManager, *m_saveService);

    std::ifstream itemFile("data/items/items.json");
    if (itemFile) {
        std::ostringstream itemJson;
        itemJson << itemFile.rdbuf();
        m_context->itemDatabase.parse(itemJson.str());
    }

    std::ifstream furFile("data/fur.json");
    if (furFile) {
        std::ostringstream furJson;
        furJson << furFile.rdbuf();
        m_context->furTable.parse(furJson.str());
    }

    globalMusicTable().loadFromFile("data/music.json");

    // Create virtual render target for pixel-perfect 640x480 retro rendering
    m_virtualTarget = LoadRenderTexture(VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
    SetTextureFilter(m_virtualTarget.texture, TEXTURE_FILTER_POINT);

    // Initial scene: pushScene with context properly executes onEnter (BUG-01)
#ifdef NUNDERTALE_DEBUG
    // QA-002: NUNDERTALE_ROOM=<roomId> boots a debug build straight into that room.
    const char* debugRoom = std::getenv("NUNDERTALE_ROOM");
    if (debugRoom != nullptr && debugRoom[0] != '\0') {
        m_context->state.currentRoom = debugRoom;
        m_sceneManager->pushScene(std::make_unique<OverworldScene>(), *m_context);
        // NUNDERTALE_BATTLE=<encounterId> also starts that battle on top of the room.
        const char* debugBattle = std::getenv("NUNDERTALE_BATTLE");
        if (debugBattle != nullptr && debugBattle[0] != 0) {
            m_sceneManager->pushScene(std::make_unique<BattleScene>(debugBattle), *m_context);
        }
        return;
    }
#endif
    m_sceneManager->pushScene(std::make_unique<DisclaimerScene>(), *m_context);
}

Game::~Game() {
    UnloadRenderTexture(m_virtualTarget);
    CloseWindow();
}

void Game::initWindow() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    SetWindowMinSize(640, 480);
    SetTargetFPS(TARGET_FPS); // 30 FPS Undertale standard (CORE-001)

    // Window icon
    if (FileExists("assets/img/icon.png")) {
        Image icon = LoadImage("assets/img/icon.png");
        SetWindowIcon(icon);
        UnloadImage(icon);
    }
}

void Game::run() {
    float accumulator = 0.0f;
    float escQuitTimer = 0.0f;

    // CORE-007: Apply user settings from %LOCALAPPDATA%/nUndertale/config.json
    UserConfig config = m_saveService->getConfig();
    m_audioService->setMusicVolume(config.musicVolume);
    m_audioService->setMasterVolume(config.masterVolume);
    if (config.fullscreen && !IsWindowFullscreen()) {
        ToggleFullscreen();
    }

    while (!WindowShouldClose() && m_context->isRunning && m_sceneManager->hasScenes()) {
        float frameDt = GetFrameTime();
        if (frameDt > 0.2f) frameDt = 0.2f; // prevent spiral of death
        accumulator += frameDt;

        // F4 toggle fullscreen (CORE-005)
        if (IsKeyPressed(KEY_F4)) {
            ToggleFullscreen();
            config.fullscreen = IsWindowFullscreen();
            m_saveService->saveConfig(config);
        }

        // Hold ESC for 1 second to quit (CORE-006, BUG-14)
        if (IsKeyDown(KEY_ESCAPE)) {
            escQuitTimer += frameDt;
            if (escQuitTimer >= 1.0f) {
                m_context->isRunning = false;
                break;
            }
        } else {
            escQuitTimer = 0.0f;
        }

#ifdef NUNDERTALE_DEBUG
        static bool s_showDebugOverlay = false;
        if (IsKeyPressed(KEY_F1)) {
            s_showDebugOverlay = !s_showDebugOverlay;
        }
        if (IsKeyPressed(KEY_F2)) {
            m_context->debugHitboxes = !m_context->debugHitboxes;
        }
#endif

        // 1. Update services
        m_inputService->update();
        m_audioService->updateMusic();

        // 2. Fixed 30 Hz logic update loop (CORE-001)
        while (accumulator >= LOGIC_TIMESTEP) {
            m_context->state.playTime += LOGIC_TIMESTEP;
            raidTick(m_context->state.raid, LOGIC_TIMESTEP,
                     !IsWindowFocused() || m_context->raidPaused);

            // GFX-006: Update screen shake
            if (m_context->shakeTimer > 0.0f) {
                m_context->shakeTimer -= LOGIC_TIMESTEP;
                int maxOffset = static_cast<int>(m_context->shakeIntensity);
                if (maxOffset > 0) {
                    m_context->shakeOffset = {
                        static_cast<float>(GetRandomValue(-maxOffset, maxOffset)),
                        static_cast<float>(GetRandomValue(-maxOffset, maxOffset))
                    };
                }
            } else {
                m_context->shakeOffset = { 0.0f, 0.0f };
            }

            // GFX-007: Update screen flash
            if (m_context->flashTimer > 0.0f) {
                m_context->flashTimer -= LOGIC_TIMESTEP;
            }

            m_sceneManager->update(LOGIC_TIMESTEP, *m_context);
            m_inputService->endTick();
            accumulator -= LOGIC_TIMESTEP;
        }

        // 3. Render into virtual 640x480 canvas
        BeginTextureMode(m_virtualTarget);
            ClearBackground(BLACK);
            m_sceneManager->render(*m_context);

            // GFX-007: Flash & tint overlay
            if (m_context->flashTimer > 0.0f && m_context->flashDuration > 0.0f) {
                float alpha = m_context->flashTimer / m_context->flashDuration;
                Color fc = m_context->flashColor;
                fc.a = static_cast<unsigned char>(std::clamp(alpha, 0.0f, 1.0f) * 255.0f);
                DrawRectangle(0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT, fc);
            }

            // Visual feedback for quitting on held ESC (CORE-006)
            if (escQuitTimer > 0.2f) {
                int dots = static_cast<int>(escQuitTimer * 4.0f) % 4;
                std::string quitText = "QUITTING" + std::string(dots, '.');
                DrawText(quitText.c_str(), 16, 16, 20, WHITE);
            }

#ifdef NUNDERTALE_DEBUG
            // Debug overlay on F1 (QA-001)
            if (s_showDebugOverlay) {
                DrawRectangle(10, 10, 240, 140, { 0, 0, 0, 210 });
                DrawRectangleLines(10, 10, 240, 140, COLOR_UNDERTALE_GREEN);
                DrawText("DEBUG OVERLAY (F1)", 20, 18, 14, COLOR_UNDERTALE_GREEN);
                DrawText(TextFormat("FPS: %d (Target: %d)", GetFPS(), TARGET_FPS), 20, 36, 14, WHITE);
                DrawText(TextFormat("Time: %.1fs", m_context->state.playTime), 20, 54, 14, WHITE);
                DrawText(TextFormat("Player: %s | LV %d", m_context->player.name.c_str(), m_context->player.level), 20, 72, 14, WHITE);
                DrawText(TextFormat("HP: %d/%d | G: %d", m_context->player.hp, m_context->player.maxHp, m_context->player.gold), 20, 90, 14, WHITE);
                DrawText(TextFormat("Furry: %d%%", m_context->state.furry.value), 20, 108, 14, WHITE);
                DrawText(TextFormat("Room: %s", m_context->state.currentRoom.c_str()), 20, 126, 14, WHITE);
            }
#endif
        EndTextureMode();

#ifdef NUNDERTALE_DEBUG
        // QA-002: NUNDERTALE_SHOT=<file.png> saves the virtual canvas after
        // NUNDERTALE_SHOT_FRAME frames (default 45, ~1.5 s) and quits.
        static int s_shotFrames = 0;
        const char* shotPath = std::getenv("NUNDERTALE_SHOT");
        const char* shotFrameText = std::getenv("NUNDERTALE_SHOT_FRAME");
        const int shotFrame = shotFrameText != nullptr ? std::max(1, std::atoi(shotFrameText)) : 45;
        if (shotPath != nullptr && shotPath[0] != '\0' && ++s_shotFrames == shotFrame) {
            Image shot = LoadImageFromTexture(m_virtualTarget.texture);
            ImageFlipVertical(&shot);
            ExportImage(shot, shotPath);
            UnloadImage(shot);
            m_context->isRunning = false;
        }
#endif

        // 4. Draw virtual canvas scaled & letterboxed into window
        BeginDrawing();
            ClearBackground(BLACK);
            renderLetterboxed();
        EndDrawing();
    }
}

void Game::renderLetterboxed() {
    float scale = std::min((float)GetScreenWidth() / VIRTUAL_WIDTH, (float)GetScreenHeight() / VIRTUAL_HEIGHT);

    float destWidth = VIRTUAL_WIDTH * scale;
    float destHeight = VIRTUAL_HEIGHT * scale;
    float destX = (static_cast<float>(GetScreenWidth()) - destWidth) * 0.5f + m_context->shakeOffset.x * scale;
    float destY = (static_cast<float>(GetScreenHeight()) - destHeight) * 0.5f + m_context->shakeOffset.y * scale;

    Rectangle sourceRec = { 0.0f, 0.0f, (float)m_virtualTarget.texture.width, -(float)m_virtualTarget.texture.height };
    Rectangle destRec = { destX, destY, destWidth, destHeight };

    DrawTexturePro(m_virtualTarget.texture, sourceRec, destRec, { 0, 0 }, 0.0f, WHITE);
}

} // namespace nundertale
