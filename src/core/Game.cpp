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
#ifdef NUNDERTALE_DEBUG
#include "core/Cutscene.hpp"
#include "core/IScene.hpp"
#include <cxxabi.h>
#include <cstdlib>
#include <map>
#include <typeinfo>
#include <unordered_map>
#endif

namespace nundertale {

#ifdef NUNDERTALE_DEBUG
namespace {

// The trace has to see things the scenes keep private (the scene stack, a battle's encounter id, whether a
// cutscene is running). Naming a private member in an explicit template instantiation is the one place the
// language skips access checks, so the scenes stay untouched. Debug builds only.
template <typename Tag, typename Tag::type Member>
struct PrivateAccess {
    friend typename Tag::type accessMember(Tag) { return Member; }
};

struct SceneStackTag {
    using type = std::vector<std::unique_ptr<IScene>> SceneManager::*;
    friend type accessMember(SceneStackTag);
};
struct BattleEncounterTag {
    using type = std::string BattleScene::*;
    friend type accessMember(BattleEncounterTag);
};
struct OverworldCutsceneTag {
    using type = CutsceneRunner OverworldScene::*;
    friend type accessMember(OverworldCutsceneTag);
};
struct OverworldSpawnTag {
    using type = std::string OverworldScene::*;
    friend type accessMember(OverworldSpawnTag);
};
struct BattleSystemTag {
    using type = std::unique_ptr<BattleSystem> BattleScene::*;
    friend type accessMember(BattleSystemTag);
};
struct RunnerCutsceneTag {
    using type = Cutscene CutsceneRunner::*;
    friend type accessMember(RunnerCutsceneTag);
};

template struct PrivateAccess<SceneStackTag, &SceneManager::m_scenes>;
template struct PrivateAccess<BattleEncounterTag, &BattleScene::m_encounterId>;
template struct PrivateAccess<OverworldCutsceneTag, &OverworldScene::m_cutscene>;
template struct PrivateAccess<OverworldSpawnTag, &OverworldScene::m_targetSpawn>;
template struct PrivateAccess<BattleSystemTag, &BattleScene::m_battleSystem>;
template struct PrivateAccess<RunnerCutsceneTag, &CutsceneRunner::m_cutscene>;

const char* battleStateName(BattleState state) {
    switch (state) {
        case BattleState::MenuSelect: return "MenuSelect";
        case BattleState::FightTarget: return "FightTarget";
        case BattleState::ActSelect: return "ActSelect";
        case BattleState::ItemSelect: return "ItemSelect";
        case BattleState::MercySelect: return "MercySelect";
        case BattleState::DialogMessage: return "DialogMessage";
        case BattleState::EnemyTurn: return "EnemyTurn";
        case BattleState::Fleeing: return "Fleeing";
        case BattleState::Fled: return "Fled";
        case BattleState::Victory: return "Victory";
        case BattleState::Defeat: return "Defeat";
    }
    return "?";
}

std::string sceneTypeName(const IScene& scene) {
    int status = 0;
    char* demangled = abi::__cxa_demangle(typeid(scene).name(), nullptr, nullptr, &status);
    std::string name = (status == 0 && demangled != nullptr) ? demangled : typeid(scene).name();
    std::free(demangled);
    const std::string prefix = "nundertale::";
    if (name.compare(0, prefix.size(), prefix) == 0) name.erase(0, prefix.size());
    return name;
}

// Everything that tells two cutscenes apart; the player keeps no id, so a running cutscene is matched against
// data/cutscenes/*.json by this fingerprint.
std::string cutsceneFingerprint(const Cutscene& cutscene) {
    std::string out;
    for (const CutsceneStep& step : cutscene.steps) {
        out += std::to_string(static_cast<int>(step.type)) + '|' + step.key + '|' +
               std::to_string(step.value) + '|' + step.encounter + '|' + step.speaker + '|' +
               step.path + '|' + step.npc + '|' + step.condition + '|' +
               std::to_string(step.seconds) + '|';
        for (const std::string& line : step.lines) out += line + '\n';
        out += ';';
    }
    return out;
}

} // namespace

// Appends one line per observable game event to the --trace file:
//   t=<logic tick> <event> key=value ...
// Events: begin, room, scene_push, scene_pop, cutscene_start, cutscene_end, battle_start, battle_state,
// battle_end, game_over, flag, end. Everything is derived by looking at the game once per logic tick (scene stack, state.currentRoom,
// flags), so no scene carries tracing code.
struct GameTrace {
    explicit GameTrace(const std::string& path) {
        const std::filesystem::path file(path);
        std::error_code error;
        if (file.has_parent_path()) std::filesystem::create_directories(file.parent_path(), error);
        m_out.open(path, std::ios::app);
        if (!m_out) throw std::runtime_error("Cannot open trace file: " + path);
    }

