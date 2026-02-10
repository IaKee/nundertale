#include "core/Game.hpp"
#include "scenes/DisclaimerScene.hpp"
#include <algorithm>

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

    // Initial scene
    m_sceneManager->pushScene(std::make_unique<DisclaimerScene>());
    m_sceneManager->update(0.0f, *m_context);
}

Game::~Game() {
    UnloadRenderTexture(m_virtualTarget);
    CloseWindow();
}

void Game::initWindow() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    SetWindowMinSize(640, 480);
    SetTargetFPS(60);

    // Window icon
    if (FileExists("assets/img/icon.png")) {
        Image icon = LoadImage("assets/img/icon.png");
        SetWindowIcon(icon);
        UnloadImage(icon);
    }
}

void Game::run() {
    while (!WindowShouldClose() && m_context->isRunning && m_sceneManager->hasScenes()) {
        float dt = GetFrameTime();

        // 1. Update services
        m_inputService->update();
        m_audioService->updateMusic();

        // 2. Update scenes
        m_sceneManager->update(dt, *m_context);

        // 3. Render into virtual 640x480 canvas
        BeginTextureMode(m_virtualTarget);
            ClearBackground(BLACK);
            m_sceneManager->render(*m_context);
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
