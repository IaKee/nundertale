#include "scenes/OverworldScene.hpp"
#include "core/ToxicTrack.hpp"
#include "scenes/BattleScene.hpp"
#include "scenes/TitleScene.hpp"
#include "scenes/EncounterTransitionScene.hpp"
#include "scenes/ShopScene.hpp"
#include "core/GameContext.hpp"
#include "core/Constants.hpp"
#include "core/SceneManager.hpp"
#include "core/SaveText.hpp"
#include "core/FurReactions.hpp"
#include "core/Cell.hpp"
#include "core/Camera2DFollow.hpp"
#include "core/RgbKeyboard.hpp"
#include "core/RoomObjects.hpp"
#include "services/MusicTracks.hpp"
#include "ui/SpriteDraw.hpp"
#include <raylib.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>

namespace nundertale {

namespace {

constexpr std::array<const char*, RGB_KEY_COUNT> RGB_KEY_FLAGS = {{
    "ruins.r4.key0", "ruins.r4.key1", "ruins.r4.key2", "ruins.r4.key3", "ruins.r4.key4"
}};

int rgbKeyboardMaskFromState(const GameState& state) {
    int mask = 0;
    for (int key = 0; key < RGB_KEY_COUNT; ++key) {
        if (state.getFlag(RGB_KEY_FLAGS[static_cast<std::size_t>(key)], 0) != 0) mask |= 1 << key;
    }
    return mask;
}

void setRgbKeyboardMask(GameState& state, int mask) {
    for (int key = 0; key < RGB_KEY_COUNT; ++key) {
        state.setFlag(RGB_KEY_FLAGS[static_cast<std::size_t>(key)], rgbKeyboardKeyLit(mask, key) ? 1 : 0);
    }
}

} // namespace

OverworldScene::OverworldScene()
    : m_player(Vector2{ 300.0f, 320.0f }) {
    initObstacles();
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

void OverworldScene::loadRoomMap(const std::string& roomId) {
    m_cutsceneFadeAlpha = 0.0f;
    m_cutsceneFadeStart = 0.0f;
    m_cutsceneFadeTarget = 0.0f;
    m_cutsceneFadeElapsed = 0.0f;
    m_cutsceneFadeActive = false;
    m_autoFadeIn = false;
    m_tiledMap = {};
    m_roomWidth = 640.0f;
    m_roomHeight = 480.0f;
    m_backgroundImage.clear();
    m_backgroundOffset = { 0.0f, 0.0f };
    m_saveLinesOverride.clear();
    m_roomEncounters.clear();
    m_waterBursts.clear();
    m_encounterBudget = 0;
    m_encounterStepsRemaining = 0;
    initObstacles();
    m_staticObstacles = m_obstacles;

    TiledMap loaded;
    if (!loadTiledMapFromFile("data/maps/" + roomId + ".json", loaded)) return;

    m_tiledMap = std::move(loaded);
    m_roomWidth = m_tiledMap.pixelWidth();
    m_roomHeight = m_tiledMap.pixelHeight();
    m_saveLinesOverride = tiledStringProperty(m_tiledMap.properties, "savePlace");
    m_roomEncounters = parseEncounterSpec(tiledStringProperty(m_tiledMap.properties, "encounters"));
    m_encounterBudget = tiledIntProperty(m_tiledMap.properties, "encounterBudget", 0);
    if (!m_roomEncounters.empty()) {
        m_encounterStepsRemaining = randomEncounterStepCountdown(m_encounterRng);
    }

    for (const TiledLayer& layer : m_tiledMap.layers) {
        if (layer.type == TiledLayerType::Image && !layer.image.empty()) {
            m_backgroundImage = layer.image;
            m_backgroundOffset = { layer.offsetX, layer.offsetY };
            break;
        }
    }

    const TiledLayer* collision = m_tiledMap.findLayer("collision");
    if (collision != nullptr && collision->type == TiledLayerType::Objects) {
        m_obstacles.clear();
        for (const TiledObject& object : collision->objects) {
            const TiledRect bounds = object.bounds();
            m_obstacles.push_back({ bounds.x, bounds.y, bounds.width, bounds.height });
        }
    }
    m_staticObstacles = m_obstacles;
}

Rectangle OverworldScene::objectBounds(const std::string& name, Rectangle fallback) const {
    const TiledObject* object = m_tiledMap.findObject(name);
    if (object == nullptr) return fallback;
    const TiledRect bounds = object->bounds();
    return { bounds.x, bounds.y, bounds.width, bounds.height };
}

Vector2 OverworldScene::objectPosition(const std::string& name, Vector2 fallback) const {
    const TiledObject* object = m_tiledMap.findObject(name);
    return object == nullptr ? fallback : Vector2{ object->x, object->y };
}

static Direction roomDirection(const std::string& name) {
    if (name == "up") return Direction::Up;
    if (name == "left") return Direction::Left;
    if (name == "right") return Direction::Right;
    return Direction::Down;
}

static std::string spriteJsonPath(const std::string& imagePath) {
    std::filesystem::path path(imagePath);
    path.replace_extension(".json");
    return path.generic_string();
}

// BUG-40: the spawn's pose (e.g. Grub lying on the plush pile in R1) is set with the position, before the
// first update and draw, so no standing frame is ever drawn ahead of a cutscene.
static void applySpawnPose(Player& player, const TiledObject* spawn, const GameContext& context) {
    if (spawn == nullptr) return;
    const SpawnPose pose = spawnPoseOf(*spawn);
    const int untilValue = pose.untilFlag.empty() ? 0 : context.state.getFlag(pose.untilFlag, 0);
    if (spawnPoseApplies(pose, untilValue)) {
        player.setOverrideAnimation(pose.sheet, pose.anim, 0.0f);
    }
}

void OverworldScene::initRoomObjects(GameContext& context) {
    m_roomObjects.clear();
    m_savePoint.reset();
    m_triggerOverlaps.clear();
    m_loggedConditions.clear();
    m_hazardLinesShown.clear();
    m_hiddenRoomObjects.clear();
    m_roomObjectAnimations.clear();
    m_bumpTrackers.clear();
    m_hazardCooldown = 0;
    m_lastHazardSafePosition = m_player.getPosition();

    // OVW-010, PRG-020: ruins.barky.state is 0 = none, 1 = spared, 2 = killed. The battle sets it with
    // the spared/killed flags; fill it in when only those are set (older saves, --flag).
    if (context.state.getFlag("ruins.barky.state", 0) == 0) {
        if (context.state.getFlag("ruins.barky.killed", 0) != 0) context.state.setFlag("ruins.barky.state", 2);
        else if (context.state.getFlag("ruins.barky.spared", 0) != 0) context.state.setFlag("ruins.barky.state", 1);
    }

    if (context.state.currentRoom == "ruins_switches" &&
        context.state.getFlag("ruins.r4.rgb_initialized", 0) == 0) {
        const int initialMask = context.state.getFlag("ruins.r4.solved", 0) != 0
            ? RGB_KEY_TARGET_MASK : RGB_KEY_START_MASK;
        setRgbKeyboardMask(context.state, initialMask);
        context.state.setFlag("ruins.r4.rgb_initialized", 1);
    }

    for (const TiledLayer& layer : m_tiledMap.layers) {
        if (layer.type != TiledLayerType::Objects || layer.name != "objects") continue;
        for (const TiledObject& object : layer.objects) {
            m_roomObjects.push_back(object);
            if (tiledObjectType(object) == "savepoint") {
                m_savePoint = std::make_unique<SavePoint>(Vector2{ object.x, object.y });
                const std::string lines = tiledStringProperty(object.properties, "lines");
                if (!lines.empty()) m_saveLinesOverride = lines;
            }
        }
    }
    refreshDynamicObstacles(context);
}

bool OverworldScene::roomObjectVisible(const TiledObject& object, GameContext& context) {
    if (!object.name.empty() && m_hiddenRoomObjects.find(object.name) != m_hiddenRoomObjects.end()) {
        return false;
    }
    const auto lookup = [&context](const std::string& key) {
        if (key == "fur") return context.state.furry.value;
        if (key == "hp") return context.player.hp;
        const int inv = evaluateInventoryCondition(key, context.player.inventory, [&context](const std::string& name) {
            const ItemDef* def = context.itemDatabase.findItemByName(name);
            return def != nullptr ? def->id : "";
        });
        if (inv >= 0) return inv;
        return context.state.getFlag(key, 0);
    };
    const std::string showIf = tiledStringProperty(object.properties, "showIf");
    const std::string hideIf = tiledStringProperty(object.properties, "hideIf");
    bool showParsed = true;
    bool hideParsed = true;
    const bool visible = roomObjectConditionVisible(
        showIf, hideIf, lookup, &showParsed, &hideParsed);
    const auto logInvalid = [this, &object](const std::string& propertyName,
                                            const std::string& condition) {
        const std::string key = std::to_string(object.id) + ":" + propertyName + ":" + condition;
        if (m_loggedConditions.insert(key).second) {
            std::clog << "Invalid room condition '" << condition << "'; treating it as true.\n";
        }
    };
    if (!showParsed) logInvalid("showIf", showIf);
    if (!hideParsed) logInvalid("hideIf", hideIf);
    return visible;
}

void OverworldScene::refreshDynamicObstacles(GameContext& context) {
    m_obstacles = m_staticObstacles;
    for (const TiledObject& object : m_roomObjects) {
        if (!roomObjectVisible(object, context)) continue;
        const std::string type = tiledObjectType(object);
        const bool solid = type == "npc"
            ? tiledBoolProperty(object.properties, "solid", true)
            : type == "prop" && tiledBoolProperty(object.properties, "solid", false);
        if (!solid) continue;
        const TiledRect bounds = object.bounds();
        m_obstacles.push_back({ bounds.x, bounds.y, bounds.width, bounds.height });
    }
}

void OverworldScene::updateBumpCutscenes(float dt, GameContext& context) {
    for (const TiledObject& object : m_roomObjects) {
        const std::string type = tiledObjectType(object);
        const bool solid = type == "npc"
            ? tiledBoolProperty(object.properties, "solid", true)
            : type == "prop" && tiledBoolProperty(object.properties, "solid", false);
        const std::string cutsceneId = tiledStringProperty(object.properties, "bumpCutscene");
        if (!solid || cutsceneId.empty()) continue;

        bool pushingInto = false;
        if (roomObjectVisible(object, context)) {
            const TiledRect tiledBounds = object.bounds();
            pushingInto = m_player.wasBlockedBy({
                tiledBounds.x, tiledBounds.y, tiledBounds.width, tiledBounds.height
            });
        }
        if (m_bumpTrackers[object.id].update(pushingInto, dt)) {
            startCutscene(cutsceneId);
            if (m_cutscene.running()) return;
        }
    }
}

static bool loadCutsceneFile(const std::string& id, Cutscene& cutscene) {
    if (id.empty()) return false;
    std::ifstream file("data/cutscenes/" + id + ".json");
    if (!file) return false;
    std::ostringstream json;
    json << file.rdbuf();
    return parseCutsceneJson(json.str(), cutscene) && !cutscene.steps.empty();
}

void OverworldScene::startCutscene(const std::string& id) {
    Cutscene cutscene;
    if (loadCutsceneFile(id, cutscene)) {
        m_autoFadeIn = false;
        m_cutsceneStartsBattle = cutsceneStartsBattle(cutscene);
        m_cutsceneBattleStarted = false;
        m_pendingTriggerOnce.clear();
        m_cutscene.start(std::move(cutscene));
    }
}

void OverworldScene::interactWithRoomObject(const TiledObject& object, GameContext& context) {
    const std::string type = tiledObjectType(object);
    if (type == "itemPickup") {
        const std::string flag = tiledStringProperty(object.properties, "flag");
        if (!flag.empty() && context.state.getFlag(flag, 0) != 0) {
            const std::string emptyLines = tiledStringProperty(object.properties, "emptyLines");
            if (!emptyLines.empty()) m_dialogBox.startDialog(splitRoomPages(emptyLines), "");
            return;
        }
        if (!context.player.canAddItem()) {
            m_dialogBox.startDialog(splitRoomPages(tiledStringProperty(
                object.properties, "fullLines", "* You're carrying too much.")), "");
            return;
        }
        const std::string item = tiledStringProperty(object.properties, "item");
        if (item.empty() || context.itemDatabase.findItemByName(item) == nullptr) return;
        context.player.addItem(item);
        if (!flag.empty()) context.state.setFlag(flag, 1);
        const std::string lines = tiledStringProperty(object.properties, "lines");
        if (!lines.empty()) m_dialogBox.startDialog(splitRoomPages(lines), "");
        return;
    }

    if (type != "sign" && type != "examine" && type != "prop" && type != "npc") return;
    const int rgbKey = tiledIntProperty(object.properties, "rgbKey", -1);
    if (type == "prop" && rgbKey >= 0 && rgbKey < RGB_KEY_COUNT) {
        if (context.state.getFlag("ruins.r4.solved", 0) != 0) {
            m_dialogBox.startDialog({ "* The keyboard is locked on pink." }, "");
            return;
        }
        const int nextMask = pressRgbKeyboardKey(rgbKeyboardMaskFromState(context.state), rgbKey);
        setRgbKeyboardMask(context.state, nextMask);
        context.audio.playSound("assets/sfx/switch_click.ogg", 0.6f);
        if (rgbKeyboardSolved(nextMask)) {
            context.state.setFlag("ruins.r4.solved", 1);
            m_dialogBox.startDialog({ "* The keyboard locks into pink. The cable barrier retracts." }, "");
        } else if (context.state.getFlag("ruins.r4.keys_thought", 0) == 0) {
            startCutscene(tiledStringProperty(object.properties, "cutscene", "r4_keys"));
        } else {
            m_dialogBox.startDialog({ "* The neighboring keys change color." }, "");
        }
        return;
    }
    std::vector<std::string> variants;
    const std::string first = tiledStringProperty(object.properties, "lines");
    const std::string cutsceneId = tiledStringProperty(object.properties, "cutscene");
    if (first.empty() && cutsceneId.empty()) return;
    if (!first.empty()) variants.push_back(first);
    for (int index = 2; index < 100; ++index) {
        const std::string value = tiledStringProperty(object.properties, "lines" + std::to_string(index));
        if (value.empty()) break;
        variants.push_back(value);
    }
    const std::string counterKey = tiledStringProperty(object.properties, "counter");
    const int counter = counterKey.empty() ? 0 : context.state.getFlag(counterKey, 0);
    const int furMin = tiledIntProperty(object.properties, "furMin", 101);
    if (!counterKey.empty()) context.state.incFlag(counterKey);
    if (type == "npc" && !tiledBoolProperty(object.properties, "noTurn", false)) {
        const TiledRect npcBounds = object.bounds();
        const Rectangle playerBox = m_player.getCollisionBox();
        m_talkingObjectId = object.id;
        m_talkingFacing = faceTowardDirection(
            npcBounds.x + npcBounds.width * 0.5f, npcBounds.y + npcBounds.height * 0.5f,
            playerBox.x + playerBox.width * 0.5f, playerBox.y + playerBox.height * 0.5f);
    }
    const bool firstSwitchFlip = counter == 0 && counterKey.rfind("ruins.r4.sw", 0) == 0;
    context.audio.playSound(
        firstSwitchFlip ? "assets/sfx/switch_click.ogg" : "assets/sfx/menu_select.wav", 0.6f);
    if (!cutsceneId.empty()) {
        startCutscene(cutsceneId);
        return;
    }
    const std::string selected = selectRoomVariant(
        variants, counter, context.state.furry.value, furMin,
        tiledStringProperty(object.properties, "furLines"));
    m_dialogBox.startDialog(
        splitRoomPages(selected), tiledStringProperty(object.properties, "speaker"));
}

void OverworldScene::updateHazards(GameContext& context) {
    if (m_hazardCooldown > 0) --m_hazardCooldown;
    const Rectangle playerBounds = m_player.getCollisionBox();
    const TiledObject* hit = nullptr;
    for (const TiledObject& object : m_roomObjects) {
        if (tiledObjectType(object) != "hazard" || !roomObjectVisible(object, context)) continue;
        const TiledRect tiledBounds = object.bounds();
        const Rectangle bounds = { tiledBounds.x, tiledBounds.y, tiledBounds.width, tiledBounds.height };
        if (CheckCollisionRecs(playerBounds, bounds)) {
            hit = &object;
            break;
        }
    }
    if (hit == nullptr) {
        m_lastHazardSafePosition = m_player.getPosition();
        return;
    }
    if (m_hazardCooldown > 0) return;

    context.player.hp = applyHazardDamage(
        context.player.hp, tiledIntProperty(hit->properties, "damage", 2));
    context.audio.playSound("assets/sfx/zap.ogg", 0.85f);
    context.flash({ 255, 48, 48, 255 }, 0.12f);
    context.shake(2.0f, 0.12f);
    m_player.setPosition(m_lastHazardSafePosition);
    context.state.playerPos = m_lastHazardSafePosition;
    m_hazardCooldown = 20;
    const std::string lines = tiledStringProperty(hit->properties, "lines");
    if (!lines.empty() && m_hazardLinesShown.insert(hit->id).second) {
        m_dialogBox.startDialog(splitRoomPages(lines), "");
    }
}

void OverworldScene::beginDoorTransition(const TiledObject& door) {
    m_targetRoom = tiledStringProperty(door.properties, "targetRoom");
    if (m_targetRoom.empty()) return;
    m_targetSpawn = tiledStringProperty(door.properties, "targetSpawn", "spawn");
    m_targetFacing = tiledStringProperty(door.properties, "facing");
    m_doorFadeState = DoorFadeState::Out;
    m_doorFadeFrame = 0;
}

void OverworldScene::swapDoorRoom(GameContext& context) {
    context.state.currentRoom = m_targetRoom;
    context.state.playerPos = { 0.0f, 0.0f };
    loadRoomMap(m_targetRoom);
    initRoomObjects(context);

    const TiledObject* spawn = findRoomSpawn(m_tiledMap, m_targetSpawn);
    if (spawn == nullptr) spawn = findRoomSpawn(m_tiledMap, "spawn");
    if (spawn != nullptr) {
        m_player.setPosition({ spawn->x, spawn->y });
        const std::string facing = m_targetFacing.empty()
            ? tiledStringProperty(spawn->properties, "facing", "down")
            : m_targetFacing;
        m_player.setDirection(roomDirection(facing));
        applySpawnPose(m_player, spawn, context);
    }
    m_lastHazardSafePosition = m_player.getPosition();
    context.state.playerPos = m_player.getPosition();
    const MusicEntry track = musicFor(roomMusicKey(context.state.currentRoom));
    context.audio.playMusic(track.path, true, track.loopStart, track.loopEnd);
    context.audio.setMusicPitch(overworldMusicPitch(toxicTrack(context.state), context.state.currentRoom));
}

void OverworldScene::updateDoorTransition(GameContext& context) {
    if (m_doorFadeState == DoorFadeState::Out) {
        ++m_doorFadeFrame;
        if (m_doorFadeFrame >= 8) {
            swapDoorRoom(context);
            m_doorFadeState = DoorFadeState::In;
            m_doorFadeFrame = 8;
        }
    } else if (m_doorFadeState == DoorFadeState::In) {
        --m_doorFadeFrame;
        if (m_doorFadeFrame <= 0) {
            m_doorFadeFrame = 0;
            m_doorFadeState = DoorFadeState::None;
        }
    }
}

void OverworldScene::updateDoorsAndTriggers(GameContext& context) {
    const Rectangle playerBounds = m_player.getCollisionBox();
    std::unordered_set<int> overlaps;
    for (const TiledObject& object : m_roomObjects) {
        if (!roomObjectVisible(object, context)) continue;
        const std::string type = tiledObjectType(object);
        const TiledRect tiledBounds = object.bounds();
        const Rectangle bounds = { tiledBounds.x, tiledBounds.y, tiledBounds.width, tiledBounds.height };
        if (!CheckCollisionRecs(playerBounds, bounds)) continue;
        if (type == "door") {
            beginDoorTransition(object);
            return;
        }
        if (type != "trigger") continue;
        overlaps.insert(object.id);
        if (m_triggerOverlaps.find(object.id) != m_triggerOverlaps.end()) continue;
        const std::string once = tiledStringProperty(object.properties, "once");
        if (!once.empty() && context.state.getFlag(once, 0) != 0) continue;
        const std::string cutsceneId = tiledStringProperty(object.properties, "cutscene");
        startCutscene(cutsceneId);
        if (m_cutscene.running()) {
            if (!once.empty()) {
                // A fight cutscene burns its once-flag only when the battle starts (see update()).
                if (triggerOnceShouldBurn(m_cutsceneStartsBattle, false)) context.state.setFlag(once, 1);
                else m_pendingTriggerOnce = once;
            }
            m_triggerOverlaps = std::move(overlaps);
            return;
        }
    }
    m_triggerOverlaps = std::move(overlaps);
}

std::vector<std::string> OverworldScene::currentSaveLines(GameContext& context) const {
    if (context.state.getRoute() == GameState::RouteType::ToxicGamer) {
        return { "* Determination." };
    }
    std::vector<std::string> lines = splitRoomPages(
        m_saveLinesOverride.empty() ? savePlaceLine(context.state.currentRoom) : m_saveLinesOverride);
    if (context.state.furry.value > 0) lines.push_back("* DETERMINATION can't fix fluff.");
    return lines;
}

std::string OverworldScene::roomDisplayName(GameContext& context) const {
    const std::string displayName = tiledStringProperty(m_tiledMap.properties, "displayName");
    return displayName.empty() ? context.save.getRoomDisplayName(context.state.currentRoom) : displayName;
}

std::string OverworldScene::roomMusicKey(const std::string& roomId) const {
    const std::string mapMusic = tiledStringProperty(m_tiledMap.properties, "music");
    return mapMusic.empty() ? "overworld." + roomId : mapMusic;
}

static float getRaidStartSeconds() {
    static const float s_raidStartSeconds = []() {
        constexpr int fallbackSeconds = 1320;
        std::ifstream file("data/raid.json");
        if (!file) return static_cast<float>(fallbackSeconds);
        std::ostringstream json;
        json << file.rdbuf();
        return static_cast<float>(raidStartSecondsFromJson(json.str(), fallbackSeconds));
    }();
    return s_raidStartSeconds;
}

static float s_prevRaidTimeLeft = -1.0f;

void OverworldScene::onEnter(GameContext& context) {
    const float raidStartSeconds = getRaidStartSeconds();

    loadRoomMap(context.state.currentRoom);
    // RAD-003: in R1 the clock starts at the squad message of r1_wake (see update()), not on entering.
    if (raidShouldStartOnEnter(context.state.raid, context.state.currentRoom,
                               context.state.getFlag(std::string(RAID_FLAG_SQUAD_MSG), 0))) {
        raidStart(context.state.raid, raidStartSeconds);
    }
    if (s_prevRaidTimeLeft < 0.0f && context.state.raid.started) {
        s_prevRaidTimeLeft = context.state.raid.timeLeft;
    }
    context.raidPaused = false;
    const MusicEntry enterTrack = musicFor(roomMusicKey(context.state.currentRoom));
    context.audio.playMusic(enterTrack.path, true, enterTrack.loopStart, enterTrack.loopEnd);
    context.audio.setMusicPitch(overworldMusicPitch(toxicTrack(context.state), context.state.currentRoom));
    const std::string requestedSpawn = context.launchSpawn.empty() ? "spawn" : context.launchSpawn;
    const TiledObject* spawn = findRoomSpawn(m_tiledMap, requestedSpawn);
    if (!context.launchSpawn.empty() && spawn != nullptr) {
        m_player.setPosition({ spawn->x, spawn->y });
        m_player.setDirection(roomDirection(tiledStringProperty(spawn->properties, "facing", "down")));
        applySpawnPose(m_player, spawn, context);
        context.launchSpawn.clear();
    } else if (context.state.playerPos.x != 0.0f && context.state.playerPos.y != 0.0f) {
        m_player.setPosition(context.state.playerPos);
    } else {
        if (spawn != nullptr) {
            m_player.setPosition({ spawn->x, spawn->y });
            m_player.setDirection(roomDirection(tiledStringProperty(spawn->properties, "facing", "down")));
            applySpawnPose(m_player, spawn, context);
        } else {
            m_player.setPosition({ 300.0f, 320.0f });
        }
    }
    context.state.playerPos = m_player.getPosition();
    initRoomObjects(context);
}

void OverworldScene::onResume(GameContext& context) {
    context.raidPaused = (m_menuState != OverworldMenuState::Closed);
    // AUD-004: back from a battle, the area track restarts from the beginning (even if it is the battle file)
    context.audio.stopMusic();
    const MusicEntry resumeTrack = musicFor(roomMusicKey(context.state.currentRoom));
    context.audio.playMusic(resumeTrack.path, true, resumeTrack.loopStart, resumeTrack.loopEnd);
    context.audio.setMusicPitch(overworldMusicPitch(toxicTrack(context.state), context.state.currentRoom));
    initRoomObjects(context);
    const std::string endCutscene = takePendingCutscene(context.state.pendingCutscene);
    if (!endCutscene.empty()) startCutscene(endCutscene);
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
            context.audio.playSound("assets/sfx/menu_move.wav", 0.4f);
        } else if (context.input.isDownPressed()) {
            m_menuIndex = (m_menuIndex + 1) % 3;
            context.audio.playSound("assets/sfx/menu_move.wav", 0.4f);
        }

        if (context.input.isCancelPressed() || IsKeyPressed(KEY_C) || IsKeyPressed(KEY_LEFT_CONTROL)) {
            m_menuState = OverworldMenuState::Closed;
            context.audio.playSound("assets/sfx/menu_select.wav", 0.3f);
            return;
        }

        if (context.input.isConfirmPressed()) {
            context.audio.playSound("assets/sfx/menu_select.wav", 0.7f);
            if (m_menuIndex == 0) {
                m_menuState = OverworldMenuState::Item;
                m_itemSubIndex = 0;
            } else if (m_menuIndex == 1) {
                m_menuState = OverworldMenuState::Stat;
            } else if (m_menuIndex == 2) {
                m_menuState = OverworldMenuState::Cell;
                m_cellSubState = CellSubState::Contacts;
                m_cellIndex = 0;
                m_furiellIndex = 0;
            }
        }
    } else if (m_menuState == OverworldMenuState::Stat) {
        if (context.input.isCancelPressed() || context.input.isConfirmPressed() || IsKeyPressed(KEY_C)) {
            m_menuState = OverworldMenuState::Main;
            context.audio.playSound("assets/sfx/menu_select.wav", 0.4f);
        }
    } else if (m_menuState == OverworldMenuState::Item) {
        int invSize = (int)context.player.inventory.size();
        if (invSize > 0) {
            if (context.input.isUpPressed()) {
                m_itemSubIndex = (m_itemSubIndex - 1 + invSize) % invSize;
                context.audio.playSound("assets/sfx/menu_move.wav", 0.4f);
            } else if (context.input.isDownPressed()) {
                m_itemSubIndex = (m_itemSubIndex + 1) % invSize;
                context.audio.playSound("assets/sfx/menu_move.wav", 0.4f);
            }
            if (context.input.isConfirmPressed()) {
                m_itemActionIndex = 0;
                m_menuState = OverworldMenuState::ItemAction;
                context.audio.playSound("assets/sfx/menu_select.wav", 0.7f);
            }
        }
        if (context.input.isCancelPressed() || IsKeyPressed(KEY_C)) {
            m_menuState = OverworldMenuState::Main;
            context.audio.playSound("assets/sfx/menu_select.wav", 0.4f);
        }
    } else if (m_menuState == OverworldMenuState::ItemAction) {
        if (context.player.inventory.empty()) {
            m_menuState = OverworldMenuState::Item;
            m_itemSubIndex = 0;
            return;
        }

        if (context.input.isLeftPressed()) {
            m_itemActionIndex = (m_itemActionIndex - 1 + 3) % 3;
            context.audio.playSound("assets/sfx/menu_move.wav", 0.4f);
        } else if (context.input.isRightPressed()) {
            m_itemActionIndex = (m_itemActionIndex + 1) % 3;
            context.audio.playSound("assets/sfx/menu_move.wav", 0.4f);
        }

        if (context.input.isCancelPressed() || IsKeyPressed(KEY_C)) {
            m_menuState = OverworldMenuState::Item;
            context.audio.playSound("assets/sfx/menu_select.wav", 0.4f);
            return;
        }

        if (context.input.isConfirmPressed()) {
            const std::string item = context.player.inventory[m_itemSubIndex];
            const ItemDef* itemDef = context.itemDatabase.findItemByName(item);
            m_menuState = OverworldMenuState::Closed;

            if (m_itemActionIndex == 1) {
                const std::string infoText = (item == "Bottle")
                    ? "* A bottle. It's not Mountain Dew.\n* (You don't want to talk about it.)"
                    : (itemDef != nullptr && !itemDef->info.empty()) ? itemDef->info : "* " + item + ".";
                m_dialogBox.startDialog(splitRoomPages(infoText), "");
                return;
            }

            if (m_itemActionIndex == 2) {
                if (itemDef != nullptr && itemDef->noDrop) {
                    const std::string dropRefusal = !itemDef->dropText.empty()
                        ? itemDef->dropText
                        : "* (Forbidden candy.)";
                    m_dialogBox.startDialog(splitRoomPages(dropRefusal), "");
                    return;
                }
                context.player.inventory.erase(context.player.inventory.begin() + m_itemSubIndex);
                const std::string dropText = (itemDef != nullptr && !itemDef->dropText.empty())
                    ? itemDef->dropText
                    : (item == "Bottle")
                        ? "* You throw it away. The Underground will remember."
                        : "* The " + item + " was thrown away.";
                m_itemSubIndex = std::max(0, std::min(m_itemSubIndex, (int)context.player.inventory.size() - 1));
                m_dialogBox.startDialog(splitRoomPages(dropText), "");
                return;
            }

            if (itemDef != nullptr && itemDef->type == "key") {
                const std::string useText = !itemDef->useText.empty()
                    ? itemDef->useText
                    : (item == "Bottle" ? "* You consider drinking from the bottle.\n* ...No. Absolutely not." : "* You used the " + item + ".");
                m_dialogBox.startDialog(splitRoomPages(useText), "");
                return;
            }

            if (item == "Bottle") {
                m_dialogBox.startDialog({ "* You consider drinking from the bottle.\n* ...No. Absolutely not." }, "");
                return;
            }

            context.player.inventory.erase(context.player.inventory.begin() + m_itemSubIndex);
            if ((itemDef != nullptr && itemDef->type == "weapon") ||
                (itemDef == nullptr && (item.find("Mouse") != std::string::npos || item.find("Keyboard") != std::string::npos || item.find("Weapon") != std::string::npos))) {
                // PRG-002: Equip weapon
                std::string oldWeapon = context.player.weapon;
                context.player.weapon = item;
                context.player.weaponAt = (itemDef != nullptr) ? itemDef->at : ((item.find("Keyboard") != std::string::npos) ? 4 : 2);
                context.player.inventory.push_back(oldWeapon);
                context.audio.playSound("assets/sfx/heal.wav", 0.8f);
                m_dialogBox.startDialog({ "* You equipped the " + item + ".\n* Attack power updated!" }, "");
            } else if ((itemDef != nullptr && itemDef->type == "armor") ||
                       (itemDef == nullptr && (item.find("Headset") != std::string::npos || item.find("Hoodie") != std::string::npos || item.find("Armor") != std::string::npos))) {
                // PRG-002: Equip armor
                std::string oldArmor = context.player.armor;
                context.player.armor = item;
                context.player.armorDf = (itemDef != nullptr) ? itemDef->df : ((item.find("Hoodie") != std::string::npos) ? 3 : 1);
                context.player.inventory.push_back(oldArmor);
                context.audio.playSound("assets/sfx/heal.wav", 0.8f);
                std::string equipText = "* You equipped the " + item + ".\n* Defense updated!";
                if (itemDef != nullptr && itemDef->furry > 0 && !itemDef->id.empty()) {
                    const std::string flag = "fur_on_equip." + itemDef->id;
                    if (!context.state.hasFlag(flag)) {
                        const bool isToxic = context.state.getRoute() == GameState::RouteType::ToxicGamer;
                        const FurChange furChange = applyFurCapped(context.state.furry.value, context.state.furry.stageMax, itemDef->furry, isToxic);
                        context.state.setFlag(flag, 1);
                        if (furChange.stageUp) {
                            equipText += "\n* You feel... fluffier.";
                        }
                        if (furChange.reachedMirror) {
                            context.state.setFlag("fur_mirror_pending", 1);
                        }
                        if (furChange.capped && !context.state.hasFlag("fur_toxic_cap_msg.ruins")) {
                            context.state.setFlag("fur_toxic_cap_msg.ruins", 1);
                            equipText += "\n* Your vitriol burns the fluff right off.";
                        }
                    }
                }
                m_dialogBox.startDialog({ equipText }, "");
            } else if (item.find("Dew") != std::string::npos) {
                context.player.hp = std::min(context.player.maxHp, context.player.hp + 10);
                context.audio.playSound("assets/sfx/heal.wav", 0.8f);
                m_dialogBox.startDialog({ "* You drank the warm Mountain Dew.\n* You recovered 10 HP!" }, "");
            } else if (item.find("Doritos") != std::string::npos) {
                context.player.hp = std::min(context.player.maxHp, context.player.hp + 6);
                context.audio.playSound("assets/sfx/heal.wav", 0.8f);
                m_dialogBox.startDialog({ "* You ate the stale Doritos.\n* You recovered 6 HP!" }, "");
            } else if (item.find("Duct Tape") != std::string::npos || item.find("Tape") != std::string::npos) {
                context.audio.playSound("assets/sfx/heal.wav", 0.8f);
                if (context.state.furry.value <= 0) {
                    m_dialogBox.startDialog({ DUCT_TAPE_EMPTY_TEXT }, "");
                } else {
                    applyFur(context.state.furry.value, context.state.furry.stageMax, DUCT_TAPE_FUR_LOSS);
                    const std::string tapeText = (itemDef != nullptr && !itemDef->useText.empty())
                        ? itemDef->useText
                        : DUCT_TAPE_DEFAULT_TEXT;
                    m_dialogBox.startDialog({ tapeText }, "");
                }
            } else if (itemDef != nullptr && itemDef->id == "apurrture_cake") {
                // C1-014, LORE §20.1: full HP, and it is delicious.
                context.player.hp = context.player.maxHp;
                context.audio.playSound("assets/sfx/heal.wav", 0.8f);
                m_dialogBox.startDialog(splitRoomPages(itemDef->useText), "");
            } else {
                context.player.hp = std::min(context.player.maxHp, context.player.hp + 8);
                context.audio.playSound("assets/sfx/heal.wav", 0.8f);
                m_dialogBox.startDialog({ "* You used " + item + ".\n* Recovered 8 HP." }, "");
            }
            return;
        }
    } else if (m_menuState == OverworldMenuState::Cell) {
        if (m_cellSubState == CellSubState::Contacts) {
            auto entries = cellEntries([&context](const std::string& flag) {
                return context.state.hasFlag(flag);
            });
            int count = static_cast<int>(entries.size());
            if (count > 0) {
                m_cellIndex = std::clamp(m_cellIndex, 0, count - 1);
                if (context.input.isUpPressed()) {
                    m_cellIndex = (m_cellIndex - 1 + count) % count;
                    context.audio.playSound("assets/sfx/menu_move.wav", 0.4f);
                } else if (context.input.isDownPressed()) {
                    m_cellIndex = (m_cellIndex + 1) % count;
                    context.audio.playSound("assets/sfx/menu_move.wav", 0.4f);
                }
            }

            if (context.input.isCancelPressed() || IsKeyPressed(KEY_C) || IsKeyPressed(KEY_LEFT_CONTROL)) {
                m_menuState = OverworldMenuState::Main;
                context.audio.playSound("assets/sfx/menu_select.wav", 0.4f);
                return;
            }

            if (context.input.isConfirmPressed()) {
                context.audio.playSound("assets/sfx/menu_select.wav", 0.7f);
                if (count > 0) {
                    const std::string& selected = entries[static_cast<size_t>(m_cellIndex)];
                    if (selected == "Furiell") {
                        m_cellSubState = CellSubState::FuriellMenu;
                        m_furiellIndex = 0;
                    } else if (selected == "Guild" || selected == "Mom") {
                        m_menuState = OverworldMenuState::Closed;
                        m_dialogBox.startDialog({ cellReceiveOnlyText() }, "");
                    } else if (selected == "Homework (400 GB)") {
                        m_menuState = OverworldMenuState::Closed;
                        m_dialogBox.startDialog({ cellHomeworkText() }, "");
                    } else if (selected == "Papyrus" || selected == "Alphys") {
                        m_menuState = OverworldMenuState::Closed;
                        m_dialogBox.startDialog({ cellNoAnswerText() }, "");
                    }
                }
                return;
            }
        } else if (m_cellSubState == CellSubState::FuriellMenu) {
            const auto& opts = furiellOptions();
            int count = static_cast<int>(opts.size());
            if (count > 0) {
                m_furiellIndex = std::clamp(m_furiellIndex, 0, count - 1);
                if (context.input.isUpPressed()) {
                    m_furiellIndex = (m_furiellIndex - 1 + count) % count;
                    context.audio.playSound("assets/sfx/menu_move.wav", 0.4f);
                } else if (context.input.isDownPressed()) {
                    m_furiellIndex = (m_furiellIndex + 1) % count;
                    context.audio.playSound("assets/sfx/menu_move.wav", 0.4f);
                }
            }

            if (context.input.isCancelPressed() || IsKeyPressed(KEY_C) || IsKeyPressed(KEY_LEFT_CONTROL)) {
                m_cellSubState = CellSubState::Contacts;
                context.audio.playSound("assets/sfx/menu_select.wav", 0.4f);
                return;
            }

            if (context.input.isConfirmPressed()) {
                context.audio.playSound("assets/sfx/menu_select.wav", 0.7f);
                if (count > 0) {
                    const auto& chosen = opts[static_cast<size_t>(m_furiellIndex)];
                    m_menuState = OverworldMenuState::Closed;
                    m_dialogBox.startDialog({ chosen.second }, "Furiell");
                }
                return;
            }
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

        // Furry route state is not represented in GameState::RouteType yet. Hide
        // these lines when that route is added and FLF replaces HP.
        const FurStatText furText = furStatText(context.state.furry.value);
        Color furColor = (context.state.furry.value > 0) ? KR_PURPLE : WHITE;
        context.resources.drawText(font, furText.valueLine, { 210, 305 }, 18, furColor, 1.0f);
        context.resources.drawText(font, furText.stageLine, { 210, 329 }, 18, furColor, 1.0f);

        const std::string raidText = raidLabel(context.state.raid);
        if (!raidText.empty()) {
            context.resources.drawText(font, raidText, { 210, 355 }, 18, WHITE, 1.0f);
        }
    } else if (m_menuState == OverworldMenuState::Item || m_menuState == OverworldMenuState::ItemAction) {
        Rectangle itemBox = { 186, 52, 400, 300 };
        DrawRectangleRec(itemBox, BLACK);
        DrawRectangleLinesEx(itemBox, 5.0f, WHITE);

        if (context.player.inventory.empty()) {
            context.resources.drawText(font, "* (Inventory is empty)", { 210, 75 }, 20, WHITE, 1.0f);
        } else {
            for (size_t i = 0; i < context.player.inventory.size(); ++i) {
                float optY = 75.0f + (float)i * 28.0f;
                if (m_menuState == OverworldMenuState::Item && m_itemSubIndex == (int)i) {
                    drawSoul(210.0f, optY + 9.0f);
                }
                context.resources.drawText(font, "* " + context.player.inventory[i], { 228.0f, optY }, 20, WHITE, 1.0f);
            }

            if (m_menuState == OverworldMenuState::ItemAction) {
                const char* actionLabels[3] = { "USE", "INFO", "DROP" };
                const float actionX[3] = { 245.0f, 365.0f, 485.0f };
                for (int i = 0; i < 3; ++i) {
                    if (m_itemActionIndex == i) {
                        drawSoul(actionX[i] - 20.0f, 326.0f);
                    }
                    context.resources.drawText(font, actionLabels[i], { actionX[i], 317.0f }, 18, WHITE, 1.0f);
                }
            }
        }
    } else if (m_menuState == OverworldMenuState::Cell) {
        Rectangle cellBox = { 186, 52, 400, 300 };
        DrawRectangleRec(cellBox, BLACK);
        DrawRectangleLinesEx(cellBox, 5.0f, WHITE);

        const std::string raidText = raidLabel(context.state.raid);
        if (!raidText.empty()) {
            float raidW = context.resources.measureTextWidth(font, raidText, 18.0f, 1.0f);
            float raidX = cellBox.x + cellBox.width - 20.0f - raidW;
            context.resources.drawText(font, raidText, { raidX, 68.0f }, 18, WHITE, 1.0f);
        }

        if (m_cellSubState == CellSubState::Contacts) {
            auto entries = cellEntries([&context](const std::string& flag) {
                return context.state.hasFlag(flag);
            });
            for (size_t i = 0; i < entries.size(); ++i) {
                float optY = 100.0f + static_cast<float>(i) * 30.0f;
                if (m_cellIndex == static_cast<int>(i)) {
                    drawSoul(210.0f, optY + 9.0f);
                }
                context.resources.drawText(font, entries[i], { 228.0f, optY }, 20, WHITE, 1.0f);
            }
        } else if (m_cellSubState == CellSubState::FuriellMenu) {
            const auto& opts = furiellOptions();
            for (size_t i = 0; i < opts.size(); ++i) {
                float optY = 100.0f + static_cast<float>(i) * 32.0f;
                if (m_furiellIndex == static_cast<int>(i)) {
                    drawSoul(210.0f, optY + 9.0f);
                }
                context.resources.drawText(font, opts[i].first, { 228.0f, optY }, 20, WHITE, 1.0f);
            }
        }
    }
}

void OverworldScene::handleSaveBoxInput(GameContext& context) {
    if (m_saveBoxState == SaveBoxState::Active) {
        if (context.input.isLeftPressed() || context.input.isRightPressed()) {
            m_saveBoxChoice = (m_saveBoxChoice == 0) ? 1 : 0;
            context.audio.playSound("assets/sfx/menu_move.wav", 0.5f);
        }

        if (context.input.isConfirmPressed()) {
            if (m_saveBoxChoice == 0) { // Save
                context.state.playerPos = m_player.getPosition();
                context.save.saveGame(context.state);
                sessionOnSave(context.session);
                context.audio.playSound("assets/sfx/u_activate.wav", 0.9f);
                m_saveBoxState = SaveBoxState::Saved;
            } else { // Return
                m_saveBoxState = SaveBoxState::Closed;
                context.audio.playSound("assets/sfx/menu_select.wav", 0.4f);
            }
        } else if (context.input.isCancelPressed()) {
            m_saveBoxState = SaveBoxState::Closed;
            context.audio.playSound("assets/sfx/menu_select.wav", 0.4f);
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
    std::string roomName = roomDisplayName(context);
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
    context.raidPaused = (m_menuState != OverworldMenuState::Closed);
    m_controlsLockedRemaining = std::max(0.0f, m_controlsLockedRemaining - std::max(0.0f, dt));
    m_roomAnimationTime += dt;
    m_player.updateOverrideAnimation(dt);
    for (WaterBurst& burst : m_waterBursts) burst.update(dt);
    m_waterBursts.erase(
        std::remove_if(m_waterBursts.begin(), m_waterBursts.end(),
                       [](const WaterBurst& burst) { return burst.isDone(); }),
        m_waterBursts.end());
    refreshDynamicObstacles(context);

    // RAD-003 / BUG-40: r1_wake sets the squad message flag at the buzz; that is when the clock starts.
    {
        int shownMask = context.state.getFlag("raid_msgs_shown", 0);
        if (raidStartFromSquadMessage(context.state.raid, context.state.getFlag(std::string(RAID_FLAG_SQUAD_MSG), 0),
                                      getRaidStartSeconds(), shownMask)) {
            context.state.setFlag("raid_msgs_shown", shownMask);
            s_prevRaidTimeLeft = context.state.raid.timeLeft;
        }
    }

    // Ambient fluff particles (FUR-010)
    int targetFluff = fluffTargetCount(context.state.furry.value, context.state.getRoute() == GameState::RouteType::ToxicGamer);
    updateFluff(m_fluffParticles, dt, targetFluff, m_fluffRng);

    if (m_autoFadeIn) {
        m_cutsceneFadeElapsed += std::max(0.0f, dt);
        m_cutsceneFadeAlpha = fadeAlpha(
            m_cutsceneFadeStart, 0.0f, m_cutsceneFadeElapsed, 0.3f);
        if (m_cutsceneFadeAlpha == 0.0f) {
            m_autoFadeIn = false;
            m_cutsceneFadeActive = false;
        }
    }

    if (m_doorFadeState != DoorFadeState::None) {
        updateDoorTransition(context);
        return;
    }

    if (m_cutscene.running()) {
        class OverworldCutsceneHost final : public CutsceneHost {
        public:
            OverworldCutsceneHost(DialogBox& dialogBox, Player& player, GameContext& context,
                                  std::vector<TiledObject>& roomObjects,
                                  std::unordered_set<std::string>& hidden,
                                  std::unordered_map<std::string, std::string>& animations,
                                  std::vector<WaterBurst>& waterBursts,
                                  std::unordered_set<std::string>& logged,
                                  float& controlsLockedRemaining, float& fadeAlphaValue,
                                  float& fadeStart, float& fadeTarget, float& fadeElapsed,
                                  bool& fadeActive, bool& battleStarted, int& choice)
                : m_dialogBox(dialogBox), m_player(player), m_context(context),
                  m_roomObjects(roomObjects), m_hidden(hidden), m_animations(animations),
                  m_waterBursts(waterBursts), m_logged(logged),
                  m_controlsLockedRemaining(controlsLockedRemaining),
                  m_fadeAlpha(fadeAlphaValue), m_fadeStart(fadeStart),
                  m_fadeTarget(fadeTarget), m_fadeElapsed(fadeElapsed),
                  m_fadeActive(fadeActive), m_battleStarted(battleStarted), m_choice(choice) {}

            void dialog(const std::string& speaker, const std::vector<std::string>& lines) override {
                m_dialogBox.startDialog(lines, speaker);
            }

            bool dialogOpen() const override {
                return m_dialogBox.isActive();
            }

            void sound(const std::string& path, float volume) override {
                m_context.audio.playSound(path, volume);
            }

            void setFlag(const std::string& key, int value) override {
                m_context.state.setFlag(key, value);
            }

            void shake(float intensity, float seconds) override {
                m_context.shake(intensity, seconds);
            }

            void flash(int r, int g, int b, float seconds) override {
                m_context.flash({
                    static_cast<unsigned char>(r),
                    static_cast<unsigned char>(g),
                    static_cast<unsigned char>(b),
                    255
                }, seconds);
            }

            void startBattle(const std::string& encounter) override {
                auto battle = std::make_unique<BattleScene>(encounter);
                if (!battle->isValid()) {
                    std::clog << "Cutscene startBattle: encounter '" << encounter
                              << "' is not valid; battle not started.\n";
                    return;
                }
                const Vector2 playerPos = m_player.getPosition();
                m_context.scenes.pushScene(
                    std::make_unique<EncounterTransitionScene>(playerPos, std::move(battle)),
                    m_context);
                m_battleStarted = true;
            }

            void openShop(const std::string& shop) override {
                if (shop.empty()) return;
                m_context.scenes.pushScene(std::make_unique<ShopScene>(shop), m_context);
            }

            void setFur(int value) override {
                m_context.state.furry.value = std::clamp(value, 0, 100);
            }

            void healFull() override {
                m_context.player.hp = m_context.player.maxHp;
            }

            void facePlayer(const std::string& key) override {
                const Direction direction = roomDirection(key);
                m_player.setDirection(direction);
                m_context.state.playerFacing = static_cast<int>(direction);
            }

            void playerAnim(const std::string& path, const std::string& key, float seconds) override {
                m_player.setOverrideAnimation(path, key, seconds);
            }

            bool condition(const std::string& text) override {
                bool parsed = false;
                const bool result = evaluateRoomCondition(text, [this](const std::string& key) {
                    if (key == "fur") return m_context.state.furry.value;
                    if (key == "hp") return m_context.player.hp;
                    const int inv = evaluateInventoryCondition(key, m_context.player.inventory, [this](const std::string& name) {
                        const ItemDef* def = m_context.itemDatabase.findItemByName(name);
                        return def != nullptr ? def->id : "";
                    });
                    if (inv >= 0) return inv;
                    return m_context.state.getFlag(key, 0);
                }, &parsed);
                if (!parsed && m_logged.insert("cutscene:" + text).second) {
                    std::clog << "Invalid cutscene condition '" << text
                              << "'; treating it as true.\n";
                }
                return result;
            }

            void giveItem(const std::string& item, const std::vector<std::string>& lines,
                          const std::vector<std::string>& fullLines) override {
                if (m_context.itemDatabase.findItemByName(item) == nullptr) return;
                if (!m_context.player.canAddItem()) {
                    m_dialogBox.startDialog(fullLines.empty()
                        ? std::vector<std::string>{ "* You're carrying too much." } : fullLines, "");
                    return;
                }
                m_context.player.addItem(item);
                if (!lines.empty()) m_dialogBox.startDialog(lines, "");
            }

            void addFlag(const std::string& key, int value) override {
                if (!key.empty()) m_context.state.incFlag(key, value);
            }

            void addFur(int value) override {
                m_context.state.furry.value = std::clamp(m_context.state.furry.value + value, 0, 100);
            }

            void npcFace(const std::string& npc, const std::string& key) override {
                if (!npc.empty()) m_animations[npc] = "stand_" + key;
            }

            void npcAnim(const std::string& npc, const std::string& key, float) override {
                if (!npc.empty()) m_animations[npc] = key;
            }

            void setNpcVisible(const std::string& npc, bool visible) override {
                if (visible) m_hidden.erase(npc);
                else if (!npc.empty()) m_hidden.insert(npc);
            }

            bool moveNpc(const std::string& npc, float x, float y, float speed, float dt) override {
                for (TiledObject& object : m_roomObjects) {
                    const std::string type = tiledObjectType(object);
                    if (object.name != npc || (type != "npc" && type != "prop")) continue;
                    const Vector2 position = { object.x, object.y };
                    const Vector2 target = { x, y };
                    const Vector2 next = stepToward(position, target, speed, dt);
                    const float dx = next.x - position.x;
                    const float dy = next.y - position.y;
                    if (dx != 0.0f || dy != 0.0f) {
                        const std::string direction = std::abs(dx) > std::abs(dy)
                            ? (dx < 0.0f ? "left" : "right")
                            : (dy < 0.0f ? "up" : "down");
                        const std::string sprite = tiledStringProperty(object.properties, "sprite");
                        const SpriteSheetDef* sheet = sprite.empty()
                            ? nullptr : spriteSheetCache().load(sprite, spriteJsonPath(sprite));
                        const std::string walk = "walk_" + direction;
                        if (sheet != nullptr && sheet->anims.find(walk) != sheet->anims.end()) {
                            m_animations[npc] = walk;
                        } else if (sheet != nullptr && sheet->anims.find(direction) != sheet->anims.end()) {
                            m_animations[npc] = direction;
                        }
                    }
                    object.x = next.x;
                    object.y = next.y;
                    return next.x == target.x && next.y == target.y;
                }
                return true;
            }

            bool movePlayer(float x, float y, float speed, float dt) override {
                const bool arrived = m_player.moveToward({ x, y }, speed, dt);
                m_context.state.playerPos = m_player.getPosition();
                m_context.state.playerFacing = static_cast<int>(m_player.getDirection());
                return arrived;
            }

            void lockControls(float seconds) override {
                m_controlsLockedRemaining = std::max(
                    m_controlsLockedRemaining, std::max(0.0f, seconds));
            }

            bool fadeOut(float seconds, float dt) override {
                return fadeTo(1.0f, seconds, dt);
            }

            bool fadeIn(float seconds, float dt) override {
                return fadeTo(0.0f, seconds, dt);
            }

            // C1-014: the options are drawn by the dialog box ({choice:A|B} page, Left/Right + Z).
            void beginChoice(const std::string& speaker, const std::vector<std::string>& pages,
                             const std::vector<std::string>&) override {
                m_choice = -1;
                m_dialogBox.startDialog(pages, speaker);
                int& choice = m_choice;
                m_dialogBox.setOnChoice([&choice](int index, const std::string&) { choice = index; });
            }

            int choiceResult() override {
                return m_choice;
            }

            void recordKill(const std::string& enemyId) override {
                applyCutsceneKill(m_context.state, m_context.session, enemyId);
            }

            bool loadCutscene(const std::string& id, Cutscene& cutscene) override {
                return loadCutsceneFile(id, cutscene);
            }

            void waterBurst(const std::string& target) override {
                Vector2 position = m_player.getSpritePivot();
                if (!target.empty() && target != "player" && target != "grub") {
                    for (const TiledObject& object : m_roomObjects) {
                        if (object.name != target) continue;
                        const TiledRect bounds = object.bounds();
                        position = { bounds.x + bounds.width * 0.5f,
                                     bounds.y + bounds.height * 0.5f };
                        break;
                    }
                }
                m_waterBursts.emplace_back(
                    position.x, position.y, WaterBurstVariant::Overworld,
                    0x508172u + static_cast<std::uint32_t>(m_waterBursts.size()));
            }

        private:
            bool fadeTo(float target, float seconds, float dt) {
                if (!m_fadeActive || m_fadeTarget != target) {
                    m_fadeStart = m_fadeAlpha;
                    m_fadeTarget = target;
                    m_fadeElapsed = 0.0f;
                    m_fadeActive = true;
                }
                m_fadeElapsed += std::max(0.0f, dt);
                m_fadeAlpha = fadeAlpha(m_fadeStart, target, m_fadeElapsed, seconds);
                if (m_fadeAlpha != target) return false;
                m_fadeActive = false;
                return true;
            }

            DialogBox& m_dialogBox;
            Player& m_player;
            GameContext& m_context;
            std::vector<TiledObject>& m_roomObjects;
            std::unordered_set<std::string>& m_hidden;
            std::unordered_map<std::string, std::string>& m_animations;
            std::vector<WaterBurst>& m_waterBursts;
            std::unordered_set<std::string>& m_logged;
            float& m_controlsLockedRemaining;
            float& m_fadeAlpha;
            float& m_fadeStart;
            float& m_fadeTarget;
            float& m_fadeElapsed;
            bool& m_fadeActive;
            bool& m_battleStarted;
            int& m_choice;
        } host(m_dialogBox, m_player, context, m_roomObjects, m_hiddenRoomObjects,
               m_roomObjectAnimations, m_waterBursts, m_loggedConditions, m_controlsLockedRemaining,
               m_cutsceneFadeAlpha, m_cutsceneFadeStart, m_cutsceneFadeTarget,
               m_cutsceneFadeElapsed, m_cutsceneFadeActive, m_cutsceneBattleStarted, m_cutsceneChoice);

        m_cutscene.update(dt, host);
        if (m_cutsceneBattleStarted) {
            if (!m_pendingTriggerOnce.empty() && triggerOnceShouldBurn(true, true)) {
                context.state.setFlag(m_pendingTriggerOnce, 1);
            }
            m_pendingTriggerOnce.clear();
            m_cutsceneBattleStarted = false;
        } else if (!m_cutscene.running() && !m_pendingTriggerOnce.empty()) {
            std::clog << "Trigger cutscene ended without starting its battle; once-flag '"
                      << m_pendingTriggerOnce << "' stays unset.\n";
            m_pendingTriggerOnce.clear();
        }
        if (!m_cutscene.running() && m_cutsceneFadeAlpha > 0.0f) {
            m_autoFadeIn = true;
            m_cutsceneFadeStart = m_cutsceneFadeAlpha;
            m_cutsceneFadeElapsed = 0.0f;
            m_cutsceneFadeActive = true;
        }
        refreshDynamicObstacles(context);
        if (m_dialogBox.isActive()) {
            m_dialogBox.update(dt, context);
        }
        return;
    }

    // If dialog is active, dialog gets all input
    if (m_dialogBox.isActive()) {
        m_dialogBox.update(dt, context);
        return;
    }
    m_talkingObjectId = -1;  // OVW-007: the NPC goes back to its own facing

    if (m_controlsLockedRemaining > 0.0f) return;

    // Save Point Box update (SAV-002)
    if (m_saveBoxState != SaveBoxState::Closed) {
        handleSaveBoxInput(context);
        return;
    }

    // Overworld C Menu update (MNU-001)
    if (m_menuState != OverworldMenuState::Closed) {
        handleMenuInput(context);
        context.raidPaused = (m_menuState != OverworldMenuState::Closed);
        return;
    }

    // RAD-003: Guild phone messages at raid clock thresholds
    if (context.state.raid.started) {
        const float nowLeft = context.state.raid.timeLeft;
        if (s_prevRaidTimeLeft < 0.0f || nowLeft > s_prevRaidTimeLeft) {
            s_prevRaidTimeLeft = nowLeft;
        }

        const int mask = context.state.getFlag("raid_msgs_shown", 0);
        const int pendingIndex = raidPendingMessage(s_prevRaidTimeLeft, nowLeft, mask);
        if (pendingIndex >= 0) {
            context.state.setFlag("raid_msgs_shown", mask | (1 << pendingIndex));
            context.audio.playSound("assets/sfx/menu_select.wav", 0.7f);
            m_dialogBox.startDialog(raidMessageLines(pendingIndex, getRaidStartSeconds()), "");
            return;
        } else {
            s_prevRaidTimeLeft = nowLeft;
        }
    }

    if (IsKeyPressed(KEY_C) || IsKeyPressed(KEY_LEFT_CONTROL)) {
        m_menuState = OverworldMenuState::Main;
        m_menuIndex = 0;
        context.raidPaused = true;
        context.audio.playSound("assets/sfx/menu_select.wav", 0.5f);
        return;
    }

    if (m_savePoint) {
        m_savePoint->update(dt);
    }

    // Player movement
    const Vector2 playerPositionBeforeMove = m_player.getPosition();
    Vector2 inputDir = { 0, 0 };
    if (context.input.isLeft()) inputDir.x -= 1.0f;
    if (context.input.isRight()) inputDir.x += 1.0f;
    if (context.input.isUp()) inputDir.y -= 1.0f;
    if (context.input.isDown()) inputDir.y += 1.0f;

    m_player.update(dt, inputDir, m_obstacles);
    const Vector2 playerPositionAfterMove = m_player.getPosition();
    const bool playerWalked = playerPositionAfterMove.x != playerPositionBeforeMove.x ||
        playerPositionAfterMove.y != playerPositionBeforeMove.y;

    // Keep GameState synchronized (CORE-004)
    context.state.playerPos = m_player.getPosition();

    updateBumpCutscenes(dt, context);
    if (m_cutscene.running()) return;

    updateHazards(context);
    if (m_dialogBox.isActive()) return;

    updateDoorsAndTriggers(context);
    if (m_doorFadeState != DoorFadeState::None || m_cutscene.running()) return;

    // Interaction check
    if (context.input.isConfirmPressed()) {
        Rectangle interactBox = m_player.getInteractionBox();

        // Check SavePoint (SAV-002)
        if (m_savePoint && CheckCollisionRecs(interactBox, m_savePoint->getBounds())) {
            m_savePoint->interact(context);
            m_dialogBox.startDialog(
                currentSaveLines(context),
                "",
                [this]() {
                    m_saveBoxState = SaveBoxState::Active;
                    m_saveBoxChoice = 0;
                }
            );
            return;
        }

        for (const TiledObject& object : m_roomObjects) {
            if (!roomObjectVisible(object, context)) continue;
            const std::string type = tiledObjectType(object);
            if (type != "sign" && type != "examine" && type != "prop" &&
                type != "npc" && type != "itemPickup") {
                continue;
            }
            const TiledRect tiledBounds = object.bounds();
            const Rectangle bounds = {
                tiledBounds.x, tiledBounds.y, tiledBounds.width, tiledBounds.height
            };
            if (CheckCollisionRecs(interactBox, bounds)) {
                interactWithRoomObject(object, context);
                return;
            }
        }

        // Check other interactables
        for (auto& item : m_interactables) {
            if (CheckCollisionRecs(interactBox, item->getBounds())) {
                item->interact(context);
                return;
            }
        }
    }

    const std::string encounterCountFlag = "enc." + context.state.currentRoom + ".count";
    const std::string_view encounterArea = encounterAreaForRoom(context.state.currentRoom);
    const int areaKills = encounterArea.empty()
        ? 0
        : context.state.getFlag("kills." + std::string(encounterArea), 0);
    if (!encounterArea.empty() &&
        areaExhaustedNoticeDue(encounterArea, areaKills,
                               context.state.getFlag(areaExhaustedAnnouncedFlag(encounterArea), 0))) {
        context.state.setFlag(areaExhaustedAnnouncedFlag(encounterArea), 1);
        m_dialogBox.startDialog({ std::string(AREA_EXHAUSTED_NOTICE) });
        return;
    }
    if (!m_roomEncounters.empty() &&
        encounterAllowed(context.state.getFlag(encounterCountFlag, 0),
                         m_encounterBudget,
                         context.state.getFlag("toxic.track", 0),
                         areaKills,
                         encounterArea) &&
        advanceEncounterStep(m_encounterStepsRemaining, playerWalked)) {
        const std::string encounterId = pickWeightedEncounter(m_roomEncounters, m_encounterRng);
        auto battle = std::make_unique<BattleScene>(encounterId);
        if (battle->isValid()) {
            context.state.incFlag(encounterCountFlag);
            m_encounterStepsRemaining = randomEncounterStepCountdown(m_encounterRng);
            const Vector2 playerPos = m_player.getPosition();
            context.scenes.pushScene(
                std::make_unique<EncounterTransitionScene>(playerPos, std::move(battle)), context);
            return;
        }
        m_encounterStepsRemaining = randomEncounterStepCountdown(m_encounterRng);
    }

#ifdef NUNDERTALE_DEBUG
    // Quick Battle Trigger shortcut for testing (ENC-005, ENC-002)
    if (IsKeyPressed(KEY_B)) {
        Vector2 pPos = m_player.getPosition();
        context.scenes.pushScene(std::make_unique<EncounterTransitionScene>(pPos, std::make_unique<BattleScene>()), context);
    }
#endif
}

void OverworldScene::drawTileLayers(GameContext& context, bool above) {
    for (const TiledLayer& layer : m_tiledMap.layers) {
        if (layer.type != TiledLayerType::Tile) continue;
        const bool layerAbove = layer.name == "above" ||
            tiledBoolProperty(layer.properties, "above", false);
        if (layerAbove != above) continue;
        const int layerWidth = layer.width > 0 ? layer.width : m_tiledMap.width;
        if (layerWidth <= 0) continue;

        for (std::size_t index = 0; index < layer.data.size(); ++index) {
            const std::uint32_t gid = maskTiledGid(layer.data[index]);
            if (gid == 0) continue;
            const TiledTileset* selected = nullptr;
            for (const TiledTileset& tileset : m_tiledMap.tilesets) {
                if (gid >= tileset.firstGid &&
                    (selected == nullptr || tileset.firstGid > selected->firstGid)) {
                    selected = &tileset;
                }
            }
            if (selected == nullptr || selected->image.empty()) continue;
            const TiledSourceRect source = gidToSourceRect(
                gid, selected->firstGid, selected->tileWidth, selected->tileHeight, selected->columns);
            if (!source.valid) continue;
            Texture2D texture = context.resources.getTexture(selected->image);
            SetTextureFilter(texture, TEXTURE_FILTER_POINT);
            const int column = static_cast<int>(index % static_cast<std::size_t>(layerWidth));
            const int row = static_cast<int>(index / static_cast<std::size_t>(layerWidth));
            const Rectangle sourceRect = {
                static_cast<float>(source.x), static_cast<float>(source.y),
                static_cast<float>(source.width), static_cast<float>(source.height)
            };
            const Rectangle destination = {
                static_cast<float>(column * m_tiledMap.tileWidth),
                static_cast<float>(row * m_tiledMap.tileHeight),
                static_cast<float>(m_tiledMap.tileWidth),
                static_cast<float>(m_tiledMap.tileHeight)
            };
            DrawTexturePro(texture, sourceRect, destination, { 0.0f, 0.0f }, 0.0f, WHITE);
        }
    }
}

void OverworldScene::drawRoomObject(const TiledObject& object, GameContext& context) {
    const std::string type = tiledObjectType(object);
    const TiledRect tiledBounds = object.bounds();
    const Rectangle bounds = { tiledBounds.x, tiledBounds.y, tiledBounds.width, tiledBounds.height };
    const std::string sprite = tiledStringProperty(object.properties, "sprite");
    if (!sprite.empty()) {
        const SpriteSheetDef* sheet = spriteSheetCache().load(sprite, spriteJsonPath(sprite));
        Texture2D texture = context.resources.getTexture(sprite);
        SetTextureFilter(texture, TEXTURE_FILTER_POINT);
        const Vector2 pivot = { bounds.x + bounds.width * 0.5f, bounds.y + bounds.height };
        if (sheet != nullptr) {
            std::string animation = tiledStringProperty(object.properties, "anim");
            const std::string baseAnimation = animation;
            const auto overrideIt = m_roomObjectAnimations.find(object.name);
            if (overrideIt != m_roomObjectAnimations.end() &&
                sheet->anims.find(overrideIt->second) != sheet->anims.end()) {
                animation = overrideIt->second;
            }
            const std::string altAnim = tiledStringProperty(object.properties, "altAnim");
            const std::string altAnimIf = tiledStringProperty(object.properties, "altAnimIf");
            bool altParsed = true;
            animation = selectRoomAnimation(animation, altAnim, altAnimIf,
                [&context](const std::string& key) {
                    if (key == "fur") return context.state.furry.value;
                    if (key == "hp") return context.player.hp;
                    return context.state.getFlag(key, 0);
                }, &altParsed);
            if (!altParsed) {
                const std::string logKey = std::to_string(object.id) + ":altAnimIf:" + altAnimIf;
                if (m_loggedConditions.insert(logKey).second) {
                    std::clog << "Invalid room condition '" << altAnimIf
                              << "'; treating it as true.\n";
                }
            }
            if (!animation.empty() && sheet->anims.find(animation) == sheet->anims.end()) {
                animation = baseAnimation;
            }
            if (animation.empty() && type == "npc" && sheet->anims.find("stand_down") != sheet->anims.end()) {
                animation = "stand_down";
            }
            if (animation.empty() && !sheet->anims.empty()) animation = sheet->anims.begin()->first;
            if (object.id == m_talkingObjectId) {
                const std::string turned = "stand_" + m_talkingFacing;
                if (sheet->anims.find(turned) != sheet->anims.end()) animation = turned;
            }
            drawSprite(texture, *sheet, animation, m_roomAnimationTime, pivot);
        } else {
            const Rectangle source = {
                0.0f, 0.0f, static_cast<float>(texture.width), static_cast<float>(texture.height)
            };
            const Rectangle destination = {
                pivot.x - static_cast<float>(texture.width),
                pivot.y - static_cast<float>(texture.height) * 2.0f,
                static_cast<float>(texture.width) * 2.0f,
                static_cast<float>(texture.height) * 2.0f
            };
            DrawTexturePro(texture, source, destination, { 0.0f, 0.0f }, 0.0f, WHITE);
        }
        return;
    }

    // Like Undertale, signs, examinables and hidden pickups have no marker (F2 hitboxes show them).
}

void OverworldScene::drawPlayer(GameContext& context) {
    if (m_player.hasOverrideAnimation()) {
        const std::string& path = m_player.getOverridePath();
        const SpriteSheetDef* sheet = spriteSheetCache().load(path, spriteJsonPath(path));
        if (sheet != nullptr) {
            Texture2D texture = context.resources.getTexture(path);
            SetTextureFilter(texture, TEXTURE_FILTER_POINT);
            std::string animation = m_player.getOverrideKey();
            if (animation.empty() && !sheet->anims.empty()) animation = sheet->anims.begin()->first;
            drawSprite(texture, *sheet, animation, m_player.getOverrideAnimationTime(), m_player.getSpritePivot());
            return;
        }
    }

    const SpriteSheetDef* grubSheet = spriteSheetCache().load(
        "assets/sprites/ow/grub.png", "assets/sprites/ow/grub.json");
    if (grubSheet != nullptr) {
        std::string direction;
        switch (m_player.getDirection()) {
            case Direction::Up: direction = "up"; break;
            case Direction::Down: direction = "down"; break;
            case Direction::Left: direction = "left"; break;
            case Direction::Right: direction = "right"; break;
        }
        const std::string animation = (m_player.isMoving() ? "walk_" : "stand_") + direction;
        Texture2D grubTexture = context.resources.getTexture("assets/sprites/ow/grub.png");
        SetTextureFilter(grubTexture, TEXTURE_FILTER_POINT);
        drawSprite(grubTexture, *grubSheet, animation,
                   m_player.getSpriteAnimationTime(), m_player.getSpritePivot());
    } else {
        Texture2D pepeTexture = context.resources.getMaskedTexture(
            "assets/img/pepe_char.bmp", { 255, 0, 255, 255 });
        m_player.render(pepeTexture);
    }
}

void OverworldScene::render(GameContext& context) {
    ClearBackground(BLACK);

    Font font8Bit = context.resources.getFont8Bit(20);
    const Vector2 playerPosition = m_player.getPosition();
    const CameraTopLeft cameraTopLeft = Camera2DFollow::calculate(
        playerPosition.x, playerPosition.y, m_roomWidth, m_roomHeight);
    Camera2D camera = {};
    camera.target = { cameraTopLeft.x, cameraTopLeft.y };
    camera.offset = { 0.0f, 0.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    BeginMode2D(camera);

    // 1. Draw Background Room Texture
    if (!m_backgroundImage.empty()) {
        Texture2D bgTex = context.resources.getTexture(m_backgroundImage);
        SetTextureFilter(bgTex, TEXTURE_FILTER_POINT);
        DrawTexture(bgTex, static_cast<int>(m_backgroundOffset.x),
                    static_cast<int>(m_backgroundOffset.y), WHITE);
    }
    drawTileLayers(context, false);

    // C1-004: a steady (never animated/flashed) wash mixes the currently lit RGB keys into the room.
    if (context.state.currentRoom == "ruins_switches") {
        const RgbKeyboardColor tint = rgbKeyboardTint(rgbKeyboardMaskFromState(context.state));
        DrawRectangle(0, 0, static_cast<int>(m_roomWidth), static_cast<int>(m_roomHeight),
                      Color{ static_cast<unsigned char>(tint.r), static_cast<unsigned char>(tint.g),
                             static_cast<unsigned char>(tint.b), 42 });
    }

    // 2. Render Interactables & SavePoint
    for (auto& item : m_interactables) {
        item->render(context);
    }
    if (m_savePoint) {
        m_savePoint->render(context);
    }

    for (const TiledObject& object : m_roomObjects) {
        if (!roomObjectVisible(object, context)) continue;
        const std::string type = tiledObjectType(object);
        if (type != "prop" && type != "npc") drawRoomObject(object, context);
    }

    struct SortedDraw {
        float y = 0.0f;
        const TiledObject* object = nullptr;
    };
    std::vector<SortedDraw> sorted;
    for (const TiledObject& object : m_roomObjects) {
        if (!roomObjectVisible(object, context)) continue;
        const std::string type = tiledObjectType(object);
        if (type == "prop" || type == "npc") {
            const TiledRect bounds = object.bounds();
            sorted.push_back({ bounds.y + bounds.height, &object });
        }
    }
    sorted.push_back({ m_player.getCollisionBox().y + m_player.getCollisionBox().height, nullptr });
    std::stable_sort(sorted.begin(), sorted.end(), [](const SortedDraw& left, const SortedDraw& right) {
        return left.y < right.y;
    });
    for (const SortedDraw& entry : sorted) {
        if (entry.object == nullptr) drawPlayer(context);
        else drawRoomObject(*entry.object, context);
    }

    for (const WaterBurst& burst : m_waterBursts) {
        if (burst.activeAnimFrame() >= 0) {
            const std::string spritePath(burst.spriteSheetPath());
            const SpriteSheetDef* sheet = spriteSheetCache().load(
                spritePath, std::string(burst.spriteSheetJson()));
            if (sheet != nullptr) {
                Texture2D texture = context.resources.getTexture(spritePath);
                SetTextureFilter(texture, TEXTURE_FILTER_POINT);
                drawSprite(texture, *sheet, std::string(burst.animName()), burst.getElapsed(),
                           { burst.getOriginX(), burst.getOriginY() });
            }
        }
        for (const WaterDroplet& droplet : burst.getDroplets()) {
            if (!droplet.alive()) continue;
            const float alpha = droplet.alpha();
            DrawRectangle(
                static_cast<int>(droplet.x), static_cast<int>(droplet.y),
                std::max(1, static_cast<int>(droplet.size)),
                std::max(1, static_cast<int>(droplet.size)),
                Color{ droplet.color.r, droplet.color.g, droplet.color.b,
                       static_cast<unsigned char>(static_cast<float>(droplet.color.a) * alpha) });
        }
    }

    drawTileLayers(context, true);

    // 5. Hitbox Debug Overlay (QA-002)
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

    EndMode2D();

    // Ambient fluff particles (FUR-010)
    for (const auto& p : m_fluffParticles) {
        Color fluffCol = (p.colorIndex == 0) ? Color{ 255, 179, 230, 180 } : Color{ 255, 210, 127, 180 };
        float drawX = fluffDrawX(p);
        DrawRectangle(static_cast<int>(drawX), static_cast<int>(p.y), 2, 2, fluffCol);
    }

    // OVW-001: Bottom HUD bar removed! Exploration is clean 1:1 like Undertale.
    // Overworld C Menu (MNU-001)
    renderMenu(context, font8Bit);

    // Save Point Box (SAV-002)
    renderSaveBox(context, font8Bit);

    // 6. Render DialogBox if open
    if (m_cutsceneFadeAlpha > 0.0f) {
        DrawRectangle(0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT,
                      Color{ 0, 0, 0, static_cast<unsigned char>(
                          std::clamp(m_cutsceneFadeAlpha, 0.0f, 1.0f) * 255.0f) });
    }
    m_dialogBox.render(context);

    if (m_doorFadeState != DoorFadeState::None) {
        const float progress = std::clamp(static_cast<float>(m_doorFadeFrame) / 8.0f, 0.0f, 1.0f);
        DrawRectangle(0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT,
                      Color{ 0, 0, 0, static_cast<unsigned char>(progress * 255.0f) });
    }
}

void OverworldScene::onExit(GameContext& context) {
    context.raidPaused = false;
}

} // namespace nundertale