    void begin(GameContext& context, SceneManager& scenes, const std::string& room,
               const std::string& spawn) {
        m_flags = context.state.flags;
        m_room = context.state.currentRoom;
        write(0, "begin", "room=" + room + " spawn=" + spawn);
        write(0, "room", "id=" + m_room + " spawn=" + (spawn.empty() ? "-" : spawn));
        observeScenes(0, context, scenes);
    }

    void tick(int tick, GameContext& context, SceneManager& scenes) {
        observeScenes(tick, context, scenes);
        observeCutscene(tick, scenes);
        observeRoom(tick, context, scenes);
        observeFlags(tick, context);
    }

    void end(int tick, const char* reason) {
        write(tick, "end", std::string("reason=") + reason);
    }

private:
    struct SceneEntry {
        const IScene* scene = nullptr;
        std::string name;
        std::string encounter; // BattleScene only
    };
    struct BattleSnapshot {
        int kills = 0;
        int spares = 0;
        int flees = 0;
    };

    void write(int tick, const std::string& event, const std::string& fields = {}) {
        m_out << "t=" << tick << ' ' << event;
        if (!fields.empty()) m_out << ' ' << fields;
        m_out << std::endl; // flushed per line: a crash still leaves the trace up to that point
    }

    static std::vector<SceneEntry> stackOf(SceneManager& scenes) {
        std::vector<SceneEntry> stack;
        for (const auto& scene : scenes.*accessMember(SceneStackTag{})) {
            SceneEntry entry{ scene.get(), sceneTypeName(*scene), {} };
            if (const auto* battle = dynamic_cast<const BattleScene*>(scene.get())) {
                entry.encounter = battle->*accessMember(BattleEncounterTag{});
            }
            stack.push_back(std::move(entry));
        }
        return stack;
    }

    static OverworldScene* overworldOf(SceneManager& scenes) {
        OverworldScene* found = nullptr;
        for (const auto& scene : scenes.*accessMember(SceneStackTag{})) {
            if (auto* overworld = dynamic_cast<OverworldScene*>(scene.get())) found = overworld;
        }
        return found;
    }

    void observeScenes(int tick, GameContext& context, SceneManager& scenes) {
        std::vector<SceneEntry> stack = stackOf(scenes);
        std::size_t common = 0;
        while (common < stack.size() && common < m_stack.size() &&
               stack[common].scene == m_stack[common].scene) ++common;

        bool gameOverShown = false;
        for (const SceneEntry& entry : stack) gameOverShown |= entry.name == "GameOverScene";
        for (std::size_t i = m_stack.size(); i > common; --i) {
            const SceneEntry& gone = m_stack[i - 1];
            write(tick, "scene_pop", "scene=" + gone.name);
            if (gone.name == "BattleScene") {
                const bool lost = gameOverShown || context.state.player.hp <= 0;
                const char* outcome = "won";
                if (lost) outcome = "lost";
                else if (context.state.fleesTotal > m_battle.flees) outcome = "fled";
                else if (context.state.killsTotal > m_battle.kills) outcome = "killed";
                else if (context.state.sparesTotal > m_battle.spares) outcome = "spared";
                write(tick, "battle_end", "encounter=" + gone.encounter + " outcome=" + outcome +
                                          " hp=" + std::to_string(context.state.player.hp));
            }
        }
        for (std::size_t i = common; i < stack.size(); ++i) {
            const SceneEntry& added = stack[i];
            write(tick, "scene_push", "scene=" + added.name);
            if (added.name == "BattleScene") {
                m_battle = { context.state.killsTotal, context.state.sparesTotal,
                             context.state.fleesTotal };
                write(tick, "battle_start", "encounter=" + added.encounter);
            } else if (added.name == "GameOverScene") {
                write(tick, "game_over", "room=" + context.state.currentRoom);
            }
        }
        m_stack = std::move(stack);
        observeBattleState(tick, scenes);
    }

