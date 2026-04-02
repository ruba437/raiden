#ifndef TANK_ENEMY_HPP
#define TANK_ENEMY_HPP

#include "Enemy.hpp"
#include <cmath>

class TankEnemy : public Enemy {
private:
    glm::vec2 m_Velocity; // 坦克的移動方向與速度

public:
    TankEnemy(const glm::vec2& startPosition)
        : Enemy(startPosition, RESOURCE_DIR "/Image/enemy/tank.png", 5) {

        m_CanShoot = true;

        // 1. 決定移動方向
        if (std::abs(startPosition.x) > std::abs(startPosition.y)) {
            m_Velocity = glm::vec2((startPosition.x < 0) ? 2.5f : -2.5f, 0.0f);
        } else {
            m_Velocity = glm::vec2(0.0f, (startPosition.y < 0) ? 2.5f : -2.5f);
        }

        // --- 2. 新增：根據移動方向設定圖片的旋轉角度 ---
        // 直接拿 m_Velocity 的 y 和 x 來計算 atan2
        float angleRad = std::atan2(m_Velocity.y, m_Velocity.x);

        // 減去 90 度的弧度 (1.570796f)，讓原本朝上的機頭轉向移動方向
        m_Transform.rotation = angleRad - 1.570796f;
    }

    void Update(const glm::vec2& playerPos) override {
        // --- 修改：只保留直線移動，移除原本的旋轉邏輯 ---
        m_Transform.translation += m_Velocity;

        Enemy::Update(playerPos);
    }

    // 發射單發追蹤彈 (子彈依然會瞄準玩家)
    std::vector<glm::vec2> GetBulletVelocities(const glm::vec2& playerPos) const override {
        glm::vec2 currentPos = GetPosition();
        float dx = playerPos.x - currentPos.x;
        float dy = playerPos.y - currentPos.y;
        float baseAngle = std::atan2(dy, dx);

        float speed = 6.0f;

        return { glm::vec2(std::cos(baseAngle) * speed, std::sin(baseAngle) * speed) };
    }

    void ResetShootTimer() override { 
        m_ShootTimer = 100.0f + (std::rand() % 40); 
    }
};

#endif