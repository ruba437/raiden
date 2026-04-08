#ifndef SPREAD_ENEMY_HPP
#define SPREAD_ENEMY_HPP

#include "Enemy.hpp"
#include <cmath>

class SpreadEnemy : public Enemy {
private:
    bool m_IsStopped = false;
    bool m_IsRetreating = false; // 新增：是否正在撤退
    float m_AttackTimer = 0.0f;  // 新增：攻擊持續時間計時器

public:
    SpreadEnemy(const glm::vec2& startPosition)
        : Enemy(startPosition, RESOURCE_DIR "/Image/enemy/spread.png", 5) {
        m_CanShoot = false;
    }

    void Update(const glm::vec2& playerPos) override {

        if (m_IsRetreating) {
            // ==========================================
            // 狀態 3：向上移動離開地圖
            // ==========================================
            m_Transform.translation.y += 4.0f; // 撤退速度

        } else if (m_IsStopped) {
            // ==========================================
            // 狀態 2：停留在原地攻擊，並計時
            // ==========================================
            m_AttackTimer += 1.0f; // 每一幀增加計時

            // 8 秒後開始撤退
            if (m_AttackTimer >= 480.0f) {
                m_IsRetreating = true;
                m_CanShoot = false; // 撤退時停止攻擊 (如果你想邊跑邊射，可以設為 true)
            }
        } else {
            // ==========================================
            // 狀態 1：進場下降
            // ==========================================
            m_Transform.translation.y -= 3.0f;
            if (m_Transform.translation.y <= 200.0f) {
                m_IsStopped = true;
                m_CanShoot = true;
            }
        }

        Enemy::Update(playerPos);
    }

    // 攻擊邏輯保持不變：永遠朝向玩家的三叉戟散彈
    std::vector<EnemyBulletData> GetBulletData(const glm::vec2& playerPos) const override {
        glm::vec2 currentPos = GetPosition();
        float dx = playerPos.x - currentPos.x;
        float dy = playerPos.y - currentPos.y;
        float baseAngle = std::atan2(dy, dx);

        float speed = 8.0f;
        float spreadAngle = 0.25f;

        // 將原本的速度向量封裝進 EnemyBulletData 結構中
        // 因為這種類型不需要位置偏移，所以 posOffset 都設為 (0, 0)
        return {
                { glm::vec2(0, 0), glm::vec2(std::cos(baseAngle) * speed, std::sin(baseAngle) * speed) },
                { glm::vec2(0, 0), glm::vec2(std::cos(baseAngle - spreadAngle) * speed, std::sin(baseAngle - spreadAngle) * speed) },
                { glm::vec2(0, 0), glm::vec2(std::cos(baseAngle + spreadAngle) * speed, std::sin(baseAngle + spreadAngle) * speed) }
        };
    }

    void ResetShootTimer() override { 
        m_ShootTimer = 80.0f + (std::rand() % 40); 
    }
};

#endif