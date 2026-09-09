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
#include <algorithm>
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
    tiled_detail::JsonValue root;
    if (!tiled_detail::JsonParser(ss.str()).parse(root) ||
        root.kind != tiled_detail::JsonValue::Kind::Object) {
        std::clog << "Invalid encounter data for '" << encounterId << "'; battle not started.\n";
        return false;
    }

    const tiled_detail::JsonValue* enemies = root.field("enemies");
    if (enemies == nullptr || enemies->kind != tiled_detail::JsonValue::Kind::Array ||
        enemies->array.empty() || enemies->array[0].kind != tiled_detail::JsonValue::Kind::String) {
        std::clog << "Encounter '" << encounterId << "' has no supported enemy; battle not started.\n";
        return false;
    }

    const std::string enemyId = enemies->array[0].string;
    if (!safeEncounterId(enemyId) || !std::filesystem::exists("data/enemies/" + enemyId + ".json")) {
        std::clog << "Encounter '" << encounterId << "' references unknown enemy '"
                  << enemyId << "'; battle not started.\n";
        return false;
    }

    std::string introText = "* " + enemyId + " blocked your path!";
    if (const auto* value = root.field("introText"); value != nullptr &&
        value->kind == tiled_detail::JsonValue::Kind::String) introText = value->string;
    std::string backgroundId = m_backgroundId;
    if (const auto* value = root.field("background"); value != nullptr &&
        value->kind == tiled_detail::JsonValue::Kind::String) backgroundId = value->string;
    std::string musicId = m_musicId;
    if (const auto* value = root.field("music"); value != nullptr &&
        value->kind == tiled_detail::JsonValue::Kind::String) musicId = value->string;
    bool canFlee = false;
    if (const auto* value = root.field("canFlee"); value != nullptr &&
        value->kind == tiled_detail::JsonValue::Kind::Boolean) canFlee = value->boolean;
    std::string fleeBlockedText;
    if (const auto* value = root.field("fleeBlockedText"); value != nullptr &&
        value->kind == tiled_detail::JsonValue::Kind::String) fleeBlockedText = value->string;
    int furCap = -1;
    if (const auto* value = root.field("furCap"); value != nullptr &&
        value->kind == tiled_detail::JsonValue::Kind::Number) furCap = static_cast<int>(value->number);

    auto enemy = Enemy::loadFromJson("data/enemies/" + enemyId + ".json");
    if (enemy) m_enemySprite = enemy->getSprite();
    m_battleSystem = std::make_unique<BattleSystem>(
        std::move(enemy), introText, canFlee, std::move(fleeBlockedText), furCap);
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
    playEncounterMusic(context);
}

void BattleScene::update(float dt, GameContext& context) {
    if (m_introActive) {
        updateIntro(dt, context);
        return;
    }
    if (m_battleSystem) {
        m_battleSystem->update(dt, context);

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

        if (m_battleSystem->isFinished() && !m_battleSystem->isFled()) {
            Font font8Bit = context.resources.getFont8Bit(20);
            context.resources.drawCenteredText(font8Bit, "[Press Z or ENTER to continue]", 364, 20, COLOR_UNDERTALE_YELLOW, 1.0f);
        }
    }
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
}

void BattleScene::onExit(GameContext& /*context*/) {
}

} // namespace nundertale
