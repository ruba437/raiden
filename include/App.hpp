#ifndef APP_HPP
#define APP_HPP

#include "pch.hpp" // IWYU pragma: export
#include "Player.hpp"
#include "Bullet.hpp"
#include "Background.hpp"
#include "Enemy.hpp"
#include "Item.hpp"
#include "Score.hpp"
#include <memory>
#include <vector>
#include "Deck.hpp"
#include "BombEffect.hpp"
#include "MenuBackground.hpp"
#include "EndFrame.hpp"

class App {
public:
    enum class State {
        START,
        MENU,
        INTRO,
        UPDATE,
        LEVELEND,
        END,
    };

    State GetCurrentState() const { return m_CurrentState; }

    void Start();
    void UpdateMenu();
    void UpdateIntro();
    void Update();
    void UpdateEnd();
    void End();

private:
    void ValidTask();
    void LoadLevel(int levelNum);

private:
    State m_CurrentState = State::START;
    std::shared_ptr<Player> m_Player;
    std::vector<std::shared_ptr<Bullet>> m_Bullets;
    std::shared_ptr<Background> m_Bg1;
    std::shared_ptr<Background> m_Bg2;
    std::vector<std::shared_ptr<Enemy>> m_Enemies;
    std::shared_ptr<Deck> m_Deck;       // 甲板物件
    float m_IntroTimer = 0.0f;          // 進場計時器
    const float m_IntroDuration = 90.0f; // 進場動畫總幀數 (假設 60FPS，90幀=1.5秒)
    std::vector<std::shared_ptr<BombEffect>> m_Effects;

    enum class EnemyType {
        ASSAULT, // 突襲型 (U字折返)
        SPREAD,   // 散彈型 (停留上方射擊三叉戟)
        TANK,
        PHANTOM,
        TURRET,
        BOSS
        // 未來有 SNIPER 或 BOSS 都可以直接加在這裡
    };

    struct EnemySpawnData {
        float spawnTime;    // 觸發時間
        glm::vec2 position; // 生成座標
        EnemyType type;     // 生成的敵人種類
        bool randomX = false;
    };

    // --- 2. 新增關卡相關變數 ---
    float m_LevelTimer = 0.0f;                 // 關卡總計時器 (從 0 開始往上加)
    std::vector<EnemySpawnData> m_LevelEvents; // 存放整關所有敵人的清單
    size_t m_CurrentEventIndex = 0;            // 記錄目前已經生成到第幾個事件了

    std::vector<std::shared_ptr<Item>> m_Items;
    std::vector<std::shared_ptr<Bullet>> m_EnemyBullets;

    int m_Score = 0;
    std::shared_ptr<ScoreUI> m_ScoreUI;
    std::shared_ptr<HpUI> m_HpUI;
    std::shared_ptr<BombUI> m_BombUI;
    float m_PlayerShootTimer = 0.0f;
    float m_MissileShootTimer = 0.0f;
    float m_BombCooldownTimer = 0.0f;
    std::shared_ptr<MenuBackground> m_MenuBg;
    int m_CurrentLevel = 1;
    std::shared_ptr<EndFrame> m_EndFrame;
};

#endif
