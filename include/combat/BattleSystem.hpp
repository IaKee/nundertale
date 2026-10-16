#pragma once

#include <raylib.h>
#include <vector>
#include <memory>
#include <string>
#include "Bullet.hpp"
#include "Enemy.hpp"
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
    Victory,
    Defeat
};

enum class MainButton {
    Fight = 0,
    Act = 1,
    Item = 2,
    Mercy = 3
};

class BattleSystem {
public:
    explicit BattleSystem(std::unique_ptr<Enemy> enemy);

    void update(float dt, GameContext& context);
    void render(GameContext& context);

    bool isFinished() const { return m_state == BattleState::Victory || m_state == BattleState::Defeat; }
    bool isVictory() const { return m_state == BattleState::Victory; }
    bool isSpared() const { return m_isSpared; }

private:
    void handleMenuInput(GameContext& context);
    void handleFightTargetInput(float dt, GameContext& context);
    void handleActInput(GameContext& context);
    void handleItemInput(GameContext& context);
    void handleMercyInput(GameContext& context);
    void startEnemyTurn();
    void updateEnemyTurn(float dt, GameContext& context);
    void spawnBulletWave();

    void drawBackgroundGrid() const;
    void drawSoul(float x, float y, Color color = SOUL_RED) const;
    void drawHUD(GameContext& context, Font font) const;
    void drawButtons(GameContext& context, Font font) const;

    std::unique_ptr<Enemy> m_enemy;
    BattleState m_state = BattleState::MenuSelect;
    MainButton m_selectedButton = MainButton::Fight;
    int m_subOptionIndex = 0;
    bool m_isSpared = false;

    // Heart inside dodge box
    Vector2 m_soulPosition{ 320.0f, 320.0f };
    float m_soulSpeed = 120.0f; // 4 px/frame at 30 fps (BHL-006)
    float m_invulnTimer = 0.0f;

    // Arena boxes (BUI-001)
    Rectangle m_arenaRect{ (float)BATTLE_BOX_X, (float)BATTLE_BOX_Y, (float)BATTLE_BOX_W, (float)BATTLE_BOX_H };
    Rectangle m_targetBoxRect{ (float)BATTLE_BOX_X, (float)BATTLE_BOX_Y, (float)BATTLE_BOX_W, (float)BATTLE_BOX_H };

    // Fight bar minigame
    float m_targetCursorX = 100.0f;
    float m_targetCursorSpeed = 400.0f;
    bool m_targetCursorMoving = false;

    // Enemy turn bullets & timer
    std::vector<Bullet> m_bullets;
    float m_enemyTurnTimer = 0.0f;
    float m_enemyTurnDuration = 5.0f;
    float m_spawnTimer = 0.0f;

    // Messages
    std::string m_narratorText;
    std::string m_enemySpeechBubble;
};

} // namespace nundertale