    // The battle's own state machine (menu, dialog, enemy turn...) is what an input script has to be timed
    // against, so its changes are traced as well.
    void observeBattleState(int tick, SceneManager& scenes) {
        const BattleScene* battle = nullptr;
        for (const auto& scene : scenes.*accessMember(SceneStackTag{})) {
            if (const auto* found = dynamic_cast<const BattleScene*>(scene.get())) battle = found;
        }
        const BattleSystem* system = battle != nullptr
            ? (battle->*accessMember(BattleSystemTag{})).get() : nullptr;
        if (system == nullptr) {
            m_battleState.clear();
            return;
        }
        const std::string state = battleStateName(system->getState());
        if (state == m_battleState) return;
        m_battleState = state;
        write(tick, "battle_state", "state=" + state);
    }

    void loadCutsceneIds() {
        m_cutsceneIdsLoaded = true;
        std::error_code error;
        for (const auto& entry : std::filesystem::directory_iterator("data/cutscenes", error)) {
            if (entry.path().extension() != ".json") continue;
            std::ifstream file(entry.path());
            std::ostringstream json;
            json << file.rdbuf();
            Cutscene cutscene;
            if (!parseCutsceneJson(json.str(), cutscene) || cutscene.steps.empty()) continue;
            std::string& ids = m_cutsceneIds[cutsceneFingerprint(cutscene)];
            if (!ids.empty()) ids += '|';
            ids += entry.path().stem().string();
        }
    }

    std::string cutsceneIdOf(const Cutscene& cutscene) {
        if (!m_cutsceneIdsLoaded) loadCutsceneIds();
        const auto found = m_cutsceneIds.find(cutsceneFingerprint(cutscene));
        return found != m_cutsceneIds.end() ? found->second : "?";
    }

    void observeCutscene(int tick, SceneManager& scenes) {
        OverworldScene* overworld = overworldOf(scenes);
        if (overworld != m_overworld) { // a different Overworld (game over, new game): start over
            m_overworld = overworld;
            m_cutsceneRunning = false;
            m_cutsceneFingerprint.clear();
        }
        if (overworld == nullptr) return;
        const CutsceneRunner& runner = overworld->*accessMember(OverworldCutsceneTag{});
        const Cutscene& cutscene = runner.*accessMember(RunnerCutsceneTag{});
        const bool running = runner.running();
        const std::string fingerprint = cutsceneFingerprint(cutscene);
        // A cutscene that finishes inside the tick it started in is still caught: the runner keeps its last
        // cutscene, so a changed fingerprint means a new one was started.
        const bool started = (running && !m_cutsceneRunning) ||
                             (!fingerprint.empty() && fingerprint != m_cutsceneFingerprint);
        if (started) {
            m_cutsceneId = cutsceneIdOf(cutscene);
            write(tick, "cutscene_start", "id=" + m_cutsceneId);
        }
        if (started ? !running : (!running && m_cutsceneRunning)) {
            write(tick, "cutscene_end", "id=" + m_cutsceneId);
        }
        m_cutsceneRunning = running;
        m_cutsceneFingerprint = fingerprint;
    }

