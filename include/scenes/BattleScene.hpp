#pragma once

#include "core/IScene.hpp"
#include "combat/BattleIntroScript.hpp"
#include "combat/BattleSystem.hpp"
#include "combat/FurHud.hpp"
#include "combat/Hitmarker.hpp"
#include "combat/Hitsplat.hpp"
#include "combat/KillAnnouncer.hpp"
#include "combat/Killfeed.hpp"
#include <memory>
#include <array>
#include <string>
#include <vector>

namespace nundertale {

class BattleScene : public IScene {
public:
    explicit BattleScene(std::string encounterId = "barky");
    ~BattleScene() override = default;

    void onEnter(GameContext& context) override;
    void update(float dt, GameContext& context) override;
    void render(GameContext& context) override;
    void onExit(GameContext& context) override;
    bool isValid() const { return m_valid; }
    bool isIntroActive() const { return m_introActive; }

private:
    // Loads data/encounters/<id>.json into a BattleSystem; false leaves the current battle untouched.
    bool loadEncounter(const std::string& encounterId, bool readIntro);
    // C1-002: the fight variant lives in two encounters ("floofy", "floofy_bass"); the battle swaps in place.
    void swapEncounter(const std::string& encounterId);
    void playEncounterMusic(GameContext& context);

    // C1-002: the scripted in-box intro that runs before the first menu turn (BattleIntroScript).
    struct IntroPetal {
        Vector2 pos{ 0.0f, 0.0f };
        Vector2 vel{ 0.0f, 0.0f };
        float age = 0.0f;
        bool homing = false;
    };
    void beginIntro(GameContext& context);
    void updateIntro(float dt, GameContext& context);
    void renderIntro(GameContext& context);
    void endIntro(GameContext& context);
    void updateIntroText(float dt, GameContext& context);
    void updateIntroPetals(float dt, GameContext& context);
    void startIntroRound();
    void hitIntroSoul(GameContext& context);
    void playIntroEffects(GameContext& context);
    void drawIntroBubble(GameContext& context, const std::string& text, std::size_t shownChars, int enemyX);
    void drawIntroBoxText(GameContext& context, const std::string& text, std::size_t shownChars);
    void drawIntroHud(GameContext& context);
    void completeTutorialWave(GameContext& context, bool caught);
    void updateTutorialBranch(float dt, GameContext& context);
    void beginGiantPetal(GameContext& context);
    void updateGiantPetal(float dt, GameContext& context);
    void renderGiantPetal(GameContext& context);
    void finishR2Tutorial(GameContext& context);

    struct R2TutorialDef {
        bool enabled = false;
        std::array<int, 3> waveCounts{ 1, 3, 6 };
        std::array<float, 3> waveSpeeds{ 30.0f, 38.0f, 48.0f };
        float spawnInterval = 0.85f;
        int hitDamage = 2;
        int hitFur = 10;
        float silenceSeconds = 1.0f;
        float fadeSeconds = 1.2f;
        float giantSpeed = 20.0f;
        float giantWidth = 160.0f;
        float safeWallWidth = 12.0f;
        float rescueProgress = 0.75f;
        float buzzShakeInterval = 0.45f;
        std::string giantSprite = "assets/sprites/bullets/giant_petal.png";
        std::string silhouettePortrait = "furiell_silhouette";
        std::string endCutscene = "r2_floofy_end";
    };

    enum class GiantPetalPhase { None, Silence, FadeIn, Sweep, Spritz, Eep };

    // TOX-002..008: the toxic track's overlays. Every one of them is gated on toxic.track == 1
    // (BattleScene only watches BattleSystem from outside; it never changes the battle rules).
    void updateToxicFx(float dt, GameContext& context, FightPhase phaseBefore, int killsBefore, int expBefore);
    void onToxicHit(GameContext& context);
    void onToxicKill(GameContext& context);
    void renderToxicBackdrop(GameContext& context) const;
    void renderToxicOverlay(GameContext& context) const;
    void drawPkSkull(GameContext& context) const;
    void drawHitsplats(GameContext& context) const;
    void drawHitmarker() const;
    void drawKillfeed(GameContext& context) const;
    void drawAnnouncerBanner(GameContext& context) const;
    void drawXpDrops(GameContext& context) const;
    void updateMercyTransition(float dt, GameContext& context, int trackBefore, bool sparedBefore);
    void renderMercyGlitch(GameContext& context) const;
    void renderBossYiff(GameContext& context) const;

    std::unique_ptr<BattleSystem> m_battleSystem;
    std::string m_encounterId;
    std::string m_enemyId;
    std::string m_enemyName;
    std::string m_musicId;
    std::string m_backgroundId;
    std::string m_enemySprite;
    bool m_valid = false;
    bool m_victoryMusicStopped = false; // AUD-004

    IntroScriptDef m_introDef;
    std::unique_ptr<BattleIntroScript> m_intro;
    bool m_introActive = false;
    bool m_introChecked = false;
    float m_introTime = 0.0f;
    IntroPhase m_introLastPhase = IntroPhase::Pause;  // a page starts typing when the phase changes to Text
    float m_introTyped = 0.0f;           // characters revealed so far
    int m_introChoice = 0;
    Rectangle m_introBox{ (float)BATTLE_BOX_X, (float)BATTLE_BOX_Y, (float)BATTLE_BOX_W, (float)BATTLE_BOX_H };
    Vector2 m_introSoul{ 320.0f, 320.0f };
    bool m_introPetalsShown = false;     // the petals buzzing around Floofy (beat 10)
    bool m_introWilted = false;          // after the hit: petals wilt black, smile too wide
    float m_introFlash = 0.0f;           // the sinister face flashes
    bool m_introRoundActive = false;
    float m_introRoundTime = 0.0f;
    int m_introSpawned = 0;
    IntroPetalRound m_introRound;
    std::vector<IntroPetal> m_introPetals;
    float m_introInvuln = 0.0f;
    FurHudTrembleClock m_introFurTremble;
    R2TutorialDef m_r2Tutorial;
    int m_tutorialWaveIndex = 0;
    int m_tutorialDodges = 0;
    bool m_tutorialBranchActive = false;
    std::vector<std::string> m_tutorialBranchLines;
    std::size_t m_tutorialBranchPage = 0;
    float m_tutorialBranchTyped = 0.0f;
    GiantPetalPhase m_giantPetalPhase = GiantPetalPhase::None;
    float m_giantTimer = 0.0f;
    float m_giantSweepTime = 0.0f;
    float m_giantX = 0.0f;
    float m_giantAlpha = 0.0f;
    float m_giantBuzzTimer = 0.0f;
    std::vector<std::string> m_giantDialogLines;
    std::size_t m_giantDialogPage = 0;
    float m_giantDialogTyped = 0.0f;

    HitmarkerState m_hitmarker;
    ToxicCombatFxManager m_toxicFx;
    KillAnnouncer m_announcer;
    Killfeed m_killfeed;
    KillAnnouncementKind m_voicedBanner = KillAnnouncementKind::None;   // the banner whose voice line already played
    bool m_lastHitWasCrit = false;
    float m_toxicClock = 0.0f;           // drives the corruption flicker
    float m_mercyGlitchTimer = 0.0f;     // MRC-006: first kill visibly flips Yiff to Spare
    float m_bossYiffTimer = 0.0f;
    bool m_bossYiffActive = false;
    bool m_bossYiffDismissing = false;
    std::string m_bossYiffText;
};

} // namespace nundertale
