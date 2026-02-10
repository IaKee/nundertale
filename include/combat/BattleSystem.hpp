#pragma once

#include <raylib.h>
#include <vector>
#include <memory>
#include "Bullet.hpp"
#include "Enemy.hpp"

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
    BattleSystem(std::unique_ptr<Enemy> enemy);

    void update(float dt, GameContext& context);
    void render(GameContext& context);

    bool isFinished() const { return m_state == BattleState::Victory || m_state == BattleState::Defeat; }
    bool isVictory() const { return m_state == BattleState::Victory; }

private:
    void handleMenuInput(GameContext& context);
    void handleFightTargetInput(GameContext& context);
    void handleActInput(GameContext& context);
    void handleItemInput(GameContext& context);
    void handleMercyInput(GameContext& context);
    void startEnemyTurn();
    void updateEnemyTurn(float dt, GameContext& context);
    void spawnBulletWave();

    std::unique_ptr<Enemy> m_enemy;
    BattleState m_state = BattleState::MenuSelect;
    MainButton m_selectedButton = MainButton::Fight;
    int m_subOptionIndex = 0;

    // Heart inside dodge box
    Vector2 m_soulPosition{ 320, 300 };
    float m_soulSpeed = 160.0f;
    float m_invulnTimer = 0.0f;

    // Dodge Arena box
    Rectangle m_arenaRect{ 220, 240, 200, 130 };

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
