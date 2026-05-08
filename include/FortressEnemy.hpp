#ifndef FORTRESS_ENEMY_HPP
#define FORTRESS_ENEMY_HPP

#include "Enemy.hpp"
#include <cmath>

class FortressEnemy : public Enemy {
private:
    float m_MapScrollSpeed = 1.0f;
    float m_MoveUpTimer = 0.0f;
    bool m_IsMovingUp = false;
    float m_UpwardDistanceLeft = 0.0f;
    float m_AttackTimer = 0.0f;
    int m_AttackType = 0;
    int m_BurstCount = 0;
    float m_BurstTimer = 0.0f;
    std::vector<EnemyBulletData> m_PendingBullets;

public:
    explicit FortressEnemy(const glm::vec2& startPos)
        : Enemy(startPos, RESOURCE_DIR "/Image/enemy/fortress.png", 15) {
        m_Transform.translation = startPos;
        m_CanShoot = true;
    }

    void Update(const glm::vec2& playerPos) override {
        // 平時跟著背景向下移動
        m_Transform.translation.y -= m_MapScrollSpeed;

        m_MoveUpTimer += 1.0f;
        if (m_MoveUpTimer >= 300.0f && !m_IsMovingUp) {
            m_IsMovingUp = true;
            m_UpwardDistanceLeft = 300.0f;
            m_MoveUpTimer = 0.0f;
        }

        if (m_IsMovingUp) {
            m_Transform.translation.y += 3.0f;
            m_UpwardDistanceLeft -= 3.0f;
            if (m_UpwardDistanceLeft <= 0.0f) {
                m_IsMovingUp = false;
            }
        }

        // 攻擊邏輯開始前先鎖定玩家當前位置
        float targetAngle = std::atan2(playerPos.y - m_Transform.translation.y,
                                       playerPos.x - m_Transform.translation.x);

        // 連發模式：每 10 幀補兩顆追蹤子彈，直到波次打完
        if (m_BurstCount > 0) {
            m_BurstTimer += 0.7f;
            if (m_BurstTimer >= 10.0f) {
                glm::vec2 forwardVel = {
                    std::cos(targetAngle) * 6.0f,
                    std::sin(targetAngle) * 6.0f
                };
                glm::vec2 perpDir = {
                    -std::sin(targetAngle),
                    std::cos(targetAngle)
                };

                // EnemyBulletData 使用相對偏移，因此這裡以砲口偏移量表達左右雙排
                m_PendingBullets.push_back({perpDir * 20.0f, forwardVel, RESOURCE_DIR "/Image/bullet/blue.png"});
                m_PendingBullets.push_back({-perpDir * 20.0f, forwardVel, RESOURCE_DIR "/Image/bullet/blue.png"});
                m_BurstCount--;
                m_BurstTimer = 0.0f;
            }
        } else {
            m_AttackTimer += 1.0f;
            if (m_AttackTimer >= 180.0f) {
                if (m_AttackType == 0) {
                    m_BurstCount = 7;
                    m_BurstTimer = 0.0f;
                    m_AttackType = 1;
                } else {
                    for (int i = -2; i <= 2; ++i) {
                        float angle = targetAngle + static_cast<float>(i) * 0.261799f;
                        m_PendingBullets.push_back({
                            glm::vec2(0.0f, 0.0f),
                            glm::vec2(std::cos(angle) * 5.0f, std::sin(angle) * 5.0f),
                            RESOURCE_DIR "/Image/bullet/blue.png"
                        });
                    }
                    m_AttackType = 0;
                }
                m_AttackTimer = 0.0f;
            }
        }

        Enemy::Update(playerPos);
    }

    bool ReadyToShoot() const override {
        return m_CanShoot && !m_PendingBullets.empty();
    }

    std::vector<EnemyBulletData> GetBulletData(const glm::vec2& /*playerPos*/) const override {
        return m_PendingBullets;
    }

    void ResetShootTimer() override {
        m_PendingBullets.clear();
    }
};

#endif