    void observeRoom(int tick, GameContext& context, SceneManager& scenes) {
        if (context.state.currentRoom == m_room) return;
        m_room = context.state.currentRoom;
        std::string spawn = "-";
        if (const OverworldScene* overworld = overworldOf(scenes)) {
            spawn = overworld->*accessMember(OverworldSpawnTag{});
        }
        write(tick, "room", "id=" + m_room + " spawn=" + spawn);
    }

    void observeFlags(int tick, GameContext& context) {
        // std::map keeps same-tick changes in a stable order.
        std::map<std::string, std::pair<int, int>> changed;
        for (const auto& [key, value] : context.state.flags) {
            const auto before = m_flags.find(key);
            const int previous = before != m_flags.end() ? before->second : 0;
            if (before == m_flags.end() || previous != value) changed[key] = { previous, value };
        }
        for (const auto& [key, change] : changed) {
            write(tick, "flag", "name=" + key + " value=" + std::to_string(change.second) +
                                " prev=" + std::to_string(change.first));
            m_flags[key] = change.second;
        }
    }

    std::ofstream m_out;
    std::vector<SceneEntry> m_stack;
    std::unordered_map<std::string, int> m_flags;
    std::string m_room;
    BattleSnapshot m_battle;
    std::string m_battleState;
    OverworldScene* m_overworld = nullptr;
    bool m_cutsceneRunning = false;
    std::string m_cutsceneFingerprint;
    std::string m_cutsceneId;
    std::map<std::string, std::string> m_cutsceneIds;
    bool m_cutsceneIdsLoaded = false;
};
#endif

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
        } else if (argument == "--trace" && index + 1 < argc) {
            m_debugTracePath = argv[++index];
        }
    }
    m_debugFast = !m_debugTracePath.empty() && !m_debugInputPath.empty() &&
                  m_debugRecordDirectory.empty();
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
        if (m_debugFast) SetRandomSeed(0U);
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
    if (!m_debugTracePath.empty()) m_debugTrace = std::make_unique<GameTrace>(m_debugTracePath);
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
    if (m_debugTrace) {
        m_debugTrace->begin(*m_context, *m_sceneManager, m_debugRoom, m_debugSpawn);
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
    if (!m_debugRecordDirectory.empty() || m_debugFast) windowFlags = FLAG_WINDOW_RESIZABLE;
#endif
    SetConfigFlags(windowFlags);
#ifdef NUNDERTALE_DEBUG
    const bool useVirtualWindow = !m_debugShotPath.empty() || !m_debugRecordDirectory.empty() ||
                                  m_debugFast;
    const int initialWidth = useVirtualWindow ? VIRTUAL_WIDTH : WINDOW_WIDTH;
    const int initialHeight = useVirtualWindow ? VIRTUAL_HEIGHT : WINDOW_HEIGHT;
    InitWindow(initialWidth, initialHeight, WINDOW_TITLE);
#else
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
#endif
    SetExitKey(KEY_NULL); // ESC is handled by the fixed-tick hold-to-quit state.
    SetWindowMinSize(640, 480);
#ifdef NUNDERTALE_DEBUG
    SetTargetFPS(m_debugRecordDirectory.empty() && !m_debugFast ? TARGET_FPS : 0);
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
    HoldToQuitState quitHold;

#ifdef NUNDERTALE_DEBUG
    if ((!m_debugRecordDirectory.empty() || m_debugFast) && m_debugRecordTickLimit <= 0) {
        if (m_debugTrace) m_debugTrace->end(0, "tick-limit");
        return;
    }
    const bool recording = !m_debugRecordDirectory.empty();
    // Recording and trace runs step the logic exactly one tick per frame, independent of wall time.
    const bool lockstep = recording || m_debugFast;
    const bool ignorePhysicalInput = recording || !m_debugInputPath.empty();
    const char* endReason = "window-closed";
#endif

    // CORE-007: Apply user settings from %LOCALAPPDATA%/nUndertale/config.json
    const UserConfig initialConfig = m_saveService->getConfig();
    m_audioService->setMusicVolume(initialConfig.musicVolume);
    m_audioService->setMasterVolume(initialConfig.masterVolume);
    bool allowConfiguredFullscreen = true;
#ifdef NUNDERTALE_DEBUG
    allowConfiguredFullscreen = m_debugShotPath.empty() && m_debugRecordDirectory.empty();
#endif
    if (allowConfiguredFullscreen && initialConfig.fullscreen && !IsWindowFullscreen()) {
        ToggleFullscreen();
    }

    while (!WindowShouldClose() && m_context->isRunning && m_sceneManager->hasScenes()) {
        float frameDt = GetFrameTime();
        if (frameDt > 0.2f) frameDt = 0.2f; // prevent spiral of death
#ifdef NUNDERTALE_DEBUG
        if (lockstep) {
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
            UserConfig config = m_saveService->getConfig();
            config.fullscreen = IsWindowFullscreen();
            m_saveService->saveConfig(config);
        }

        // ESC is a global hold-to-quit action. Its transition runs in the fixed loop below.
#ifdef NUNDERTALE_DEBUG
        const bool escapeHeld = !ignorePhysicalInput && IsKeyDown(KEY_ESCAPE);
#else
        const bool escapeHeld = IsKeyDown(KEY_ESCAPE);
#endif

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
        if (escapeHeld) {
            // Keep keyboard ESC from reaching scenes (for example, returning to the title).
            (void)m_inputService->isEscapePressed();
        }
        m_audioService->updateMusic();

        // 2. Fixed 30 Hz logic update loop (CORE-001)
        while (accumulator >= LOGIC_TIMESTEP) {
            m_context->state.playTime += LOGIC_TIMESTEP;
            raidTick(m_context->state.raid, LOGIC_TIMESTEP,
#ifdef NUNDERTALE_DEBUG
                     (!lockstep && !IsWindowFocused()) || m_context->raidPaused);
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
            if (m_debugTrace) m_debugTrace->tick(m_debugTicks, *m_context, *m_sceneManager);
#endif
            const bool quitReached = quitHold.update(escapeHeld);
            m_inputService->endTick();
            accumulator -= LOGIC_TIMESTEP;
            if (quitReached) {
                m_context->isRunning = false;
#ifdef NUNDERTALE_DEBUG
                endReason = "quit";
#endif
                break;
            }
        }
        if (!m_context->isRunning) break;

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
            if (quitHold.active()) {
                const float progress = quitHold.progress();
                Color quitColor = WHITE;
                quitColor.a = static_cast<unsigned char>(
                    std::lround(64.0f + 191.0f * progress));
                const Font quitFont = m_resourceService->getFont8Bit(16);
                m_resourceService->drawText(
                    quitFont, "QUITTING...", { 16.0f, 16.0f }, 16.0f, quitColor, 1.0f);
                DrawRectangle(16, 38, 120, 2, { 255, 255, 255, 48 });
                DrawRectangle(16, 38,
                              static_cast<int>(std::lround(120.0f * progress)), 2,
                              quitColor);
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
        if (lockstep && m_debugTicks >= m_debugRecordTickLimit) {
            m_context->isRunning = false;
            endReason = "tick-limit";
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
            endReason = "shot";
        }
#endif
    }
#ifdef NUNDERTALE_DEBUG
    if (m_debugTrace) m_debugTrace->end(m_debugTicks, endReason);
#endif
}

void Game::renderLetterboxed() {
    const bool integerScale = m_saveService->getConfig().integerScale;
    const VirtualCanvasGeometry geometry = calculateVirtualCanvasGeometry(
        GetScreenWidth(), GetScreenHeight(), integerScale);
    const float destX = geometry.x + m_context->shakeOffset.x * geometry.scale;
    const float destY = geometry.y + m_context->shakeOffset.y * geometry.scale;

    Rectangle sourceRec = { 0.0f, 0.0f, (float)m_virtualTarget.texture.width, -(float)m_virtualTarget.texture.height };
    Rectangle destRec = { destX, destY, geometry.width, geometry.height };

    DrawTexturePro(m_virtualTarget.texture, sourceRec, destRec, { 0, 0 }, 0.0f, WHITE);
}

} // namespace nundertale
