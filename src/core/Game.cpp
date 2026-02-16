#include "core/Game.hpp"
#include "scenes/DisclaimerScene.hpp"
#include <algorithm>
#include <string>

namespace nundertale {

Game::Game() {
    initWindow();

    m_audioService = std::make_unique<AudioService>();
    m_resourceService = std::make_unique<ResourceService>();
    m_inputService = std::make_unique<InputService>();
    m_sceneManager = std::make_unique<SceneManager>();

    // Wire up context dependencies (Dependency Injection)
    m_context = std::make_unique<GameContext>(*m_audioService, *m_resourceService, *m_inputService, *m_sceneManager);

    // Create virtual render target for pixel-perfect 640x480 retro rendering
    m_virtualTarget = LoadRenderTexture(VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
    SetTextureFilter(m_virtualTarget.texture, TEXTURE_FILTER_POINT);

    // Initial scene: pushScene with context properly executes onEnter (BUG-01)
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

    while (!WindowShouldClose() && m_context->isRunning && m_sceneManager->hasScenes()) {
        float frameDt = GetFrameTime();
        if (frameDt > 0.2f) frameDt = 0.2f; // prevent spiral of death
        accumulator += frameDt;

        // F4 toggle fullscreen (CORE-005)
        if (IsKeyPressed(KEY_F4)) {
            ToggleFullscreen();
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
#endif

        // 1. Update services
        m_inputService->update();
        m_audioService->updateMusic();

        // 2. Fixed 30 Hz logic update loop (CORE-001)
        while (accumulator >= LOGIC_TIMESTEP) {
            m_context->state.playTime += LOGIC_TIMESTEP;
            m_sceneManager->update(LOGIC_TIMESTEP, *m_context);
            accumulator -= LOGIC_TIMESTEP;
        }

        // 3. Render into virtual 640x480 canvas
        BeginTextureMode(m_virtualTarget);
            ClearBackground(BLACK);
            m_sceneManager->render(*m_context);

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
                DrawText(TextFormat("Furry: %.1f%%", m_context->player.furryMeter), 20, 108, 14, WHITE);
                DrawText(TextFormat("Room: %s", m_context->state.currentRoom.c_str()), 20, 126, 14, WHITE);
            }
#endif
        EndTextureMode();

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
    float destX = (GetScreenWidth() - destWidth) * 0.5f;
    float destY = (GetScreenHeight() - destHeight) * 0.5f;

    Rectangle sourceRec = { 0.0f, 0.0f, (float)m_virtualTarget.texture.width, -(float)m_virtualTarget.texture.height };
    Rectangle destRec = { destX, destY, destWidth, destHeight };

    DrawTexturePro(m_virtualTarget.texture, sourceRec, destRec, { 0, 0 }, 0.0f, WHITE);
}

} // namespace nundertale
