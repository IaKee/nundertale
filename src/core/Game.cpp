#include "core/Game.hpp"
#include "scenes/DisclaimerScene.hpp"
#include "scenes/OverworldScene.hpp"
#include "scenes/BattleScene.hpp"
#include "services/AudioService.hpp"
#include "services/InputService.hpp"
#include "services/MusicTracks.hpp"
#ifdef NUNDERTALE_DEBUG
#include "services/RecordingAudioService.hpp"
#include "services/ScriptedInputService.hpp"
#endif
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <filesystem>

namespace nundertale {

Game::Game(int argc, char** argv) {
#ifdef NUNDERTALE_DEBUG
    for (int index = 1; index < argc; ++index) {
        const std::string argument = argv[index];
        if (argument == "--room" && index + 1 < argc) {
            m_debugRoom = argv[++index];
            m_debugLaunch = true;
        } else if (argument == "--battle" && index + 1 < argc) {
            m_debugBattle = argv[++index];
            m_debugLaunch = true;
        } else if (argument == "--spawn" && index + 1 < argc) {
            m_debugSpawn = argv[++index];
        } else if (argument == "--flag" && index + 1 < argc) {
            const std::string assignment = argv[++index];
            const std::size_t equals = assignment.find('=');
            if (equals != std::string::npos && equals > 0) {
                try {
                    m_debugFlags.emplace_back(
                        assignment.substr(0, equals), std::stoi(assignment.substr(equals + 1)));
                } catch (...) {
                }
            }
        } else if (argument == "--shot" && index + 1 < argc) {
            m_debugShotPath = argv[++index];
        } else if (argument == "--frames" && index + 1 < argc) {
            try {
                m_debugShotFrames = std::max(0, std::stoi(argv[++index]));
            } catch (...) {
                m_debugShotFrames = 20;
            }
        } else if (argument == "--record" && index + 1 < argc) {
            m_debugRecordDirectory = argv[++index];
        } else if (argument == "--seconds" && index + 1 < argc) {
            try {
                m_debugRecordSeconds = std::max(0.0, std::stod(argv[++index]));
            } catch (...) {
                m_debugRecordSeconds = 30.0;
            }
        } else if (argument == "--input" && index + 1 < argc) {
            m_debugInputPath = argv[++index];
        }
    }
    m_debugRecordTickLimit = static_cast<int>(std::llround(
        m_debugRecordSeconds * static_cast<double>(TARGET_FPS)));
#else
    (void)argc;
    (void)argv;
#endif
    initWindow();

#ifdef NUNDERTALE_DEBUG
    if (!m_debugRecordDirectory.empty()) {
        std::error_code directoryError;
        std::filesystem::create_directories(m_debugRecordDirectory, directoryError);
        if (directoryError) {
            throw std::runtime_error("Cannot create recording directory: " +
                                     m_debugRecordDirectory);
        }
        SetRandomSeed(0U);
        m_audioService = std::make_unique<RecordingAudioService>(m_debugRecordDirectory);
    } else {
        m_audioService = std::make_unique<AudioService>();
    }
#else
    m_audioService = std::make_unique<AudioService>();
#endif
    m_resourceService = std::make_unique<ResourceService>();
#ifdef NUNDERTALE_DEBUG
    if (!m_debugInputPath.empty()) {
        m_inputService = std::make_unique<ScriptedInputService>(m_debugInputPath);
    } else {
        m_inputService = std::make_unique<InputService>();
    }
#else
    m_inputService = std::make_unique<InputService>();
#endif
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
    if (m_debugLaunch) {
        if (!m_debugRoom.empty()) m_context->state.currentRoom = m_debugRoom;
        m_context->state.playerPos = { 0.0f, 0.0f };
        m_context->launchSpawn = m_debugSpawn;
        m_context->state.setFlag("raid_msgs_shown", 0xF);
        for (const auto& [key, value] : m_debugFlags) m_context->state.setFlag(key, value);
        if (!m_debugRoom.empty() || m_debugBattle.empty()) {
            m_sceneManager->pushScene(std::make_unique<OverworldScene>(), *m_context);
        }
        if (!m_debugBattle.empty()) {
            m_sceneManager->pushScene(std::make_unique<BattleScene>(m_debugBattle), *m_context);
        }
    } else {
        m_sceneManager->pushScene(std::make_unique<DisclaimerScene>(), *m_context);
    }
#else
    m_sceneManager->pushScene(std::make_unique<DisclaimerScene>(), *m_context);
#endif
}

Game::~Game() {
    UnloadRenderTexture(m_virtualTarget);
    CloseWindow();
}

void Game::initWindow() {
    unsigned int windowFlags = FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT;
#ifdef NUNDERTALE_DEBUG
    if (!m_debugRecordDirectory.empty()) windowFlags = FLAG_WINDOW_RESIZABLE;
#endif
    SetConfigFlags(windowFlags);
#ifdef NUNDERTALE_DEBUG
    const bool useVirtualWindow = !m_debugShotPath.empty() || !m_debugRecordDirectory.empty();
    const int initialWidth = useVirtualWindow ? VIRTUAL_WIDTH : WINDOW_WIDTH;
    const int initialHeight = useVirtualWindow ? VIRTUAL_HEIGHT : WINDOW_HEIGHT;
    InitWindow(initialWidth, initialHeight, WINDOW_TITLE);
#else
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
#endif
    SetWindowMinSize(640, 480);
#ifdef NUNDERTALE_DEBUG
    SetTargetFPS(m_debugRecordDirectory.empty() ? TARGET_FPS : 0);
#else
    SetTargetFPS(TARGET_FPS); // 30 FPS Undertale standard (CORE-001)
#endif

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

#ifdef NUNDERTALE_DEBUG
    if (!m_debugRecordDirectory.empty() && m_debugRecordTickLimit <= 0) return;
    const bool recording = !m_debugRecordDirectory.empty();
    const bool ignorePhysicalInput = recording || !m_debugInputPath.empty();
#endif

    // CORE-007: Apply user settings from %LOCALAPPDATA%/nUndertale/config.json
    UserConfig config = m_saveService->getConfig();
    m_audioService->setMusicVolume(config.musicVolume);
    m_audioService->setMasterVolume(config.masterVolume);
    bool allowConfiguredFullscreen = true;
#ifdef NUNDERTALE_DEBUG
    allowConfiguredFullscreen = m_debugShotPath.empty() && m_debugRecordDirectory.empty();
#endif
    if (allowConfiguredFullscreen && config.fullscreen && !IsWindowFullscreen()) {
        ToggleFullscreen();
    }

    while (!WindowShouldClose() && m_context->isRunning && m_sceneManager->hasScenes()) {
        float frameDt = GetFrameTime();
        if (frameDt > 0.2f) frameDt = 0.2f; // prevent spiral of death
#ifdef NUNDERTALE_DEBUG
        if (recording) {
            frameDt = LOGIC_TIMESTEP;
            accumulator = LOGIC_TIMESTEP;
        } else {
            accumulator += frameDt;
        }
#else
        accumulator += frameDt;
#endif

        // F4 toggle fullscreen (CORE-005)
#ifdef NUNDERTALE_DEBUG
        if (!ignorePhysicalInput && IsKeyPressed(KEY_F4)) {
#else
        if (IsKeyPressed(KEY_F4)) {
#endif
            ToggleFullscreen();
            config.fullscreen = IsWindowFullscreen();
            m_saveService->saveConfig(config);
        }

        // Hold ESC for 1 second to quit (CORE-006, BUG-14)
#ifdef NUNDERTALE_DEBUG
        if (!ignorePhysicalInput && IsKeyDown(KEY_ESCAPE)) {
#else
        if (IsKeyDown(KEY_ESCAPE)) {
#endif
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
        if (!ignorePhysicalInput && IsKeyPressed(KEY_F1)) {
            s_showDebugOverlay = !s_showDebugOverlay;
        }
        if (!ignorePhysicalInput && IsKeyPressed(KEY_F2)) {
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
#ifdef NUNDERTALE_DEBUG
                     (!recording && !IsWindowFocused()) || m_context->raidPaused);
#else
                     !IsWindowFocused() || m_context->raidPaused);
#endif

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
#ifdef NUNDERTALE_DEBUG
            ++m_debugTicks;
#endif
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
        if (recording) {
            std::ostringstream frameName;
            frameName << "frame_" << std::setfill('0') << std::setw(5)
                      << (m_debugTicks - 1) << ".png";
            const std::filesystem::path framePath =
                std::filesystem::path(m_debugRecordDirectory) / frameName.str();
            Image frame = LoadImageFromTexture(m_virtualTarget.texture);
            ImageFlipVertical(&frame);
            ExportImage(frame, framePath.string().c_str());
            UnloadImage(frame);
        }
#endif

        // 4. Draw virtual canvas scaled & letterboxed into window
        BeginDrawing();
            ClearBackground(BLACK);
            renderLetterboxed();
        EndDrawing();

#ifdef NUNDERTALE_DEBUG
        if (recording && m_debugTicks >= m_debugRecordTickLimit) {
            m_context->isRunning = false;
        }
        if (!m_debugShotPath.empty() && m_debugTicks >= m_debugShotFrames) {
            const std::filesystem::path shotPath(m_debugShotPath);
            std::error_code directoryError;
            if (shotPath.has_parent_path()) {
                std::filesystem::create_directories(shotPath.parent_path(), directoryError);
            }
            Image screenshot = LoadImageFromScreen();
            ExportImage(screenshot, m_debugShotPath.c_str());
            UnloadImage(screenshot);
            m_context->isRunning = false;
        }
#endif
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
