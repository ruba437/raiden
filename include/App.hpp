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

class App {
public:
    enum class State {
        START,
        UPDATE,
        END,
    };

    State GetCurrentState() const { return m_CurrentState; }

    void Start();

    void Update();

    void End(); // NOLINT(readability-convert-member-functions-to-static)

private:
    void ValidTask();

private:
    State m_CurrentState = State::START;
    std::shared_ptr<Player> m_Player;
    std::vector<std::shared_ptr<Bullet>> m_Bullets;
    std::shared_ptr<Background> m_Bg1;
    std::shared_ptr<Background> m_Bg2;

    std::vector<std::shared_ptr<Enemy>> m_Enemies;

    struct EnemySpawnData {
        float spawnTime;    // 觸發時間 (以遊戲經過的幀數計算，例如 60 = 1秒)
        glm::vec2 position; // 生成的初始座標
        // (未來如果有不同種類的敵人，也可以把 Type 或 HP 寫進這裡！)
    };

    // --- 2. 新增關卡相關變數 ---
    float m_LevelTimer = 0.0f;                 // 關卡總計時器 (從 0 開始往上加)
    std::vector<EnemySpawnData> m_LevelEvents; // 存放整關所有敵人的清單
    size_t m_CurrentEventIndex = 0;            // 記錄目前已經生成到第幾個事件了

    std::vector<std::shared_ptr<Item>> m_Items;
    std::vector<std::shared_ptr<Bullet>> m_EnemyBullets;

    int m_Score = 0;
    std::shared_ptr<ScoreUI> m_ScoreUI;
    float m_PlayerShootTimer = 0.0f;
    float m_MissileShootTimer = 0.0f;
};

#endif
