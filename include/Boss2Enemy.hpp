#ifndef BOSS2_ENEMY_HPP
#define BOSS2_ENEMY_HPP

#include "Enemy.hpp"
#include "DashEnemy.hpp"
#include <cmath>
#include <cstdlib>

class Boss2Enemy : public Enemy {
private:
    enum class State { SCATTER, SUMMON };

    State m_State = State::SCATTER;
    int m_ActionCount = 0;
    float m_ActionTimer = 0.0f;
    float m_ScatterAngle = 0.0f;
    int m_ScatterColor = 0;
    float m_ScatterFireTimer = 0.0f;
    float m_SpinDirection = 1.0f;
    std::vector<std::shared_ptr<Enemy>> m_SpawnedEnemies;
    std::vector<EnemyBulletData> m_PendingBullets;

public:
    explicit Boss2Enemy(const glm::vec2& startPos)
        : Enemy(startPos, RESOURCE_DIR "/Image/enemy/boss2.png", 150) {
        m_Transform.translation = startPos;
        m_CanShoot = true;
    }

    std::vector<std::shared_ptr<Enemy>> GetAndClearSpawnedEnemies() {
        std::vector<std::shared_ptr<Enemy>> spawned = m_SpawnedEnemies;
        m_SpawnedEnemies.clear();
        return spawned;
    }

    void Update(const glm::vec2& playerPos) override {
        // 先進場到畫面上方固定位置
        if (m_Transform.translation.y > 200.0f) {
            m_Transform.translation.y -= 2.0f;
            if (m_Transform.translation.y < 200.0f) {
                m_Transform.translation.y = 200.0f;
            }
            Enemy::Update(playerPos);
            return;
        }

        if (m_State == State::SCATTER) {
            m_ActionTimer += 1.0f;
            m_ScatterFireTimer += 1.0f;

            // 雙螺旋：每 3 幀發射 2 顆 180 度對稱的子彈
            if (m_ScatterFireTimer >= 3.0f) {
                m_ScatterFireTimer = 0.0f;

                const float pi = 3.14159f;
                const std::string bulletPath = (m_ScatterColor == 0)
                                                   ? RESOURCE_DIR "/Image/bullet/yellow.png"
                                                   : RESOURCE_DIR "/Image/bullet/green.png";

                float randomOffset = ((static_cast<float>(std::rand() % 100) / 100.0f) - 0.5f) * 0.6f;
                float actualAngle1 = m_ScatterAngle + randomOffset;
                float actualAngle2 = m_ScatterAngle + pi + randomOffset;

                glm::vec2 vel1 = {
                    std::cos(actualAngle1) * 5.0f,
                    std::sin(actualAngle1) * 5.0f
                };
                glm::vec2 vel2 = {
                    std::cos(actualAngle2) * 5.0f,
                    std::sin(actualAngle2) * 5.0f
                };

                m_PendingBullets.push_back({glm::vec2(0.0f, 0.0f), vel1, bulletPath});
                m_PendingBullets.push_back({glm::vec2(0.0f, 0.0f), vel2, bulletPath});

                m_ScatterAngle += 0.25f * m_SpinDirection;
            }

            // 每 90 幀視為一個波次，切色並反向旋轉
            if (m_ActionTimer >= 90.0f) {
                m_ActionTimer = 0.0f;
                m_ActionCount++;
                m_ScatterColor = (m_ScatterColor == 0) ? 1 : 0;
                m_SpinDirection = -m_SpinDirection;
            }
        } else if (m_State == State::SUMMON) {
            m_ActionTimer += 1.0f;
            if (m_ActionTimer >= 90.0f) {
                glm::vec2 bossPos = m_Transform.translation;
                m_SpawnedEnemies.push_back(std::make_shared<DashEnemy>(bossPos, glm::vec2(-12.0f, 2.0f)));
                m_SpawnedEnemies.push_back(std::make_shared<DashEnemy>(bossPos, glm::vec2(-6.0f, 1.0f)));
                m_SpawnedEnemies.push_back(std::make_shared<DashEnemy>(bossPos, glm::vec2(6.0f, 1.0f)));
                m_SpawnedEnemies.push_back(std::make_shared<DashEnemy>(bossPos, glm::vec2(12.0f, 2.0f)));
                m_SpawnedEnemies.push_back(std::make_shared<DashEnemy>(bossPos, glm::vec2(18.0f, 3.0f)));
                m_SpawnedEnemies.push_back(std::make_shared<DashEnemy>(bossPos, glm::vec2(-18.0f, 3.0f)));

                m_ActionTimer = 0.0f;
                m_ActionCount++;
            }
        }

        if (m_ActionCount >= 10) {
            m_ActionCount = 0;
            m_ActionTimer = 0.0f;
            m_ScatterFireTimer = 0.0f;
            m_State = (m_State == State::SCATTER) ? State::SUMMON : State::SCATTER;
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

