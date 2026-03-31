#ifndef SPREAD_ENEMY_HPP
#define SPREAD_ENEMY_HPP

#include "Enemy.hpp"
#include <cmath> // --- 確保有引入 cmath ---

class SpreadEnemy : public Enemy {
private:
    bool m_IsStopped = false;

public:
    SpreadEnemy(const glm::vec2& startPosition)
        : Enemy(startPosition, RESOURCE_DIR "/Image/enemy/spread.png", 15) {
        m_CanShoot = false;
    }

    void Update(const glm::vec2& playerPos) override {
        if (!m_IsStopped) {
            m_Transform.translation.y -= 3.0f;
            if (m_Transform.translation.y <= 200.0f) {
                m_IsStopped = true;
                m_CanShoot = true;
            }
        }
        Enemy::Update(playerPos);
    }

    // --- 修改：實作自機狙擊散彈數學運算 ---
    std::vector<glm::vec2> GetBulletVelocities(const glm::vec2& playerPos) const override {

        glm::vec2 currentPos = GetPosition();

        // 1. 計算敵機到玩家的相對距離 (dx, dy)
        float dx = playerPos.x - currentPos.x;
        float dy = playerPos.y - currentPos.y;

        // 2. 計算基礎瞄準角度 (弧度)
        float baseAngle = std::atan2(dy, dx);

        // 3. 設定子彈速度與散開的角度大小
        float speed = 8.0f;
        float spreadAngle = 0.25f; // 0.25 弧度大約是 14.3 度

        // 4. 利用 cos 和 sin 將角度轉回 X 軸與 Y 軸的速度向量
        // 中間子彈：精準瞄準玩家
        glm::vec2 centerVel(std::cos(baseAngle) * speed, std::sin(baseAngle) * speed);

        // 左側子彈：基礎角度 - 散開角度
        glm::vec2 leftVel(std::cos(baseAngle - spreadAngle) * speed, std::sin(baseAngle - spreadAngle) * speed);

        // 右側子彈：基礎角度 + 散開角度
        glm::vec2 rightVel(std::cos(baseAngle + spreadAngle) * speed, std::sin(baseAngle + spreadAngle) * speed);

        // 回傳這三顆子彈的速度
        return { centerVel, leftVel, rightVel };
    }

    void ResetShootTimer() override { 
        m_ShootTimer = 80.0f + (std::rand() % 40); 
    }
};

#endif