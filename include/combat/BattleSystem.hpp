#pragma once

#include <raylib.h>
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include "Bullet.hpp"
#include "DamageZone.hpp"
#include "Enemy.hpp"
#include "BattleBackground.hpp"
#include "ResidualDamage.hpp"
#include "AttackTarget.hpp"
#include "BarkyPatterns.hpp"
#include "FurHud.hpp"
#include "FloofyScript.hpp"
#include "BeatMap.hpp"
#include "EncounterModel.hpp"
#include "core/Constants.hpp"

namespace nundertale {

struct GameContext;

enum class BattleState {
    MenuSelect,
    FightTarget,
    ActSelect,
    ItemSelect,
    MercySelect,
    DialogMessage,
    EnemyTurn,
    Fleeing,
    Fled,
    Victory,
    Defeat
};

enum class MainButton {
    Fight = 0,
    Act = 1,
    Item = 2,
    Mercy = 3
};

// BHL-015: "thinking" ribbit text over soul easter egg
struct RibbitState {
    float stillTime = 0.0f;
    float showTime = 0.0f;
    bool shownThisTurn = false;
};

// Advances the state; returns true on the exact update where the bubble starts showing.
inline bool updateRibbit(RibbitState& s, float dt, bool soulMoving, float threshold = 3.0f, float duration = 1.5f) {
    if (soulMoving) {
        s.stillTime = 0.0f;
        s.showTime = 0.0f;
        return false;
    }

    if (s.showTime > 0.0f) {
        s.showTime -= dt;
        if (s.showTime < 0.0f) {
            s.showTime = 0.0f;
        }
    }

    if (s.shownThisTurn) {
        return false;
    }

    s.stillTime += dt;
    if (s.stillTime >= threshold) {
        s.shownThisTurn = true;
        s.showTime = duration;
        return true;
    }

    return false;
}

inline bool ribbitVisible(const RibbitState& s) {
    return s.showTime > 0.0f;
}

class BattleSystem {
public:
    explicit BattleSystem(std::unique_ptr<Enemy> enemy,
                          std::string introText = {},
                          bool canFlee = false,
                          std::string fleeBlockedText = {},
                          int furCap = -1);
    BattleSystem(std::vector<std::unique_ptr<Enemy>> enemies,
                 Encounter encounter,
                 std::string introText = {},
                 std::string fleeBlockedText = {},
                 int furCap = -1);

    void update(float dt, GameContext& context);
    void render(GameContext& context);

    bool isFinished() const { return m_state == BattleState::Victory || m_state == BattleState::Fled || m_state == BattleState::Defeat; }
    bool isVictory() const { return m_state == BattleState::Victory; }
    bool isFled() const { return m_state == BattleState::Fled; }
    bool isSpared() const { return m_isSpared; }
    BattleState getState() const { return m_state; }
    Vector2 getSoulPosition() const { return m_soulPosition; }
    const RibbitState& getRibbitState() const { return m_ribbitState; }
    void addDamageZone(const DamageZone& zone) { m_zones.push_back(zone); }
    void setBackground(BattleBackground bg) { m_background = bg; }
    BattleBackground getBackground() const { return m_background; }
    void addResidualDamage(float amount) { m_residual.add(amount); }
    const ResidualMeter& getResidual() const { return m_residual; }
    FightPhase getFightPhase() const { return m_fightPhase; }
    float getFightPhaseTimer() const { return m_fightPhaseTimer; }
    float getFurTrembleTimer() const { return m_furTrembleTimer; }
    bool isFurTrembling() const { return furHudShouldTremble(m_furTrembleTimer); }
    bool isHealNextAttack() const { return m_healNextAttack; }
    // Read-only view of the last FIGHT swing, for the toxic-track overlays (TOX-003/005/006).
    int getLastDamage() const { return m_lastDamage; }
    bool wasLastMiss() const { return m_lastMiss; }
    bool wasLastCrit() const { return m_lastPlayerAction == PlayerTurnAction::FightCrit; }
    std::size_t enemyCount() const { return m_enemies.size(); }
    Enemy* enemyAt(std::size_t index) const {
        return index < m_enemies.size() ? m_enemies[index].get() : nullptr;
    }
    std::string currentEnemyId() const { return m_enemy ? m_enemy->getId() : std::string{}; }
    std::string currentEnemyName() const { return m_enemy ? m_enemy->getName() : std::string{}; }

private:
    void handleMenuInput(GameContext& context);
    void handleEnemySelectInput(GameContext& context);
    void handleFightTargetInput(float dt, GameContext& context);
    void handleActInput(GameContext& context);
    void handleItemInput(GameContext& context);
    void handleMercyInput(GameContext& context);
    std::vector<int> getAvailableActIndices(const GameContext& context) const;
    void startEnemyTurn(GameContext& context);
    void updateEnemyTurn(float dt, GameContext& context);
    void spawnBulletWave();
    void spawnGenericBulletWave(const EnemyPatternDef& pattern);
    const EnemyPatternDef* selectedPattern() const;
    bool finishSurvivalIfComplete(GameContext& context);
    bool finishScriptedBattleIfComplete(GameContext& context, bool atTurnStart);
    int effectiveScriptedEndTurn(const GameContext& context) const;
    bool applySoulDamage(GameContext& context, int rawDamage, int fur);
    void beginTargetAction(MainButton action, GameContext& context);
    void beginFightTarget();
    void selectEnemy(std::size_t index);
    void resolveCurrentEnemy(GameContext& context, EnemyStatus outcome);
    void choosePatternsForActiveEnemies();
    void spawnAggregatedBulletWave();
    std::vector<std::size_t> activeEnemyIndices() const;
    int enemyPositionX(std::size_t index) const;

