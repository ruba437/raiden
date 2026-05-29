#ifndef MOUNTED_TANK_ENEMY_HPP
#define MOUNTED_TANK_ENEMY_HPP

#include "Enemy.hpp"
#include <cmath>
#include <vector>

// 類別名稱改為 MountedTankEnemy
class MountedTankEnemy : public Enemy {
public:
    MountedTankEnemy(const glm::vec2& startPosition)
        : Enemy(startPosition, RESOURCE_DIR "/Image/enemy/tank.png", 5) {

        m_CanShoot = true;
        m_ShootTimer = 90.0f;

        // 圖層設為 9，蓋在 Carrier (8) 的上面
        m_ZIndex = 9;
    }

    void Update(const glm::vec2& playerPos) override {
        // 跟隨 Carrier (地圖) 移動，速度設為一樣的 -0.5f
        m_Transform.translation.y -= 1.0f;

        // 隨時瞄準玩家
        float angle = std::atan2(playerPos.y - m_Transform.translation.y,
                                 playerPos.x - m_Transform.translation.x);
        m_Transform.rotation = angle + 1.570796f;

        Enemy::Update(playerPos);
    }

    std::vector<EnemyBulletData> GetBulletData(const glm::vec2& playerPos) const override {
        float speed = 8.0f;
        float actualAngle = m_Transform.rotation - 1.570796f;
        glm::vec2 velocity = { std::cos(actualAngle) * speed, std::sin(actualAngle) * speed };

        return {
                { glm::vec2(0, 0), velocity, RESOURCE_DIR "/Image/bullet/yellow.png" }
        };
    }

    void ResetShootTimer() override {
        m_ShootTimer = 120.0f;
    }
};

#endif