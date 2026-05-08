#ifndef STRAFER_ENEMY_HPP
#define STRAFER_ENEMY_HPP

#include "Enemy.hpp"

class StraferEnemy : public Enemy {
private:
    enum class State { ENTERING, STRAFING };

    State m_State = State::ENTERING;
    float m_TargetY = 250.0f;
    float m_SpeedX = 2.0f;
    int m_MoveDirection = 1;
    float m_FireCooldown = 120.0f;
    float m_FireTimer = 0.0f;

public:
    explicit StraferEnemy(const glm::vec2& startPos)
        : Enemy(startPos, RESOURCE_DIR "/Image/enemy/strafer.png", 15) {
        m_Transform.translation = startPos;
        m_CanShoot = true;
    }

    void Update(const glm::vec2& playerPos) override {
        if (m_State == State::ENTERING) {
            m_Transform.translation.y -= 2.0f;
            if (m_Transform.translation.y <= m_TargetY) {
                m_Transform.translation.y = m_TargetY;
                m_State = State::STRAFING;
            }
        } else if (m_State == State::STRAFING) {
            m_Transform.translation.x += m_SpeedX * static_cast<float>(m_MoveDirection);

            if (m_Transform.translation.x > 200.0f) {
                m_Transform.translation.x = 200.0f;
                m_MoveDirection = -1;
            } else if (m_Transform.translation.x < -200.0f) {
                m_Transform.translation.x = -200.0f;
                m_MoveDirection = 1;
            }

            m_FireTimer += 1.0f;
        }

        Enemy::Update(playerPos);
    }

    bool ReadyToShoot() const override {
        return m_State == State::STRAFING && m_FireTimer >= m_FireCooldown;
    }

    std::vector<EnemyBulletData> GetBulletData(const glm::vec2& /*playerPos*/) const override {
        return {
            { glm::vec2(-75.0f, 0.0f), glm::vec2(0.0f, -5.0f) },
            { glm::vec2(-25.0f, 0.0f), glm::vec2(0.0f, -5.0f) },
            { glm::vec2(25.0f, 0.0f), glm::vec2(0.0f, -5.0f) },
            { glm::vec2(75.0f, 0.0f), glm::vec2(0.0f, -5.0f) }
        };
    }

    void ResetShootTimer() override {
        m_FireTimer = 0.0f;
    }
};

#endif