    void drawBackgroundGrid() const;
    void drawSoul(float x, float y, Color color = SOUL_RED) const;
    void drawSoul(GameContext& context, float x, float y, Color color = SOUL_RED) const;
    void drawHUD(GameContext& context, Font font) const;
    void drawButtons(GameContext& context, Font font) const;
    void spawnDust(Vector2 center, int count, Color col);
    void spawnPlushDust(Vector2 center, int count, bool exGamer);
    static bool readExGamer(const std::string& enemyId);
    void updateDust(float dt);
    void drawDust() const;
    void drawAttackTarget() const;
    void drawAttackTarget(GameContext& context) const;
    void drawBullet(GameContext& context, const Bullet& b) const;

    std::vector<std::unique_ptr<Enemy>> m_enemies;
    Enemy* m_enemy = nullptr;
    EncounterState m_encounterState;
    std::size_t m_selectedEnemyIndex = 0;
    MainButton m_targetAction = MainButton::Fight;
    bool m_selectingEnemy = false;
    std::vector<std::size_t> m_turnAttackers;
    std::vector<std::size_t> m_patternIndices;
    std::vector<int> m_frozenTurnsByEnemy;
    std::vector<int> m_tempAtkDeltasByEnemy;
    BattleState m_state = BattleState::MenuSelect;
    MainButton m_selectedButton = MainButton::Fight;
    int m_subOptionIndex = 0;
    bool m_isSpared = false;
    bool m_targetSparedPending = false;
    bool m_enemyFledPending = false;
    bool m_isBarky = true;
    bool m_canFlee = false;
    std::string m_fleeBlockedText;
    int m_furCap = -1;
    int m_turnCount = 0;
    int m_battleExpEarned = 0;
    int m_battleGoldEarned = 0;

    // Heart inside dodge box
    Vector2 m_soulPosition{ 320.0f, 320.0f };
    float m_soulSpeed = 120.0f; // 4 px/frame at 30 fps (BHL-006)
    float m_invulnTimer = 0.0f;

    // Arena boxes (BUI-001)
    Rectangle m_arenaRect{ (float)BATTLE_BOX_X, (float)BATTLE_BOX_Y, (float)BATTLE_BOX_W, (float)BATTLE_BOX_H };
    Rectangle m_targetBoxRect{ (float)BATTLE_BOX_X, (float)BATTLE_BOX_Y, (float)BATTLE_BOX_W, (float)BATTLE_BOX_H };

    // Fight bar minigame (FGT-001, FGT-002)
    float m_targetCursorX = 100.0f;
    float m_targetCursorSpeed = 400.0f;
    bool m_targetCursorMoving = false;
    FightPhase m_fightPhase = FightPhase::Moving;
    float m_fightPhaseTimer = 0.0f;

