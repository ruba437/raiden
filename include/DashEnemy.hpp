#ifndef DASH_ENEMY_HPP
#define DASH_ENEMY_HPP

#include "Enemy.hpp"
#include <cmath>

class DashEnemy : public Enemy {
private:
    enum class State { EJECTING, AIMING, DASHING };

    State m_State = State::EJECTING;
    float m_AimTimer = 0.0f;
    glm::vec2 m_EjectVelocity = {0.0f, 0.0f};
    float m_EjectTimer = 0.0f;
    glm::vec2 m_Velocity = {0.0f, 0.0f};

public:
    explicit DashEnemy(const glm::vec2& startPos, const glm::vec2& ejectVelocity = {0.0f, 0.0f})
        : Enemy(startPos, RESOURCE_DIR "/Image/enemy/dash.png", 3) {
        m_Transform.translation = startPos;
        m_EjectVelocity = ejectVelocity;
        m_ZIndex = 10;
        m_CanShoot = false;
    }

    void Update(const glm::vec2& playerPos) override {
        if (m_State == State::EJECTING) {
            m_Transform.translation += m_EjectVelocity;
            m_EjectTimer += 1.0f;
            // 彈射 15 幀後進入瞄準
            if (m_EjectTimer >= 15.0f) {
                m_State = State::AIMING;
            }
        } else if (m_State == State::AIMING) {
            float targetAngle = std::atan2(playerPos.y - m_Transform.translation.y,
                                           playerPos.x - m_Transform.translation.x);
            m_Transform.rotation = targetAngle + 1.570796f;

            m_AimTimer += 1.0f;
            if (m_AimTimer >= 30.0f) {
                m_Velocity = {
                    std::cos(targetAngle) * 12.0f,
                    std::sin(targetAngle) * 12.0f
                };
                m_State = State::DASHING;
            }
        } else if (m_State == State::DASHING) {
            m_Transform.translation += m_Velocity;
        }

        Enemy::Update(playerPos);
    }
};

#endif

