#include "scenes/BattleScene.hpp"
#include "scenes/OverworldScene.hpp"
#include "scenes/GameOverScene.hpp"
#include "core/GameContext.hpp"
#include "core/Constants.hpp"
#include "core/SceneManager.hpp"
#include "services/MusicTracks.hpp"
#include "core/TiledMap.hpp"
#include "core/Fur.hpp"
#include "ui/SpriteDraw.hpp"
#include "combat/Corruption.hpp"
#include "combat/KillScaling.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <string>
#include <vector>
#include <raylib.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cctype>

namespace nundertale {

namespace {

const void* s_lastScaledBattleSystem = nullptr;

void applyBattleKillScaling(BattleSystem* battleSystem, int ruinsKills) {
    if (!battleSystem || ruinsKills <= 0) return;
    if (s_lastScaledBattleSystem == battleSystem) return;

    for (std::size_t i = 0; i < battleSystem->enemyCount(); ++i) {
        Enemy* enemy = battleSystem->enemyAt(i);
        if (enemy == nullptr) continue;
        KillScalingDef scaling;
        if (!loadKillScalingFromFile("data/enemies/" + enemy->getId() + ".json", scaling) ||
            !scaling.present) continue;
        applyKillScaling(*enemy, scaling, ruinsKills);
    }
    s_lastScaledBattleSystem = battleSystem;
}

} // namespace

static bool safeEncounterId(const std::string& id) {
    if (id.empty()) return false;
    for (char ch : id) {
        if (std::isalnum(static_cast<unsigned char>(ch)) == 0 && ch != '_') return false;
    }
    return true;
}

BattleScene::BattleScene(std::string encounterId)
    : m_encounterId(std::move(encounterId)) {
    m_valid = loadEncounter(m_encounterId, true);
}

bool BattleScene::loadEncounter(const std::string& encounterId, bool readIntro) {
    if (!safeEncounterId(encounterId)) {
        std::clog << "Invalid encounter id '" << encounterId << "'; battle not started.\n";
        return false;
    }

    std::ifstream encFile("data/encounters/" + encounterId + ".json");
    if (!encFile) {
        std::clog << "Unknown encounter id '" << encounterId << "'; battle not started.\n";
        return false;
    }

    std::stringstream ss;
    ss << encFile.rdbuf();
    const std::string encounterJson = ss.str();
    tiled_detail::JsonValue root;
    Encounter encounter;
    if (!tiled_detail::JsonParser(encounterJson).parse(root) ||
        !parseEncounter(encounterJson, encounter) ||
        root.kind != tiled_detail::JsonValue::Kind::Object) {
        std::clog << "Invalid encounter data for '" << encounterId << "'; battle not started.\n";
        return false;
    }

    std::vector<std::unique_ptr<Enemy>> loadedEnemies;
    std::vector<std::string> enemyNames;
    loadedEnemies.reserve(encounter.enemies.size());
    enemyNames.reserve(encounter.enemies.size());
    for (const std::string& enemyId : encounter.enemies) {
        if (!safeEncounterId(enemyId) || !std::filesystem::exists("data/enemies/" + enemyId + ".json")) {
            std::clog << "Encounter '" << encounterId << "' references unknown enemy '"
                      << enemyId << "'; battle not started.\n";
            return false;
        }
        auto enemy = Enemy::loadFromJson("data/enemies/" + enemyId + ".json");
        if (!enemy) return false;
        enemyNames.push_back(enemy->getName());
        loadedEnemies.push_back(std::move(enemy));
    }

    const std::string introText = encounterIntroText(encounter, enemyNames);
    std::string backgroundId = encounter.background.empty() ? m_backgroundId : encounter.background;
    std::string musicId = encounter.music.empty() ? m_musicId : encounter.music;
    std::string fleeBlockedText;
    if (const auto* value = root.field("fleeBlockedText"); value != nullptr &&
        value->kind == tiled_detail::JsonValue::Kind::String) fleeBlockedText = value->string;
    int furCap = -1;
    if (const auto* value = root.field("furCap"); value != nullptr &&
        value->kind == tiled_detail::JsonValue::Kind::Number) furCap = static_cast<int>(value->number);

    m_enemySprite = loadedEnemies.front()->getSprite();
    m_enemyId = loadedEnemies.front()->getId();
    m_enemyName = loadedEnemies.front()->getName();
    m_battleSystem = std::make_unique<BattleSystem>(
        std::move(loadedEnemies), std::move(encounter), introText, std::move(fleeBlockedText), furCap);
    m_backgroundId = std::move(backgroundId);
    m_musicId = std::move(musicId);
    if (!m_backgroundId.empty()) m_battleSystem->setBackground(backgroundFromId(m_backgroundId));

    // C1-002: the optional scripted intro (data/encounters/<id>.json "intro")
    if (readIntro) {
        if (const auto* intro = root.field("intro"); intro != nullptr) {
            if (!parseIntroScript(*intro, m_introDef)) {
                std::clog << "Encounter '" << encounterId << "' has a malformed intro block; intro skipped.\n";
            }
        }
    }
    return true;
}

void BattleScene::swapEncounter(const std::string& encounterId) {
    if (encounterId == m_encounterId) return;
    if (loadEncounter(encounterId, false)) {
        m_encounterId = encounterId;
    } else {
        std::clog << "Could not swap the battle to '" << encounterId << "'; keeping '" << m_encounterId << "'.\n";
    }
}

void BattleScene::playEncounterMusic(GameContext& context) {
    const std::string musicContext = m_musicId.rfind("battle.", 0) == 0
        ? m_musicId
        : "battle." + m_musicId;
    const MusicEntry track = musicFor(
        musicContext,
        [](const std::string& path) { return std::filesystem::exists(path); });
    context.audio.playMusic(track.path, true, track.loopStart, track.loopEnd);
}

void BattleScene::onEnter(GameContext& context) {
    if (!m_valid || !m_battleSystem) return;
    if (!m_introChecked) {
        m_introChecked = true;
        if (introShouldRun(m_introDef, context.state.flags)) {
            beginIntro(context);
            return;
        }
        // A retry skips the intro and goes straight to the locked fight variant.
        if (!m_introDef.empty()) {
            const int locked = context.state.getFlag("ruins.floofy.fight", 0);
            if (locked != 0) swapEncounter(introFightEncounter(locked));
        }
    }
    const int ruinsKills = context.state.getFlag("kills.ruins", 0);
    applyBattleKillScaling(m_battleSystem.get(), ruinsKills);
    playEncounterMusic(context);
}

void BattleScene::update(float dt, GameContext& context) {
    if (m_introActive) {
        updateIntro(dt, context);
        return;
    }
    if (m_battleSystem) {
        if (m_bossYiffActive) {
            m_bossYiffTimer += dt;
            if (!m_bossYiffDismissing && m_bossYiffTimer >= 0.35f && context.input.isConfirmPressed()) {
                m_bossYiffDismissing = true;
                m_bossYiffTimer = 0.0f;
            } else if (m_bossYiffDismissing && m_bossYiffTimer >= 0.30f) {
                m_bossYiffActive = false;
                m_bossYiffDismissing = false;
                m_bossYiffTimer = 0.0f;
            }
            return;
        }

        const FightPhase phaseBefore = m_battleSystem->getFightPhase();
        const int killsBefore = toxicKills(context.state);
        const int expBefore = context.player.exp;
        const int trackBefore = toxicTrack(context.state);
        const bool sparedBefore = m_battleSystem->isSpared();
        const int hpBefore = context.player.hp;
        m_battleSystem->update(dt, context);
        updateToxicFx(dt, context, phaseBefore, killsBefore, expBefore);
        updateMercyTransition(dt, context, trackBefore, sparedBefore);

        if (context.player.hp < hpBefore) {
            const bool defeated = context.player.hp <= 0 ||
                m_battleSystem->getState() == BattleState::Defeat;
            context.audio.playSound(
                defeated ? "assets/sfx/grunt_death.ogg" : "assets/sfx/grunt_hurt.ogg",
                defeated ? 0.9f : 0.65f);
        }

        // BND-010 & BUG-03: Transition directly to authentic GameOverScene
        if (m_battleSystem->getState() == BattleState::Defeat) {
            Vector2 deathPos = m_battleSystem->getSoulPosition();
            context.scenes.changeScene(std::make_unique<GameOverScene>(deathPos, m_encounterId), 0.0f);
            return;
        }

        // AUD-004: the battle music stops the moment YOU WON shows
        if (m_battleSystem->isVictory() && !m_victoryMusicStopped) {
            context.audio.stopMusic();
            m_victoryMusicStopped = true;
        }

        if (m_battleSystem->isFinished()) {
            if (m_battleSystem->isFled()) {
                context.scenes.popScene(context);
                return;
            }
            if (context.input.isConfirmPressed()) {
                context.scenes.popScene(context);
            }
        }
    }
}

void BattleScene::render(GameContext& context) {
    ClearBackground(BLACK);

    if (m_introActive) {
        renderIntro(context);
        return;
    }

    if (m_battleSystem) {
        m_battleSystem->render(context);
        renderToxicBackdrop(context);

        if (m_battleSystem->isFinished() && !m_battleSystem->isFled() && !m_bossYiffActive) {
            Font font8Bit = context.resources.getFont8Bit(20);
            context.resources.drawCenteredText(font8Bit, "[Press Z or ENTER to continue]", 364, 20, COLOR_UNDERTALE_YELLOW, 1.0f);
        }
        renderToxicOverlay(context);
        renderMercyGlitch(context);
        renderBossYiff(context);
    }
}

namespace {

constexpr float MERCY_GLITCH_SECONDS = 0.48f;
constexpr float BOSS_YIFF_FADE_IN = 0.35f;
constexpr float BOSS_YIFF_FADE_OUT = 0.30f;

bool hasBossYiffEnding(const std::string& enemyId) {
    return enemyId == "barky" || enemyId == "furiell";
}

Color mercyFxColor(Color color, float alpha) {
    color.a = static_cast<unsigned char>(std::clamp(alpha, 0.0f, 1.0f) * 255.0f);
    return color;
}

unsigned int mercyFxNoise(unsigned int seed) {
    seed ^= seed << 13U;
    seed ^= seed >> 17U;
    seed ^= seed << 5U;
    return seed;
}

} // namespace

void BattleScene::updateMercyTransition(float dt, GameContext& context, int trackBefore, bool sparedBefore) {
    if (m_mercyGlitchTimer > 0.0f) m_mercyGlitchTimer = std::max(0.0f, m_mercyGlitchTimer - dt);

    const int trackAfter = toxicTrack(context.state);
    if ((trackBefore != 1 && trackAfter == 1) || (trackBefore == 1 && trackAfter != 1)) {
        m_mercyGlitchTimer = MERCY_GLITCH_SECONDS;
    }

    if (context.state.getFlag("debug.mercy_glitch", 0) == 1 && m_mercyGlitchTimer <= 0.0f) {
        context.state.setFlag("debug.mercy_glitch", 0);
        m_mercyGlitchTimer = MERCY_GLITCH_SECONDS;
    }

    if (context.state.getFlag("debug.boss_yiff", 0) == 1 && !m_bossYiffActive && m_bossYiffTimer == 0.0f) {
        context.state.setFlag("debug.boss_yiff", 0);
        m_bossYiffText = m_enemyId == "barky"
            ? "* The narrator refuses to describe this."
            : "* The narrator refuses to describe this.\n* (You don't want to talk about it.)";
        m_bossYiffTimer = BOSS_YIFF_FADE_IN;
        m_bossYiffDismissing = false;
        m_bossYiffActive = true;
    }

    const bool yiffedBoss = !sparedBefore && m_battleSystem->isSpared() &&
                            trackBefore != 1 && hasBossYiffEnding(m_enemyId);
    if (!yiffedBoss) return;

    applyFurCapped(context.state.furry.value, context.state.furry.stageMax, 15, false);
    m_bossYiffText = m_enemyId == "barky"
        ? "* The narrator refuses to describe this."
        : "* The narrator refuses to describe this.\n* (You don't want to talk about it.)";
    m_bossYiffTimer = 0.0f;
    m_bossYiffDismissing = false;
    m_bossYiffActive = true;
}

void BattleScene::renderMercyGlitch(GameContext& context) const {
    if (m_mercyGlitchTimer <= 0.0f) return;

    const float progress = 1.0f - m_mercyGlitchTimer / MERCY_GLITCH_SECONDS;
    const float strength = std::sin(std::clamp(progress, 0.0f, 1.0f) * 3.14159265f);
    const Rectangle labelArea{
        static_cast<float>(BATTLE_BUTTON_X_MERCY + 34),
        static_cast<float>(BATTLE_BUTTON_Y + 5),
        static_cast<float>(BATTLE_BUTTON_W - 39),
        static_cast<float>(BATTLE_BUTTON_H - 10)
    };
    DrawRectangleRec(labelArea, BLACK);

    Font font = context.resources.getFont8Bit(18);
    const bool toSpare = (toxicTrack(context.state) == 1);
    const bool targetFrame = (static_cast<int>(progress * 24.0f) % 2) == 0 || progress > 0.72f;
    const bool isSpare = toSpare ? targetFrame : !targetFrame;
    const std::string label = isSpare ? "SPARE" : "YIFF";
    const float textW = context.resources.measureTextWidth(font, label, 18, 1.0f);
    const float jitter = (isSpare ? 1.0f : -1.0f) * std::ceil(strength * 2.0f);
    const Color color = isSpare ? UI_ORANGE : Color{ 255, 119, 200, 255 };
    context.resources.drawText(font, label,
        { labelArea.x + (labelArea.width - textW) * 0.5f + jitter, labelArea.y + 5.0f },
        18, color, 1.0f);

    const unsigned int frame = static_cast<unsigned int>(progress * 1000.0f);
    for (unsigned int i = 0; i < 4U; ++i) {
        const unsigned int noise = mercyFxNoise(frame + i * 41U);
        const float y = labelArea.y + static_cast<float>(noise % 22U);
        const float x = labelArea.x + static_cast<float>((noise >> 5U) % 18U) - 5.0f;
        const float width = 18.0f + static_cast<float>((noise >> 11U) % 35U);
        DrawRectangleRec({ x, y, width, 1.0f + static_cast<float>(i % 2U) },
                         mercyFxColor(i % 2U == 0U ? color : WHITE, 0.35f + strength * 0.45f));
    }
}

void BattleScene::renderBossYiff(GameContext& context) const {
    if (!m_bossYiffActive) return;

    const float alpha = m_bossYiffDismissing
        ? 1.0f - std::clamp(m_bossYiffTimer / BOSS_YIFF_FADE_OUT, 0.0f, 1.0f)
        : std::clamp(m_bossYiffTimer / BOSS_YIFF_FADE_IN, 0.0f, 1.0f);
    DrawRectangle(0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT, mercyFxColor(BLACK, alpha));
    if (m_bossYiffTimer < BOSS_YIFF_FADE_IN && !m_bossYiffDismissing) return;

    Font font = context.resources.getFont8Bit(20);
    std::stringstream ss(m_bossYiffText);
    std::string line;
    float lineY = (m_bossYiffText.find('\n') != std::string::npos) ? 200.0f : 220.0f;
    while (std::getline(ss, line)) {
        if (!line.empty()) {
            context.resources.drawCenteredText(font, line, lineY, 20, mercyFxColor(WHITE, alpha), 1.0f);
            lineY += 32.0f;
        }
    }
    context.resources.drawCenteredText(font, "[Press Z or ENTER]", 360, 16,
                                       mercyFxColor(COLOR_UNDERTALE_YELLOW, alpha), 1.0f);
}

// ---------------------------------------------------------------- C1-002: the in-box intro (LORE §19.0 beats 8-18)

namespace {

constexpr float INTRO_TYPE_CPS = 42.0f;           // typewriter speed, characters per second
constexpr float INTRO_SOUL_SPEED = 120.0f;        // same as the battle (BHL-006)
constexpr float INTRO_SOUL_PADDING = 12.0f;
constexpr float INTRO_PETAL_INTERVAL = 0.85f;     // seconds between petals of a round
constexpr float INTRO_PETAL_HIT_RADIUS = 9.0f;
constexpr float INTRO_FORCED_RAMP = 55.0f;        // px/s gained per second by the unavoidable petals
constexpr float INTRO_PETAL_Y_WOBBLE = 1.5f;
constexpr Rectangle INTRO_SMALL_BOX{ 237.0f, 250.0f, 165.0f, 140.0f };   // the dodge box (BHL-001)
constexpr Rectangle INTRO_BIG_BOX{ static_cast<float>(BATTLE_BOX_X), static_cast<float>(BATTLE_BOX_Y),
                                   static_cast<float>(BATTLE_BOX_W), static_cast<float>(BATTLE_BOX_H) };

// Wraps on spaces; the first line may have a different width (hanging indent after "* ").
std::vector<std::string> wrapIntroText(GameContext& context, Font font, const std::string& text, float size,
                                       float firstWidth, float restWidth) {
    std::vector<std::string> lines;
    std::stringstream paragraphs(text);
    std::string paragraph;
    while (std::getline(paragraphs, paragraph, '\n')) {
        std::stringstream words(paragraph);
        std::string word;
        std::string current;
        bool first = true;
        while (words >> word) {
            const std::string test = current.empty() ? word : current + " " + word;
            const float limit = first ? firstWidth : restWidth;
            if (!current.empty() && context.resources.measureTextWidth(font, test, size, 1.0f) > limit) {
                lines.push_back(current);
                current = word;
                first = false;
            } else {
                current = test;
            }
        }
        lines.push_back(current);
    }
    return lines;
}

// Characters of `lines` revealed by a typewriter that has typed `shown` characters (a line break counts as one).
std::size_t introLineChars(const std::string& line, std::size_t& left) {
    const std::size_t take = std::min(left, line.size());
    left = left > line.size() + 1U ? left - (line.size() + 1U) : 0U;
    return take;
}

void drawIntroSoul(GameContext& context, float x, float y, Color color) {
    const SpriteSheetDef* soulSheet = spriteSheetCache().load("assets/ui/souls.png", "assets/ui/souls.json");
    if (soulSheet != nullptr) {
        Texture2D soulTex = context.resources.getTexture("assets/ui/souls.png");
        if (soulTex.id > 0) {
            SetTextureFilter(soulTex, TEXTURE_FILTER_POINT);
            const std::string anim = context.player.name == "GRUB" ? "headset" : "red";
            drawSprite(soulTex, *soulSheet, anim, 0.0f, { x, y + 1.0f }, 2.0f, WHITE);
            return;
        }
    }
    DrawCircle(static_cast<int>(x) - 4, static_cast<int>(y) - 2, 5, color);
    DrawCircle(static_cast<int>(x) + 4, static_cast<int>(y) - 2, 5, color);
    DrawTriangle({ x - 9.0f, y }, { x + 9.0f, y }, { x, y + 9.0f }, color);
}

void drawIntroPetal(GameContext& context, Vector2 pos, float age, Color tint) {
    const std::string png = "assets/bullets/petal.png";
    const SpriteSheetDef* sheet = spriteSheetCache().load(png, "assets/bullets/petal.json");
    if (sheet != nullptr) {
        Texture2D tex = context.resources.getTexture(png);
        if (tex.id > 0) {
            SetTextureFilter(tex, TEXTURE_FILTER_POINT);
            drawSprite(tex, *sheet, "spin", age, pos, 2.0f, tint);
            return;
        }
    }
    DrawCircleV(pos, 5.0f, tint);
}

bool introSfxExists(const std::string& id) {
    return std::filesystem::exists("assets/sfx/" + id + ".ogg");
}

} // namespace

void BattleScene::beginIntro(GameContext& context) {
    m_intro = std::make_unique<BattleIntroScript>(m_introDef);
    m_intro->start(context.state.flags);
    m_introActive = true;
    m_introTime = 0.0f;
    m_introTyped = 0.0f;
    m_introLastPhase = IntroPhase::Pause;
    m_introBox = INTRO_SMALL_BOX;
    m_introSoul = { INTRO_SMALL_BOX.x + INTRO_SMALL_BOX.width * 0.5f, INTRO_SMALL_BOX.y + INTRO_SMALL_BOX.height * 0.5f };
    m_introPetalsShown = false;
    m_introWilted = false;
    m_introFlash = 0.0f;
    m_introRoundActive = false;
    m_introPetals.clear();

    // The cute lounge music (falls back to the default battle track until the asset exists).
    const std::string musicId = m_introDef.music.empty() ? m_musicId : m_introDef.music;
    const std::string musicContext = musicId.rfind("battle.", 0) == 0 ? musicId : "battle." + musicId;
    const MusicEntry track = musicFor(
        musicContext,
        [](const std::string& path) { return std::filesystem::exists(path); });
    context.audio.playMusic(track.path, true, track.loopStart, track.loopEnd);
    playIntroEffects(context);
}

void BattleScene::endIntro(GameContext& context) {
    const int variant = m_intro ? m_intro->fightVariant() : context.state.getFlag("ruins.floofy.fight", 0);
    m_introActive = false;
    m_intro.reset();
    m_introPetals.clear();
    if (m_introFurTremble.running()) {
        // the battle proper must not tremble the bar a second time
        context.state.setFlag("fur_hud_seen", 1);
        m_introFurTremble = FurHudTrembleClock{};
    }
    if (variant != 0) swapEncounter(introFightEncounter(variant));
    const int ruinsKills = context.state.getFlag("kills.ruins", 0);
    applyBattleKillScaling(m_battleSystem.get(), ruinsKills);
    playEncounterMusic(context);
}

void BattleScene::playIntroEffects(GameContext& context) {
    if (!m_intro) return;
    for (const IntroEffect& effect : m_intro->takeEffects()) {
        switch (effect.kind) {
        case IntroEffectKind::StopMusic:
            context.audio.stopMusic();
            break;
        case IntroEffectKind::Sound:
            if (introSfxExists(effect.id)) context.audio.playSound("assets/sfx/" + effect.id + ".ogg", 0.9f);
            break;
        case IntroEffectKind::ShowPetals:
            m_introPetalsShown = true;
            if (introSfxExists("petal_buzz")) context.audio.playSound("assets/sfx/petal_buzz.ogg", 0.7f);
            break;
        case IntroEffectKind::Flash:
            m_introFlash = effect.seconds;
            break;
        case IntroEffectKind::FurBarFirst:
            m_introFurTremble.start();   // FUR-003: the bar shows up for the first time, trembling
            break;
        case IntroEffectKind::Wilt:
            m_introWilted = true;
            break;
        }
    }
}

void BattleScene::startIntroRound() {
    m_introRound = m_intro->round();
    m_introRoundActive = true;
    m_introRoundTime = 0.0f;
    m_introSpawned = 0;
    m_introPetals.clear();
}

void BattleScene::hitIntroSoul(GameContext& context) {
    GameState& state = context.state;
    const IntroHitResult hit = m_intro->petalsHit(state.flags, context.player.maxHp, state.furry.value);
    context.player.hp = introHpAfterHit(context.player.hp, hit.damage);   // the tutorial can't kill
    applyFurCapped(state.furry.value, state.furry.stageMax, hit.furDelta, false);
    context.audio.playSound("assets/sfx/hurt.ogg", 0.7f);
    context.audio.playSound("assets/sfx/grunt_hurt.ogg", 0.65f);
    context.shake(6.0f, 0.25f);
    context.flash({ 255, 0, 0, 255 }, 0.15f);
    m_introPetals.clear();
    m_introRoundActive = false;
    m_introInvuln = 0.8f;
}

void BattleScene::updateIntroPetals(float dt, GameContext& context) {
    if (!m_introRoundActive) startIntroRound();
    m_introRoundTime += dt;

    // soul movement inside the dodge box
    Vector2 move{ 0.0f, 0.0f };
    if (context.input.isLeft()) move.x -= 1.0f;
    if (context.input.isRight()) move.x += 1.0f;
    if (context.input.isUp()) move.y -= 1.0f;
    if (context.input.isDown()) move.y += 1.0f;
    m_introSoul.x += move.x * INTRO_SOUL_SPEED * dt;
    m_introSoul.y += move.y * INTRO_SOUL_SPEED * dt;
    m_introSoul.x = std::clamp(m_introSoul.x, m_introBox.x + INTRO_SOUL_PADDING,
                               m_introBox.x + m_introBox.width - INTRO_SOUL_PADDING);
    m_introSoul.y = std::clamp(m_introSoul.y, m_introBox.y + INTRO_SOUL_PADDING,
                               m_introBox.y + m_introBox.height - INTRO_SOUL_PADDING);

    // spawn the petals from the top of the box, aimed at where the soul is now
    while (m_introSpawned < m_introRound.count &&
           m_introRoundTime >= INTRO_PETAL_INTERVAL * static_cast<float>(m_introSpawned) + 0.4f) {
        IntroPetal petal;
        const float span = std::max(10.0f, m_introBox.width - 50.0f);
        petal.pos = { m_introBox.x + 25.0f + static_cast<float>(GetRandomValue(0, static_cast<int>(span))),
                      m_introBox.y + 8.0f };
        const Vector2 d{ m_introSoul.x - petal.pos.x, m_introSoul.y - petal.pos.y };
        const float len = std::max(1.0f, std::sqrt(d.x * d.x + d.y * d.y));
        petal.vel = { d.x / len * m_introRound.speed, d.y / len * m_introRound.speed };
        petal.homing = !m_introRound.dodgeable;
        m_introPetals.push_back(petal);
        ++m_introSpawned;
    }

    const float rampSpeed = m_introRound.speed + m_introRoundTime * INTRO_FORCED_RAMP;
    for (IntroPetal& petal : m_introPetals) {
        petal.age += dt;
        if (petal.homing) {
            const Vector2 d{ m_introSoul.x - petal.pos.x, m_introSoul.y - petal.pos.y };
            const float len = std::max(1.0f, std::sqrt(d.x * d.x + d.y * d.y));
            petal.vel = { d.x / len * rampSpeed, d.y / len * rampSpeed };   // can't be outrun
        }
        petal.pos.x += petal.vel.x * dt;
        petal.pos.y += petal.vel.y * dt;
    }

    // the hit
    if (m_introInvuln <= 0.0f) {
        for (const IntroPetal& petal : m_introPetals) {
            const float dx = petal.pos.x - m_introSoul.x;
            const float dy = petal.pos.y - m_introSoul.y;
            if (dx * dx + dy * dy <= INTRO_PETAL_HIT_RADIUS * INTRO_PETAL_HIT_RADIUS) {
                hitIntroSoul(context);
                return;
            }
        }
    }

    // petals that left the box are dodged
    m_introPetals.erase(
        std::remove_if(m_introPetals.begin(), m_introPetals.end(), [this](const IntroPetal& petal) {
            return !petal.homing && petal.age > 0.5f &&
                   (petal.pos.x < m_introBox.x - 12.0f || petal.pos.x > m_introBox.x + m_introBox.width + 12.0f ||
                    petal.pos.y < m_introBox.y - 12.0f || petal.pos.y > m_introBox.y + m_introBox.height + 12.0f);
        }),
        m_introPetals.end());

    if (m_introRound.dodgeable && m_introSpawned >= m_introRound.count && m_introPetals.empty()) {
        m_introRoundActive = false;
        m_intro->petalsDodged(context.state.flags);
    }
}

void BattleScene::updateIntroText(float dt, GameContext& context) {
    const IntroView view = m_intro->view();
    if (m_introLastPhase != IntroPhase::Text) m_introTyped = 0.0f;
    const float length = static_cast<float>(view.text.size());
    m_introTyped = std::min(length, m_introTyped + INTRO_TYPE_CPS * dt);
    if (context.input.isConfirmPressed()) {
        if (m_introTyped < length) {
            m_introTyped = length;
        } else {
            m_introTyped = 0.0f;
            m_intro->confirm(context.state.flags);
        }
    }
}

void BattleScene::updateIntro(float dt, GameContext& context) {
    if (!m_intro) {
        m_introActive = false;
        return;
    }
    IntroFlags& flags = context.state.flags;
    m_introTime += dt;
    m_introFlash = std::max(0.0f, m_introFlash - dt);
    m_introInvuln = std::max(0.0f, m_introInvuln - dt);
    if (m_introFurTremble.update(dt)) context.state.setFlag("fur_hud_seen", 1);

    m_intro->tick(dt, flags);

    switch (m_intro->phase()) {
    case IntroPhase::Text:
        updateIntroText(dt, context);
        break;
    case IntroPhase::Choice: {
        const int count = static_cast<int>(m_intro->options().size());
        if (m_introLastPhase != IntroPhase::Choice) m_introChoice = 0;
        if (context.input.isUpPressed() && count > 0) {
            m_introChoice = (m_introChoice + count - 1) % count;
            context.audio.playSound("assets/sfx/menu_move.wav", 0.5f);
        }
        if (context.input.isDownPressed() && count > 0) {
            m_introChoice = (m_introChoice + 1) % count;
            context.audio.playSound("assets/sfx/menu_move.wav", 0.5f);
        }
        if (context.input.isConfirmPressed()) {
            context.audio.playSound("assets/sfx/menu_select.wav", 0.8f);
            m_intro->chooseOption(m_introChoice, flags);
        }
        break;
    }
    case IntroPhase::Petals:
        updateIntroPetals(dt, context);
        break;
    case IntroPhase::Pause:
    case IntroPhase::Finished:
        break;
    }
    m_introLastPhase = m_intro->phase();
    playIntroEffects(context);

    if (m_intro->finished()) {
        endIntro(context);
        return;
    }

    // box: full width for box text and choices, the small dodge box otherwise
    const bool bigBox = m_intro->phase() == IntroPhase::Choice ||
        (m_intro->phase() == IntroPhase::Text && m_intro->view().speaker == IntroSpeaker::Narrator);
    const Rectangle target = bigBox ? INTRO_BIG_BOX : INTRO_SMALL_BOX;
    const float speed = std::clamp(20.0f * dt, 0.0f, 1.0f);
    m_introBox.x += (target.x - m_introBox.x) * speed;
    m_introBox.y += (target.y - m_introBox.y) * speed;
    m_introBox.width += (target.width - m_introBox.width) * speed;
    m_introBox.height += (target.height - m_introBox.height) * speed;
    if (m_intro->phase() != IntroPhase::Petals) {
        m_introSoul = { m_introBox.x + m_introBox.width * 0.5f, m_introBox.y + m_introBox.height * 0.5f };
        m_introRoundActive = false;
    }
}

void BattleScene::drawIntroBubble(GameContext& context, const std::string& text, std::size_t shownChars, int enemyX) {
    Font font = context.resources.getFont8Bit(16);
    const std::vector<std::string> lines = wrapIntroText(context, font, text, 16.0f, 230.0f, 230.0f);
    float widest = 0.0f;
    for (const std::string& line : lines) {
        widest = std::max(widest, context.resources.measureTextWidth(font, line, 16.0f, 1.0f));
    }
    const float bubbleW = std::clamp(widest + 24.0f, 100.0f, 260.0f);
    const float bubbleH = 20.0f + 20.0f * static_cast<float>(lines.size());
    Rectangle rect{ static_cast<float>(enemyX) + 45.0f, static_cast<float>(ENEMY_BASELINE_Y) - 165.0f, bubbleW, bubbleH };
    rect.x = std::min(rect.x, static_cast<float>(VIRTUAL_WIDTH) - bubbleW - 8.0f);

    DrawRectangleRec(rect, WHITE);
    DrawRectangleLinesEx(rect, 2.0f, BLACK);
    const Vector2 p1{ rect.x, rect.y + 16.0f };
    const Vector2 p2{ rect.x, rect.y + 32.0f };
    const Vector2 p3{ rect.x - 14.0f, rect.y + 24.0f };
    DrawTriangle(p1, p2, p3, WHITE);
    DrawLineEx(p1, p3, 2.0f, BLACK);
    DrawLineEx(p2, p3, 2.0f, BLACK);

    std::size_t left = shownChars;
    float y = rect.y + 10.0f;
    for (const std::string& line : lines) {
        if (left == 0U) break;
        const std::size_t take = introLineChars(line, left);
        context.resources.drawText(font, line.substr(0, take), { rect.x + 10.0f, y }, 16, BLACK, 1.0f);
        y += 20.0f;
    }
}

void BattleScene::drawIntroBoxText(GameContext& context, const std::string& text, std::size_t shownChars) {
    Font font = context.resources.getFont8Bit(22);
    const float startX = m_introBox.x + 25.0f;
    const float indentX = startX + 24.0f;
    const bool asterisk = text.rfind("* ", 0) == 0;
    const float firstW = m_introBox.x + m_introBox.width - 25.0f - startX;
    const float restW = m_introBox.x + m_introBox.width - 25.0f - (asterisk ? indentX : startX);
    const std::vector<std::string> lines = wrapIntroText(context, font, text, 20.0f, firstW, restW);
    std::size_t left = shownChars;
    float y = m_introBox.y + 24.0f;
    for (std::size_t i = 0; i < lines.size(); ++i) {
        if (left == 0U) break;
        const std::size_t take = introLineChars(lines[i], left);
        context.resources.drawText(font, lines[i].substr(0, take),
                                   { (i == 0U || !asterisk) ? startX : indentX, y }, 20, WHITE, 1.0f);
        y += 30.0f;
    }
}

void BattleScene::drawIntroHud(GameContext& context) {
    Font font = context.resources.getFont8Bit(22);
    context.resources.drawText(font, context.player.name, { (float)HUD_NAME_X, (float)HUD_Y }, 20, WHITE, 1.0f);
    context.resources.drawText(font, "LV " + std::to_string(context.player.level), { (float)HUD_LV_X, (float)HUD_Y }, 20, WHITE, 1.0f);
    context.resources.drawText(font, "HP", { (float)HUD_HP_LABEL_X, (float)HUD_Y + 4 }, 14, WHITE, 1.0f);

    const int barW = std::max(20, (int)(static_cast<float>(context.player.maxHp) * 1.2f));
    DrawRectangle(HUD_HP_BAR_X, HUD_Y, barW, HUD_HP_BAR_H, HP_RED);
    const float hpPct = std::clamp((float)context.player.hp / (float)std::max(1, context.player.maxHp), 0.0f, 1.0f);
    DrawRectangle(HUD_HP_BAR_X, HUD_Y, (int)(static_cast<float>(barW) * hpPct), HUD_HP_BAR_H, HP_YELLOW);

    const bool furVisible = furHudVisible(context.state.furry.value);
    const int jitter = m_introFurTremble.jitter();
    if (furVisible) {
        const float furPct = std::clamp(static_cast<float>(context.state.furry.value) / static_cast<float>(FUR_MAX), 0.0f, 1.0f);
        const int furW = (int)(static_cast<float>(barW) * furPct);
        DrawRectangle(HUD_HP_BAR_X + barW - furW + jitter, HUD_Y + jitter, furW, HUD_HP_BAR_H, KR_PURPLE);
    }
    const int barEnd = HUD_HP_BAR_X + barW;
    int numX = barEnd + 16;
    if (furVisible) {
        const float labelX = static_cast<float>(barEnd + 10);
        context.resources.drawText(font, "FUR", { labelX + static_cast<float>(jitter), static_cast<float>(HUD_Y + 4 + jitter) }, 14, KR_PURPLE, 1.0f);
        numX = static_cast<int>(labelX + context.resources.measureTextWidth(font, "FUR", 14, 1.0f) + 10.0f);
    }
    const std::string hpStr = std::to_string(context.player.hp) + " / " + std::to_string(context.player.maxHp);
    context.resources.drawText(font, hpStr, { static_cast<float>(numX), static_cast<float>(HUD_Y) }, 20,
                               furVisible ? KR_PURPLE : WHITE, 1.0f);
}

void BattleScene::renderIntro(GameContext& context) {
    if (!m_intro) return;
    drawBattleBackground(backgroundFromId(m_backgroundId), m_introTime);

    const IntroPhase phase = m_intro->phase();
    const IntroView view = m_intro->view();
    const bool floofyTalking = phase == IntroPhase::Text && view.speaker == IntroSpeaker::Floofy;
    const int enemyX = VIRTUAL_WIDTH / 2;
    const int baselineY = ENEMY_BASELINE_Y;

    // Floofy: talking while he speaks, sinister on the flash and after the hit
    const std::string spriteBase = "assets/sprites/battle/" + m_enemySprite;
    const SpriteSheetDef* sheet = spriteSheetCache().load(spriteBase + ".png", spriteBase + ".json");
    Texture2D tex{ 0, 0, 0, 0, 0 };
    if (sheet != nullptr) tex = context.resources.getTexture(spriteBase + ".png");
    if (sheet != nullptr && tex.id > 0) {
        std::string anim = "idle";
        if ((m_introFlash > 0.0f || m_introWilted) && sheet->anims.count("sinister") != 0U) anim = "sinister";
        else if (floofyTalking && sheet->anims.count("talking") != 0U) anim = "talking";
        SetTextureFilter(tex, TEXTURE_FILTER_POINT);
        drawSprite(tex, *sheet, anim, m_introTime, { static_cast<float>(enemyX), static_cast<float>(baselineY) }, 2.0f, WHITE);
    } else {
        DrawCircle(enemyX, baselineY - 110, 32, { 120, 120, 140, 255 });
        DrawRectangle(enemyX - 35, baselineY - 80, 70, 70, { 50, 50, 70, 255 });
    }

    // the petals buzzing around him (beat 10); they wilt black after the hit
    if (m_introPetalsShown) {
        const Color tint = m_introWilted ? Color{ 70, 60, 80, 255 } : WHITE;
        for (int i = 0; i < 6; ++i) {
            const float angle = static_cast<float>(i) * 1.0472f + m_introTime * 0.7f;
            const float wobble = std::sin(m_introTime * 40.0f + static_cast<float>(i)) * 1.5f;
            const float sag = m_introWilted ? 10.0f : 0.0f;
            const Vector2 pos{ static_cast<float>(enemyX) + std::cos(angle) * 78.0f + wobble,
                               static_cast<float>(baselineY) - 70.0f + std::sin(angle) * 34.0f + sag };
            drawIntroPetal(context, pos, m_introTime + static_cast<float>(i) * 0.1f, tint);
        }
    }

    // the box
    DrawRectangleRec(m_introBox, BLACK);
    DrawRectangleLinesEx(m_introBox, 5.0f, WHITE);

    if (phase == IntroPhase::Text && view.speaker == IntroSpeaker::Narrator) {
        drawIntroBoxText(context, view.text, static_cast<std::size_t>(m_introTyped));
    } else if (phase == IntroPhase::Choice) {
        Font font = context.resources.getFont8Bit(22);
        const std::vector<std::string> labels = m_intro->options();
        for (std::size_t i = 0; i < labels.size(); ++i) {
            const float optX = m_introBox.x + 55.0f;
            const float optY = m_introBox.y + 25.0f + static_cast<float>(i) * 35.0f;
            if (static_cast<int>(i) == m_introChoice) drawIntroSoul(context, optX - 20.0f, optY + 10.0f, SOUL_RED);
            context.resources.drawText(font, "* " + labels[i], { optX, optY }, 20, WHITE, 1.0f);
        }
    } else {
        if (phase == IntroPhase::Petals) {
            BeginScissorMode(static_cast<int>(m_introBox.x) + 5, static_cast<int>(m_introBox.y) + 5,
                             std::max(1, static_cast<int>(m_introBox.width) - 10),
                             std::max(1, static_cast<int>(m_introBox.height) - 10));
            for (const IntroPetal& petal : m_introPetals) {
                const Vector2 pos{ petal.pos.x, petal.pos.y + std::sin(petal.age * 30.0f) * INTRO_PETAL_Y_WOBBLE };
                drawIntroPetal(context, pos, petal.age, WHITE);
            }
            EndScissorMode();
        }
        if (m_introInvuln <= 0.0f || static_cast<int>(GetTime() * 15.0) % 2 == 0) {
            drawIntroSoul(context, m_introSoul.x, m_introSoul.y, SOUL_RED);
        }
    }

    if (floofyTalking) {
        drawIntroBubble(context, view.text, static_cast<std::size_t>(m_introTyped), enemyX);
    }

    drawIntroHud(context);
    drawPkSkull(context);
}

// ---------------------------------------------------------------- TOX-002..008: the toxic track in battle

namespace {

constexpr float TOXIC_HITSPLAT_LIFETIME = 1.5f;     // same as BattleSystem's damage number, so the splat covers it exactly
constexpr float TOXIC_HITMARKER_LIFETIME = 0.25f;
constexpr float TOXIC_HITMARKER_INNER = 6.0f;
constexpr float TOXIC_HITMARKER_TICK = 10.0f;
constexpr int TOXIC_ENEMY_CENTER_Y = ENEMY_BASELINE_Y - 105;
constexpr float TOXIC_HP_BAR_Y = static_cast<float>(ENEMY_BASELINE_Y) - 145.0f;   // where BattleSystem puts the damage number
constexpr float TOXIC_BANNER_Y = 118.0f;
constexpr int TOXIC_MERCY_BUTTON_X = BATTLE_BUTTON_X_MERCY;
constexpr int TOXIC_FIGHT_BUTTON_X = BATTLE_BUTTON_X_FIGHT;

constexpr Color OSRS_SPLAT_RED{ 196, 28, 28, 255 };
constexpr Color OSRS_SPLAT_RED_EDGE{ 92, 6, 6, 255 };
constexpr Color OSRS_SPLAT_BLUE{ 44, 98, 214, 255 };
constexpr Color OSRS_SPLAT_BLUE_EDGE{ 12, 30, 92, 255 };
constexpr Color LOL_BANNER_RED{ 150, 18, 28, 255 };
constexpr Color LOL_BANNER_GOLD{ 240, 196, 72, 255 };
constexpr Color XP_GOLD{ 255, 208, 64, 255 };

Color withAlpha(Color color, float alpha) {
    color.a = static_cast<unsigned char>(std::clamp(alpha, 0.0f, 1.0f) * static_cast<float>(color.a));
    return color;
}

Color killfeedColor(const KillfeedColor& c, float alpha) {
    return withAlpha(Color{ c.r, c.g, c.b, c.a }, alpha);
}

// Deterministic noise for the glitch overlay; the game's own RNG is never consumed by cosmetics.
unsigned int toxicNoise(unsigned int seed) {
    seed ^= seed << 13U;
    seed ^= seed >> 17U;
    seed ^= seed << 5U;
    return seed * 2654435761U;
}

float toxicNoise01(unsigned int seed) {
    return static_cast<float>(toxicNoise(seed) % 1000U) / 1000.0f;
}

std::string announcerVoiceId(KillAnnouncementKind kind) {
    switch (kind) {
    case KillAnnouncementKind::FirstBlood: return "announcer_first_blood";
    case KillAnnouncementKind::DoubleKill: return "announcer_double_kill";
    case KillAnnouncementKind::TripleKill: return "announcer_triple_kill";
    case KillAnnouncementKind::QuadraKill: return "announcer_quadra_kill";
    case KillAnnouncementKind::PentaKill: return "announcer_penta_kill";
    case KillAnnouncementKind::Ace: return "announcer_ace";
    case KillAnnouncementKind::None: break;
    }
    return "";
}

// A tiny pixel icon for the killfeed: '#' cells are drawn, scaled by `scale`.
template <std::size_t N>
void drawPixelIcon(const std::array<const char*, N>& rows, float x, float y, int scale, Color color) {
    for (std::size_t row = 0; row < N; ++row) {
        for (std::size_t col = 0; rows[row][col] != '\0'; ++col) {
            if (rows[row][col] != '#') continue;
            DrawRectangle(static_cast<int>(x) + static_cast<int>(col) * scale,
                          static_cast<int>(y) + static_cast<int>(row) * scale, scale, scale, color);
        }
    }
}

constexpr std::array<const char*, 8> PK_SKULL_ROWS{
    "..####..",
    ".######.",
    "##.##.##",
    "##.##.##",
    ".######.",
    "..####..",
    "..#.#.#.",
    "..####..",
};
constexpr int PK_SKULL_SCALE = 2;

constexpr std::array<const char*, 5> KILLFEED_WEAPON_ROWS{
    "......##",
    ".....##.",
    "....##..",
    "###.#...",
    "##......",
};
constexpr std::array<const char*, 7> KILLFEED_HEADSHOT_ROWS{
    "..###..",
    ".#...#.",
    "#..#..#",
    "#.###.#",
    "#..#..#",
    ".#...#.",
    "..###..",
};

} // namespace

void BattleScene::updateToxicFx(float dt, GameContext& context, FightPhase phaseBefore, int killsBefore, int expBefore) {
    m_toxicClock += dt;
    // The overlays that are already running finish on their own; nothing new starts off the toxic track.
    m_hitmarker.update(dt);
    m_toxicFx.update(dt);
    m_killfeed.update(dt);
    m_announcer.update(dt);

    if (m_announcer.hasActiveBanner() && m_announcer.currentBanner().kind != m_voicedBanner) {
        m_voicedBanner = m_announcer.currentBanner().kind;
        const std::string voice = "assets/sfx/" + announcerVoiceId(m_voicedBanner) + ".ogg";
        // AUD-014: the voice lines are not recorded yet; the bell stands in until they exist.
        const std::string path = std::filesystem::exists(voice) ? voice : "assets/sfx/bell.ogg";
        context.audio.playSound(path, 0.6f);
    } else if (!m_announcer.hasActiveBanner()) {
        m_voicedBanner = KillAnnouncementKind::None;
    }

    if (!m_battleSystem || toxicTrack(context.state) != 1) return;

    const FightPhase phaseAfter = m_battleSystem->getFightPhase();
    if (phaseBefore == FightPhase::Moving && phaseAfter != FightPhase::Moving) onToxicHit(context);

    if (toxicKills(context.state) > killsBefore) onToxicKill(context);

    const int expGain = context.player.exp - expBefore;
    if (expGain > 0 && xpDropsActive(context.state)) m_toxicFx.spawnXpDrop(expGain);
}

void BattleScene::onToxicHit(GameContext& context) {
    const bool miss = m_battleSystem->wasLastMiss();
    const bool crit = !miss && m_battleSystem->wasLastCrit();
    const int damage = m_battleSystem->getLastDamage();
    m_lastHitWasCrit = crit;

    if (!miss && shouldPlayToxicHitmarker(context.state)) {
        m_hitmarker.spawn(static_cast<float>(VIRTUAL_WIDTH / 2), static_cast<float>(TOXIC_ENEMY_CENTER_Y),
                          TOXIC_HITMARKER_LIFETIME, TOXIC_HITMARKER_INNER, TOXIC_HITMARKER_TICK);
        // a critical hit already plays the hitmarker in BattleSystem (FGT-008)
        if (!crit) context.audio.playSound(std::string(hitmarkerSoundId()), 0.7f);
    }
    if (hitsplatsActive(context.state)) {
        m_toxicFx.spawnHitsplat(hitsplatKindFor(damage, crit, miss), damage,
                                static_cast<float>(VIRTUAL_WIDTH / 2), TOXIC_HP_BAR_Y, TOXIC_HITSPLAT_LIFETIME);
    }
}

void BattleScene::onToxicKill(GameContext& context) {
    GameState& state = context.state;
    m_announcer.onKill(toxicKills(state), toxicStreak(state));

    // ACE: the notice is remembered in a flag, like the overworld does
    const std::string area = "ruins";
    const int areaKills = state.getFlag("kills." + area, 0);
    const std::string announcedFlag = areaExhaustedAnnouncedFlag(area);
    if (areaExhaustedNoticeDue(area, areaKills, state.getFlag(announcedFlag, 0))) {
        state.setFlag(announcedFlag, 1);
        m_announcer.enqueue(makeAceAnnouncement());
    }

    // ex-gamers show their old gamertag; everybody else their name
    const std::string victim = lookupExGamerGamertag(m_enemyId).empty() ? m_enemyName : m_enemyId;
    const std::string attacker = context.player.name.empty() ? "GRUB" : context.player.name;
    const std::string weapon = context.player.weapon.empty() ? "crowbar" : context.player.weapon;
    m_killfeed.push(attacker, weapon, victim, m_lastHitWasCrit);
}

void BattleScene::renderToxicBackdrop(GameContext& context) const {
    if (toxicTrack(context.state) != 1) return;
    const CorruptionLook look = corruptionLook(context.state);
    if (look.stage <= 0) return;

    const float clock = m_toxicClock;
    const unsigned int tick = static_cast<unsigned int>(clock * 12.0f);
    const float flicker = look.gridFlicker * (0.55f + 0.45f * std::sin(clock * 19.0f)) *
                          (toxicNoise01(tick * 7U + 3U) > 0.35f ? 1.0f : 0.25f);

    // the text box, the buttons and the HUD line are never covered: only the play field above the box is tinted
    const int fieldBottom = BATTLE_BOX_Y - 6;
    const int sideTop = BATTLE_BOX_Y - 6;
    const int sideBottom = BATTLE_BOX_Y + BATTLE_BOX_H + 6;
    const int boxRight = BATTLE_BOX_X + BATTLE_BOX_W;

    // 1. dark red tint (9+ kills)
    const float tintAlpha = look.overlayAlpha * look.gridDarkness;
    if (tintAlpha > 0.0f) {
        const Color tint = withAlpha(Color{ 70, 0, 6, 255 }, tintAlpha);
        DrawRectangle(0, 0, VIRTUAL_WIDTH, fieldBottom, tint);
        DrawRectangle(0, sideTop, BATTLE_BOX_X - 3, sideBottom - sideTop, tint);
        DrawRectangle(boxRight + 3, sideTop, VIRTUAL_WIDTH - boxRight - 3, sideBottom - sideTop, tint);
    }

    // 2. the grid flickers red at the edges
    const Color edge = withAlpha(Color{ 230, 24, 24, 255 }, 0.55f * flicker);
    DrawRectangle(0, 0, 5, VIRTUAL_HEIGHT - 90, edge);
    DrawRectangle(VIRTUAL_WIDTH - 5, 0, 5, VIRTUAL_HEIGHT - 90, edge);
    DrawRectangle(0, 0, VIRTUAL_WIDTH, 4, edge);

    // 3. scanlines and glitch blocks, kept to the play field above the box
    for (int i = 0; i < look.scanlines; ++i) {
        const unsigned int seed = tick * 31U + static_cast<unsigned int>(i) * 97U;
        const int y = static_cast<int>(toxicNoise01(seed) * static_cast<float>(fieldBottom - 2));
        const Color line = (i % 2 == 0) ? Color{ 255, 70, 70, 255 } : Color{ 255, 255, 255, 255 };
        DrawRectangle(0, y, VIRTUAL_WIDTH, 1, withAlpha(line, 0.22f));
    }
    const int blocks = static_cast<int>(look.glitch * 8.0f);
    for (int i = 0; i < blocks; ++i) {
        const unsigned int seed = tick * 53U + static_cast<unsigned int>(i) * 131U + 11U;
        const int w = 24 + static_cast<int>(toxicNoise01(seed) * 90.0f);
        const int x = static_cast<int>(toxicNoise01(seed + 1U) * static_cast<float>(VIRTUAL_WIDTH - w));
        const int y = static_cast<int>(toxicNoise01(seed + 2U) * static_cast<float>(fieldBottom - 8));
        const Color glitch = (i % 2 == 0) ? Color{ 255, 40, 60, 255 } : Color{ 40, 220, 255, 255 };
        DrawRectangle(x, y, w, 2 + static_cast<int>(toxicNoise01(seed + 3U) * 3.0f), withAlpha(glitch, 0.30f * look.glitch + 0.1f));
    }

    // 4. MERCY goes gray (and cracks), FIGHT glows. Both stay fully usable and readable.
    const Rectangle mercy{ static_cast<float>(TOXIC_MERCY_BUTTON_X), static_cast<float>(BATTLE_BUTTON_Y),
                           static_cast<float>(BATTLE_BUTTON_W), static_cast<float>(BATTLE_BUTTON_H) };
    if (look.mercyGray) {
        DrawRectangleRec(mercy, withAlpha(Color{ 110, 110, 110, 255 }, 0.45f));
    }
    if (look.mercyCracked) {
        const Color crack{ 12, 0, 0, 255 };
        // the crack runs down the gap between the icon and the label, so the word stays readable
        DrawLineEx({ mercy.x + 33.0f, mercy.y }, { mercy.x + 37.0f, mercy.y + 12.0f }, 2.0f, crack);
        DrawLineEx({ mercy.x + 37.0f, mercy.y + 12.0f }, { mercy.x + 31.0f, mercy.y + 24.0f }, 2.0f, crack);
        DrawLineEx({ mercy.x + 31.0f, mercy.y + 24.0f }, { mercy.x + 35.0f, mercy.y + BATTLE_BUTTON_H }, 2.0f, crack);
        DrawLineEx({ mercy.x + 37.0f, mercy.y + 12.0f }, { mercy.x + 44.0f, mercy.y + 16.0f }, 1.0f, crack);
    }
    if (look.fightGlow) {
        const Rectangle fight{ static_cast<float>(TOXIC_FIGHT_BUTTON_X), static_cast<float>(BATTLE_BUTTON_Y),
                               static_cast<float>(BATTLE_BUTTON_W), static_cast<float>(BATTLE_BUTTON_H) };
        const float pulse = 0.5f + 0.5f * std::sin(clock * 5.0f);
        DrawRectangleLinesEx({ fight.x - 5.0f, fight.y - 5.0f, fight.width + 10.0f, fight.height + 10.0f }, 2.0f,
                             withAlpha(Color{ 255, 30, 30, 255 }, 0.18f + 0.15f * pulse));
        DrawRectangleLinesEx({ fight.x - 2.0f, fight.y - 2.0f, fight.width + 4.0f, fight.height + 4.0f }, 2.0f,
                             withAlpha(Color{ 255, 40, 40, 255 }, 0.45f + 0.35f * pulse));
    }
}

void BattleScene::renderToxicOverlay(GameContext& context) const {
    // Finished fx are drained in update(); the gate here keeps the neutral route untouched.
    if (toxicTrack(context.state) != 1) return;
    drawPkSkull(context);
    drawHitsplats(context);
    drawHitmarker();
    drawXpDrops(context);
    drawKillfeed(context);
    drawAnnouncerBanner(context);
}

void BattleScene::drawPkSkull(GameContext& context) const {
    if (toxicTrack(context.state) != 1 || toxicKills(context.state) < 1) return;   // TOX-004: from the first kill, visual only
    Font font = context.resources.getFont8Bit(22);
    const float nameW = context.resources.measureTextWidth(font, context.player.name, 20, 1.0f);
    const float skullW = 8.0f * static_cast<float>(PK_SKULL_SCALE);
    float x = static_cast<float>(HUD_NAME_X) + nameW + 6.0f;
    float y = static_cast<float>(HUD_Y) + 2.0f;
    if (x + skullW > static_cast<float>(HUD_LV_X) - 4.0f) {   // a long name: the skull rides above it instead
        x = static_cast<float>(HUD_NAME_X);
        y = static_cast<float>(HUD_Y) - skullW - 2.0f;
    }
    drawPixelIcon(PK_SKULL_ROWS, x, y, PK_SKULL_SCALE, WHITE);
}

void BattleScene::drawHitsplats(GameContext& context) const {
    Font font = context.resources.getFont8Bit(22);
    for (const Hitsplat& splat : m_toxicFx.hitsplats()) {
        if (!splat.active()) continue;
        const bool miss = splat.kind == HitsplatKind::Miss;
        const bool max = splat.kind == HitsplatKind::MaxHit;
        const float alpha = splat.alpha();

        // it sits exactly where BattleSystem draws its own number / MISS, and covers it
        float top = 0.0f;
        std::string text;
        float coverSize = 26.0f;
        if (miss) {
            top = splat.y - 6.0f;
            text = "0";
            coverSize = 24.0f;
        } else {
            const float t = splat.timer;
            const float bounce = (t > 1.2f) ? -12.0f * std::sin((1.5f - t) / 0.3f * 3.14159f) : -4.0f;
            top = splat.y - 26.0f + bounce;
            text = std::to_string(splat.amount);
        }
        const std::string cover = miss ? "MISS" : text;
        const float coverW = context.resources.measureTextWidth(font, cover, coverSize, 1.0f);
        const float width = std::max(34.0f, coverW + 18.0f);
        const Rectangle rect{ static_cast<float>(VIRTUAL_WIDTH / 2) - width * 0.5f, top - 4.0f, width, coverSize + 8.0f };

        const Color fill = miss ? OSRS_SPLAT_BLUE : OSRS_SPLAT_RED;
        const Color edge = miss ? OSRS_SPLAT_BLUE_EDGE : OSRS_SPLAT_RED_EDGE;
        DrawRectangleRounded(rect, 0.5f, 6, withAlpha(fill, alpha));
        DrawRectangleRoundedLinesEx(rect, 0.5f, 6, 2.0f, withAlpha(edge, alpha));
        if (max) {
            // the max-hit splat: a pale rim
            DrawRectangleRoundedLinesEx({ rect.x + 3.0f, rect.y + 3.0f, rect.width - 6.0f, rect.height - 6.0f }, 0.5f, 6, 1.0f,
                                        withAlpha(Color{ 255, 220, 220, 255 }, alpha));
        }
        const float textW = context.resources.measureTextWidth(font, text, 22, 1.0f);
        const Vector2 pos{ rect.x + (rect.width - textW) * 0.5f, rect.y + 5.0f };
        context.resources.drawText(font, text, { pos.x + 1.0f, pos.y + 1.0f }, 22, withAlpha(BLACK, alpha), 1.0f);
        context.resources.drawText(font, text, pos, 22, withAlpha(WHITE, alpha), 1.0f);
    }
}

void BattleScene::drawHitmarker() const {
    if (!m_hitmarker.active) return;
    const float alpha = m_hitmarker.alpha();
    // a slight outward drift as it fades, like the CoD one
    HitmarkerState drawn = m_hitmarker;
    drawn.innerRadius += (1.0f - alpha) * 3.0f;
    for (const HitmarkerTick& tick : drawn.ticks()) {
        DrawLineEx({ tick.x1 + 1.0f, tick.y1 + 1.0f }, { tick.x2 + 1.0f, tick.y2 + 1.0f }, 3.0f, withAlpha(BLACK, alpha * 0.6f));
        DrawLineEx({ tick.x1, tick.y1 }, { tick.x2, tick.y2 }, 2.0f, withAlpha(WHITE, alpha));
    }
}

void BattleScene::drawXpDrops(GameContext& context) const {
    Font font = context.resources.getFont8Bit(22);
    for (const XpDrop& drop : m_toxicFx.xpDrops()) {
        if (!drop.active()) continue;
        const float alpha = drop.alpha();
        const std::string text = drop.text();
        const float width = context.resources.measureTextWidth(font, text, 20, 1.0f);
        const float right = static_cast<float>(VIRTUAL_WIDTH) - 14.0f;
        const float x = right - width;
        // the gold XP orb sits to the left of the number
        DrawCircle(static_cast<int>(x) - 10, static_cast<int>(drop.currentY) + 10, 6.0f, withAlpha(XP_GOLD, alpha));
        DrawCircleLines(static_cast<int>(x) - 10, static_cast<int>(drop.currentY) + 10, 6.0f, withAlpha(Color{ 120, 80, 0, 255 }, alpha));
        context.resources.drawText(font, text, { x + 1.0f, drop.currentY + 1.0f }, 20, withAlpha(BLACK, alpha), 1.0f);
        context.resources.drawText(font, text, { x, drop.currentY }, 20, withAlpha(XP_GOLD, alpha), 1.0f);
    }
}

void BattleScene::drawKillfeed(GameContext& context) const {
    if (m_killfeed.empty()) return;
    Font font = context.resources.getFont8Bit(16);
    constexpr float lineH = 18.0f;
    constexpr float gap = 6.0f;
    constexpr float iconW = 16.0f;
    float y = 8.0f;
    for (const KillfeedEntry& entry : m_killfeed.entries()) {
        // CS 1.6: no box, each line fades out over its last seconds
        const float alpha = std::min(1.0f, entry.alphaRatio() * 3.0f);
        const float attackerW = context.resources.measureTextWidth(font, entry.attacker, 16, 1.0f);
        const float victimW = context.resources.measureTextWidth(font, entry.victim, 16, 1.0f);
        float total = attackerW + gap + iconW + gap + victimW;
        if (entry.headshot) total += iconW + gap;
        float x = static_cast<float>(VIRTUAL_WIDTH) - 8.0f - total;

        const auto shadowed = [&](const std::string& text, float tx, Color color) {
            context.resources.drawText(font, text, { tx + 1.0f, y + 1.0f }, 16, withAlpha(BLACK, alpha * 0.8f), 1.0f);
            context.resources.drawText(font, text, { tx, y }, 16, color, 1.0f);
        };
        shadowed(entry.attacker, x, killfeedColor(KILLFEED_COLOR_ATTACKER_RED, alpha));
        x += attackerW + gap;
        drawPixelIcon(KILLFEED_WEAPON_ROWS, x, y + 3.0f, 2, killfeedColor(KILLFEED_COLOR_WEAPON_ORANGE, alpha));
        x += iconW + gap;
        if (entry.headshot) {
            drawPixelIcon(KILLFEED_HEADSHOT_ROWS, x, y + 1.0f, 2, killfeedColor(KILLFEED_COLOR_HEADSHOT_YELLOW, alpha));
            x += iconW + gap;
        }
        shadowed(entry.victim, x, killfeedColor(KILLFEED_COLOR_VICTIM_BLUE, alpha));
        y += lineH;
    }
}

void BattleScene::drawAnnouncerBanner(GameContext& context) const {
    if (!m_announcer.hasActiveBanner()) return;
    const KillAnnouncement& banner = m_announcer.currentBanner();
    const float elapsed = banner.duration - m_announcer.currentRemainingTime();
    const float remaining = m_announcer.currentRemainingTime();
    const float alpha = std::clamp(std::min(elapsed / 0.2f, remaining / 0.4f), 0.0f, 1.0f);
    const float slide = (1.0f - std::clamp(elapsed / 0.2f, 0.0f, 1.0f)) * 40.0f;   // sweeps in from the left

    Font bigFont = context.resources.getFont8Bit(36);
    Font smallFont = context.resources.getFont8Bit(16);
    const bool hasSub = !banner.subText.empty();
    const float height = hasSub ? 62.0f : 42.0f;
    const Rectangle band{ 110.0f - slide, TOXIC_BANNER_Y, 420.0f, height };

    DrawRectangleRec(band, withAlpha(LOL_BANNER_RED, 0.88f * alpha));
    DrawRectangle(static_cast<int>(band.x), static_cast<int>(band.y), static_cast<int>(band.width), 3, withAlpha(LOL_BANNER_GOLD, alpha));
    DrawRectangle(static_cast<int>(band.x), static_cast<int>(band.y + band.height) - 3, static_cast<int>(band.width), 3, withAlpha(LOL_BANNER_GOLD, alpha));

    const float textW = context.resources.measureTextWidth(bigFont, banner.bannerText, 32, 2.0f);
    const Vector2 textPos{ band.x + (band.width - textW) * 0.5f, band.y + 7.0f };
    context.resources.drawText(bigFont, banner.bannerText, { textPos.x + 2.0f, textPos.y + 2.0f }, 32, withAlpha(Color{ 40, 4, 8, 255 }, alpha), 2.0f);
    context.resources.drawText(bigFont, banner.bannerText, textPos, 32, withAlpha(LOL_BANNER_GOLD, alpha), 2.0f);
    if (hasSub) {
        const float subW = context.resources.measureTextWidth(smallFont, banner.subText, 16, 1.0f);
        context.resources.drawText(smallFont, banner.subText, { band.x + (band.width - subW) * 0.5f, band.y + 40.0f }, 16, withAlpha(WHITE, alpha), 1.0f);
    }
}

void BattleScene::onExit(GameContext& /*context*/) {
}

} // namespace nundertale
