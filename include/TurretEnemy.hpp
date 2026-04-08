#ifndef TURRET_ENEMY_HPP
#define TURRET_ENEMY_HPP

#include "Enemy.hpp"
#include <cmath>

class TurretEnemy : public Enemy {
public:
    // 砲塔通常比較堅固，給它稍微多一點血量 (例如 8)
    TurretEnemy(const glm::vec2& startPosition)
        : Enemy(startPosition, RESOURCE_DIR "/Image/enemy/turret.png", 5) {

        m_CanShoot = true; // 出生就可以準備開火
    }

    void Update(const glm::vec2& playerPos) override {
        // 跟背景移動速動相同
        m_Transform.translation.y -= 1.0f;

        // --- 2. 旋轉面向玩家 (砲塔瞄準) ---
        glm::vec2 currentPos = GetPosition();
        float dx = playerPos.x - currentPos.x;
        float dy = playerPos.y - currentPos.y;

        // 假設圖片頭部預設朝上，減去 1.570796f (90度) 來正確轉向
        m_Transform.rotation = std::atan2(dy, dx) - 1.570796f;

        // 3. 呼叫父類別以處理射擊計時器
        Enemy::Update(playerPos);
    }

    // 發射單發精準的自機狙擊彈
    std::vector<EnemyBulletData> GetBulletData(const glm::vec2& playerPos) const override {
        glm::vec2 currentPos = GetPosition();
        float dx = playerPos.x - currentPos.x;
        float dy = playerPos.y - currentPos.y;
        float baseAngle = std::atan2(dy, dx);

        float speed = 6.0f; // 子彈速度

        return {
                { glm::vec2(0, 0), glm::vec2(std::cos(baseAngle) * speed, std::sin(baseAngle) * speed) }
        };
    }

    // 開火頻率：因為是固定砲台，可以設得有節奏一點，例如每 1.5 ~ 2 秒一發
    void ResetShootTimer() override {
        m_ShootTimer = 90.0f + (std::rand() % 30);
    }
};

#endif