    // Enemy turn bullets & timer
    std::vector<Bullet> m_bullets;
    std::vector<DamageZone> m_zones;
    float m_enemyTurnTimer = 0.0f;
    float m_enemyTurnDuration = 5.0f;
    float m_spawnTimer = 0.0f;
    BarkyPattern m_barkyPattern = BarkyPattern::UwuRain;
    bool m_howlUnlocked = false;
    bool m_howlFromLeft = true;
    float m_howlWallTimer = 0.0f;
    std::size_t m_genericPatternIndex = 0;
    int m_patternSpawnCount = 0;
    std::size_t m_patternWordIndex = 0;

    // Messages
    std::string m_narratorText;
    std::string m_enemySpeechBubble;
    bool m_invulnerableDialogBubble = false;
    bool m_dialogReturnsToMenu = false;
    bool m_furStageUpPending = false;

    // Enemy damage feedback (FGT-004, FGT-005, FGT-006, FGT-007)
    float m_enemyShakeTimer = 0.0f;
    float m_damageDisplayTimer = 0.0f;
    float m_slashAnimTimer = 0.0f;
    float m_enemyVisualHp = 0.0f;
    int m_lastDamage = 0;
    bool m_lastMiss = false;

    // Flee animation (MRC-002)
    float m_fleeSoulX = 0.0f;
    float m_fleeSoulY = 0.0f;
    float m_fleeTimer = 0.0f;

    // Dust particles (FGT-007, BND-002, BND-003)
    struct DustParticle {
        Vector2 pos;
        Vector2 vel;
        float alpha;
        float size;
        Color color;
        float fade = 1.4f;
    };
    std::vector<DustParticle> m_dustParticles;
    bool m_exGamer = false;

    // BFL-002 & BFL-003: Turn action tracking & contextual dialogue triggers
    enum class PlayerTurnAction {
        None,
        FightHit,
        FightCrit,
        FightMiss,
        Act,
        Item,
        MercySpareFail
    };
    PlayerTurnAction m_lastPlayerAction = PlayerTurnAction::None;
    std::string m_lastActionDetail;
    std::string m_lastActResponse;
    bool m_furReactionSaid = false;

    // ACT-006/007: Per-battle ACT use counts and queued skipped enemy attacks
    std::unordered_map<std::string, int> m_actUseCounts;
    int m_frozenTurns = 0;
    bool m_healNextAttack = false;
    int m_tempAtkDelta = 0;

    // BUI-010: Enemy idle animation
    float m_enemyIdleTimer = 0.0f;

    // ITM-005: Mountain Dew speed buff & soul trail
    int m_soulSpeedBuffTurns = 0;
    struct SoulTrail {
        Vector2 pos;
        float alpha;
    };
    std::vector<SoulTrail> m_soulTrails;
    void updateSoulTrails(float dt);
    void drawSoulTrails() const;

    // BHL-015: "thinking" ribbit text over soul
    RibbitState m_ribbitState;

    // BUI-013: Per-encounter battle background
    BattleBackground m_background = BattleBackground::Grid;
    float m_backgroundAnimTimer = 0.0f;

    // BHL-017: KR-style residual damage
    ResidualMeter m_residual;

    // FUR-003: KR-style FUR HUD state
    float m_furTrembleTimer = -1.0f;
    bool m_furTrembleTriggered = false;
    bool m_furVisibleAtStart = false;
    bool m_furStartChecked = false;

    // BSK (C7): Bass Shake beat turn
    bool beatPatternSelected() const;
    void startBeatTurn(GameContext& context);
    bool updateBeatTurn(float dt, GameContext& context); // returns true when the turn is over
    void endBeatTurn(GameContext& context);
    // C1-028: the scripted last turn of a beat fight ends on a drop (spray + record scratch)
    bool beatFinaleTurn(const GameContext& context) const;
    void finishBeatFinale(GameContext& context);
    void spawnBeatTuft(const EnemyPatternDef& pattern, const BeatEvent& event);
    BeatMap m_beatMap;
    bool m_beatMapLoaded = false;
    bool m_beatTurnActive = false;
    bool m_beatFinaleActive = false; // this beat turn is the scripted last one
    BeatTurnClock m_beatClock;
    double m_beatPrevTime = 0.0;
    double m_beatTurnTime = 0.0;
    bool m_beatStopped = false;      // stopsBeat ACT used
    bool m_musicDucked = false;

    // Context reference during render for sub-draw helpers
    mutable GameContext* m_currentContext = nullptr;
};

} // namespace nundertale
