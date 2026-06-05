#ifndef BOSS3_ENEMY_HPP
#define BOSS3_ENEMY_HPP

#include "Enemy.hpp"
#include <cmath>
#include <vector>

class Boss3Enemy : public Enemy {
private:
    int m_AttackMode = 0;
    int m_BurstCount = 10;

public:
    Boss3Enemy(const glm::vec2& startPosition)
        : Enemy(startPosition, RESOURCE_DIR "/Image/enemy/boss3.png", 200) {

        m_CanShoot = true;
        m_ShootTimer = 60.0f;
        m_ZIndex = 8;
    }

    void Update(const glm::vec2& playerPos) override {
        if (m_Transform.translation.y > 200.0f) {
            m_Transform.translation.y -= 1.0f;
        }
        m_Transform.rotation = 3.14159265f;
        Enemy::Update(playerPos);
    }

    std::vector<EnemyBulletData> GetBulletData(const glm::vec2& playerPos) const override {
        std::vector<EnemyBulletData> bullets;

        if (m_AttackMode == 0) {
            // ==========================================
            // 【模式一】中央機槍：並排的黃色子彈
            // ==========================================
            float speed = 6.0f;
            glm::vec2 leftMuzzle = {-20.0f, -50.0f};
            glm::vec2 rightMuzzle = {20.0f, -50.0f};

            bullets.push_back({ leftMuzzle, {0.0f, -speed}, RESOURCE_DIR "/Image/bullet/yellow.png" });
            bullets.push_back({ rightMuzzle, {0.0f, -speed}, RESOURCE_DIR "/Image/bullet/yellow.png" });
        }
        else if (m_AttackMode == 1) {
            // ==========================================
            // 【模式二】雙翼散射：藍色子彈
            // ==========================================
            float speed = 4.5f;
            glm::vec2 leftWing = {-80.0f, 0.0f};
            glm::vec2 rightWing = {80.0f, 0.0f};

            float anglesLeft[] = {-140.0f, -120.0f, -100.0f};
            for (float ang : anglesLeft) {
                float rad = ang * 3.14159265f / 180.0f;
                bullets.push_back({ leftWing, {std::cos(rad) * speed, std::sin(rad) * speed}, RESOURCE_DIR "/Image/bullet/blue.png" });
            }

            float anglesRight[] = {-80.0f, -60.0f, -40.0f};
            for (float ang : anglesRight) {
                float rad = ang * 3.14159265f / 180.0f;
                bullets.push_back({ rightWing, {std::cos(rad) * speed, std::sin(rad) * speed}, RESOURCE_DIR "/Image/bullet/blue.png" });
            }
        }
        else if (m_AttackMode == 2) {
            // ==========================================
            // 【模式三】大範圍火網：紅色扇形子彈
            // ==========================================
            float speed = 5.0f;
            glm::vec2 centerMuzzle = {0.0f, -50.0f};

            for (int i = 0; i < 7; ++i) {
                float ang = -150.0f + i * 20.0f;
                float rad = ang * 3.14159265f / 180.0f;
                bullets.push_back({ centerMuzzle, {std::cos(rad) * speed, std::sin(rad) * speed}, RESOURCE_DIR "/Image/bullet/blue.png" });
            }
        }
        else if (m_AttackMode == 3) {
            // ==========================================
            // 🌟【模式四】新增！精準狙擊：綠色三連裝追蹤散彈
            // ==========================================
            float speed = 7.0f; // 速度特別快！
            glm::vec2 centerMuzzle = {0.0f, -30.0f};
            glm::vec2 actualPos = m_Transform.translation + centerMuzzle;

            // 計算指向玩家的精準角度
            float targetAngle = std::atan2(playerPos.y - actualPos.y, playerPos.x - actualPos.x);

            // 朝玩家方向發射 3 顆 (角度偏移約為 -12度, 0度, +12度)
            float offsets[] = {-0.2f, 0.0f, 0.2f};
            for (float offset : offsets) {
                float rad = targetAngle + offset;
                bullets.push_back({ centerMuzzle, {std::cos(rad) * speed, std::sin(rad) * speed}, RESOURCE_DIR "/Image/bullet/yellow.png" });
            }
        }

        return bullets;
    }

    void ResetShootTimer() override {
        if (m_BurstCount > 0) {
            // 控制各自模式的連射間隔
            if (m_AttackMode == 0) m_ShootTimer = 8.0f;
            else if (m_AttackMode == 1) m_ShootTimer = 15.0f;
            else if (m_AttackMode == 2) m_ShootTimer = 25.0f;
            else m_ShootTimer = 12.0f;  // 🌟 模式四的射速

            m_BurstCount--;
        } else {
            // 🌟 1. 增加到 4 種模式切換
            m_AttackMode = (m_AttackMode + 1) % 4;

            // 🌟 2. 大幅縮短切換攻擊時的空窗期 (從 90.0f 改為 40.0f，約 0.6 秒)
            m_ShootTimer = 40.0f;

            // 裝填下一個模式的攻擊次數
            if (m_AttackMode == 0) m_BurstCount = 10;
            else if (m_AttackMode == 1) m_BurstCount = 5;
            else if (m_AttackMode == 2) m_BurstCount = 3;
            else m_BurstCount = 6;  // 🌟 模式四連續狙擊 6 次
        }
    }
};

#endif